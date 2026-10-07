/* net_cpinet.c - host side of the game's network layer (docs/network.md).
 *
 * The PS2 game reaches the network through Capcom's CpInet* wrappers, which call the
 * IOP TCP/IP stack by RPC (Ave_* -> sceSifCallRpc). That stack is Sony/third-party IOP
 * code and is not decompiled; the port replaces it at the CpInet* boundary with this file,
 * which implements the entry points the game's lobby / cnet code uses on BSD sockets
 * (POSIX) or Winsock. The wire protocol above TCP is untouched: the game's own cnet C
 * builds and parses the packets.
 *
 * Only linked into ONLINE=1 builds (tools/build_pc.sh). It never connects anywhere on its
 * own: the target comes from the caller (rt_net.c: RT_NET_HOST / RT_NET_PORT, default
 * 127.0.0.1). Safety rules enforced here:
 *   - destinations outside loopback / private ranges are refused unless RT_NET_ALLOW_PUBLIC=1;
 *   - the known MH Oldschool addresses are refused unconditionally (the project may not
 *     connect to that public server without its operators' permission, CLAUDE.md).
 *
 * Return conventions follow the original wrappers (cpinet*.c, ave*.c):
 *   TcpOpen   -> socket handle >= 0, or < 0 on error
 *   TcpGetStatus(sock, st): st[0] = state (4 = established, 2 = connecting, 0 = closed,
 *             11 = error), st[2] = free send space, st[3] = bytes waiting; returns 0 / < 0
 *   TcpRecv   -> bytes copied (0 = nothing yet), < 0 on error
 *   TcpSend   -> bytes accepted, < 0 on error
 *   DnsLookUp -> 1 done, -3 pending, -2 / -1 error
 */
#include "../rt/rt_plat.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "net_cpinet.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET hsock;
#define HS_BAD INVALID_SOCKET
#define hs_close closesocket
#define hs_err() WSAGetLastError()
#define E_INPROGRESS WSAEWOULDBLOCK
#define E_WOULDBLOCK WSAEWOULDBLOCK
#else
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
typedef int hsock;
#define HS_BAD (-1)
#define hs_close close
#define hs_err() errno
#define E_INPROGRESS EINPROGRESS
#define E_WOULDBLOCK EWOULDBLOCK
#endif

#define MAXSOCK 8
#define MAXTICKET 8

enum { S_FREE, S_CONNECTING, S_UP, S_CLOSED, S_ERROR };

typedef struct {
    int state;
    hsock fd;
    uint32_t addr;          /* network byte order as stored in memory (first octet in the low byte) */
    uint16_t port;          /* network byte order */
} NETSOCK;

typedef struct {
    int used, done, ok;
    uint32_t addr;
} DNSTICKET;

static NETSOCK socks[MAXSOCK];
static DNSTICKET tickets[MAXTICKET];
static int net_up;
static uint64_t stat_tx, stat_rx;

void net_log(const char *fmt, ...);

/* ---- destination policy ---- */

static const uint8_t mho_deny[][4] = {      /* MH Oldschool public infrastructure (docs/network.md); never contacted */
    { 34, 75, 107, 68 }, { 151, 80, 238, 99 }, { 151, 80, 238, 101 }, { 151, 80, 238, 104 },
};

static int dest_allowed(uint32_t addr)
{
    const uint8_t *b = (const uint8_t *)&addr;
    size_t i;
    for (i = 0; i < sizeof mho_deny / sizeof mho_deny[0]; i++)
        if (!memcmp(b, mho_deny[i], 4)) {
            fprintf(stderr, "net: refusing %u.%u.%u.%u: MH Oldschool servers may not be contacted without their operators' permission\n",
                    b[0], b[1], b[2], b[3]);
            return 0;
        }
    if (b[0] == 127 || b[0] == 10 || (b[0] == 192 && b[1] == 168) || (b[0] == 172 && (b[1] & 0xF0) == 16) ||
        (b[0] == 169 && b[1] == 254))
        return 1;
    if (getenv("RT_NET_ALLOW_PUBLIC") && atoi(getenv("RT_NET_ALLOW_PUBLIC")) == 1)
        return 1;
    fprintf(stderr, "net: refusing %u.%u.%u.%u: only loopback and private addresses unless RT_NET_ALLOW_PUBLIC=1\n",
            b[0], b[1], b[2], b[3]);
    return 0;
}

/* ---- helpers ---- */

static void sock_nonblock(hsock s)
{
#ifdef _WIN32
    u_long one = 1;
    ioctlsocket(s, FIONBIO, &one);
#else
    fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
#endif
}

static int sock_pending(hsock s)
{
#ifdef _WIN32
    u_long n = 0;
    if (ioctlsocket(s, FIONREAD, &n) != 0)
        return 0;
    return (int)n;
#else
    int n = 0;
    if (ioctl(s, FIONREAD, &n) != 0)
        return 0;
    return n;
#endif
}

/* writable / connected / failed check for a connecting socket */
static void sock_poll_connect(NETSOCK *n)
{
    fd_set w, e;
    struct timeval tv = { 0, 0 };
    int err = 0;
    socklen_t len = sizeof err;
    FD_ZERO(&w);
    FD_ZERO(&e);
    FD_SET(n->fd, &w);
    FD_SET(n->fd, &e);
    if (select((int)n->fd + 1, NULL, &w, &e, &tv) <= 0)
        return;
    if (getsockopt(n->fd, SOL_SOCKET, SO_ERROR, (char *)&err, &len) != 0 || err != 0 || FD_ISSET(n->fd, &e)) {
        n->state = S_ERROR;
        return;
    }
    if (FD_ISSET(n->fd, &w))
        n->state = S_UP;
}

static NETSOCK *get(int h)
{
    if (h < 0 || h >= MAXSOCK || socks[h].state == S_FREE)
        return NULL;
    return &socks[h];
}

/* ---- init ---- */

int CpInetInitialize(void)
{
#ifdef _WIN32
    WSADATA wd;
    if (!net_up && WSAStartup(MAKEWORD(2, 2), &wd) != 0)
        return -1;
#endif
    net_up = 1;
    return 0;
}

int CpInetTerminate(void)
{
    int i;
    for (i = 0; i < MAXSOCK; i++)
        if (socks[i].state != S_FREE) {
            hs_close(socks[i].fd);
            socks[i].state = S_FREE;
        }
#ifdef _WIN32
    if (net_up)
        WSACleanup();
#endif
    net_up = 0;
    return 0;
}

int CpInetTcpInitialize(int kind, int prov)
{
    (void)kind;
    (void)prov;
    return CpInetInitialize();
}

int CpInetTcpTerminate(void)
{
    return CpInetTerminate();
}

/* ---- TCP ---- */

int CpInetTcpOpen(CPINET_TCPOPEN *t)
{
    int i;
    struct sockaddr_in sa;
    hsock fd;
    if (!net_up || !t)
        return -1;
    if (!dest_allowed((uint32_t)t->addr))
        return -1;
    for (i = 0; i < MAXSOCK && socks[i].state != S_FREE; i++)
        ;
    if (i == MAXSOCK)
        return -1;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == HS_BAD)
        return -1;
    sock_nonblock(fd);
    {
        int one = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&one, sizeof one);
    }
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_port = t->port;
    memcpy(&sa.sin_addr, &t->addr, 4);
    socks[i].fd = fd;
    socks[i].addr = (uint32_t)t->addr;
    socks[i].port = t->port;
    if (connect(fd, (struct sockaddr *)&sa, sizeof sa) == 0)
        socks[i].state = S_UP;
    else if (hs_err() == E_INPROGRESS || hs_err() == E_WOULDBLOCK)
        socks[i].state = S_CONNECTING;
    else {
        hs_close(fd);
        socks[i].state = S_FREE;
        return -1;
    }
    {
        const uint8_t *b = (const uint8_t *)&t->addr;
        net_log("tcp open %d -> %u.%u.%u.%u:%u", i, b[0], b[1], b[2], b[3], ntohs(t->port));
    }
    return i;
}

int CpInetTcpGetStatus(int h, int16_t *st)
{
    NETSOCK *n = get(h);
    if (!n)
        return -1;
    if (n->state == S_CONNECTING)
        sock_poll_connect(n);
    st[2] = 0;
    st[3] = 0;
    switch (n->state) {
    case S_CONNECTING:
        st[0] = 2;
        break;
    case S_UP: {
        int p = sock_pending(n->fd);
        st[0] = 4;
        st[2] = 0x2000;                 /* send space: the game wants more than 0x400 */
        st[3] = (int16_t)(p > 0x7FFF ? 0x7FFF : p);
        if (p == 0) {                   /* peer closed? (readable with 0 bytes) */
            fd_set r;
            struct timeval tv = { 0, 0 };
            FD_ZERO(&r);
            FD_SET(n->fd, &r);
            if (select((int)n->fd + 1, &r, NULL, NULL, &tv) > 0) {
                char c;
                int k = recv(n->fd, &c, 1, MSG_PEEK);
                if (k == 0 || (k < 0 && hs_err() != E_WOULDBLOCK)) {
                    n->state = S_CLOSED;
                    st[0] = 0;
                    st[2] = 0;
                }
            }
        }
        break;
    }
    case S_CLOSED:
        st[0] = 0;
        break;
    default:
        st[0] = 11;
        break;
    }
    return 0;
}

int CpInetTcpRecv(int h, void *buf, int len)
{
    NETSOCK *n = get(h);
    int k;
    if (!n)
        return -1;
    if (len <= 0)
        return 0;
    if (n->state == S_CONNECTING)
        sock_poll_connect(n);
    if (n->state != S_UP)
        return n->state == S_CLOSED ? 0 : -1;
    if (len > 0x3CA)
        len = 0x3CA;                    /* one RPC transfer on the PS2 */
    k = (int)recv(n->fd, (char *)buf, len, 0);
    if (k > 0) {
        stat_rx += (unsigned)k;
        return k;
    }
    if (k == 0) {
        n->state = S_CLOSED;
        return 0;
    }
    if (hs_err() == E_WOULDBLOCK)
        return 0;
    n->state = S_ERROR;
    return -1;
}

int CpInetTcpSend(int h, const void *buf, int len)
{
    NETSOCK *n = get(h);
    int done = 0;
    if (!n)
        return -1;
    if (n->state == S_CONNECTING)
        sock_poll_connect(n);
    if (n->state != S_UP)
        return -1;
    while (done < len) {
        int k = (int)send(n->fd, (const char *)buf + done, len - done, 0);
        if (k > 0) {
            done += k;
            continue;
        }
        if (k < 0 && hs_err() == E_WOULDBLOCK) {
            fd_set w;
            struct timeval tv = { 1, 0 };
            FD_ZERO(&w);
            FD_SET(n->fd, &w);
            if (select((int)n->fd + 1, NULL, &w, NULL, &tv) <= 0) {
                n->state = S_ERROR;
                return -1;
            }
            continue;
        }
        n->state = S_ERROR;
        return -1;
    }
    stat_tx += (unsigned)done;
    return done;
}

int CpInetTcpClose(int *h)
{
    NETSOCK *n;
    if (!h)
        return -1;
    n = get(*h);
    if (n) {
        hs_close(n->fd);
        n->state = S_FREE;
    }
    *h = -1;
    return 0;
}

int CpInetTcpAbort(int h)
{
    NETSOCK *n = get(h);
    if (n && n->state != S_CLOSED)
        n->state = S_CLOSED;
    return 0;
}

int CpInetTcpDelete(int *h)
{
    return CpInetTcpClose(h);
}

int CpInetTcpGetOption(int h, int buf)
{
    (void)h;
    (void)buf;
    return 0;
}

int CpInetTcpSetOption(int h, int buf)
{
    (void)h;
    (void)buf;
    return 0;
}

/* ---- interface state: one always-up Ethernet device, no PPP / modem ---- */

int CpInetGetStatus(void) { return 0; }                 /* no interface problem */
int CpInetInterfaceProblemEnable(int on) { (void)on; return 0; }
int CpInetInterfaceGetStatus(void) { return 3; }        /* interface up */
int CpInetTcpNbCallEnd(void) { return 1; }
int CpInetDevChanged(int *n) { if (n) *n = 0; return 0; }
int CpInetDevSelect(int dev) { (void)dev; return 0; }

/* ---- DNS ---- */

uint32_t InetIPAddrFromString(const char *s)
{
    /* dotted quad only (the original returns 0 for anything else); first octet in the low byte */
    unsigned o[4] = { 0, 0, 0, 0 };
    int part = 0, digits = 0;
    const char *p;
    for (p = s; *p; p++) {
        if (*p >= '0' && *p <= '9') {
            o[part] = o[part] * 10 + (unsigned)(*p - '0');
            if (++digits > 3 || o[part] > 255)
                return 0;
        } else if (*p == '.' && digits > 0 && part < 3) {
            part++;
            digits = 0;
        } else
            return 0;
    }
    if (part != 3 || digits == 0)
        return 0;
    return o[0] | (o[1] << 8) | (o[2] << 16) | ((uint32_t)o[3] << 24);
}

int CpInetDnsInitialize(void) { return 0; }
int CpInetDnsDispose(void) { return 0; }

int CpInetDnsGetTicket(const char *name)
{
    int i;
    struct addrinfo hints, *res = NULL;
    for (i = 0; i < MAXTICKET && tickets[i].used; i++)
        ;
    if (i == MAXTICKET)
        return -1;
    tickets[i].used = 1;
    tickets[i].done = 1;
    tickets[i].ok = 0;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(name, NULL, &hints, &res) == 0 && res) {
        memcpy(&tickets[i].addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr, 4);
        tickets[i].ok = 1;
    }
    if (res)
        freeaddrinfo(res);
    return i;
}

int CpInetDnsLookUp(int t, uint32_t *ip)
{
    if (t < 0 || t >= MAXTICKET || !tickets[t].used)
        return -1;
    if (!tickets[t].done)
        return -3;
    if (!tickets[t].ok)
        return -2;
    *ip = tickets[t].addr;
    return 1;
}

int CpInetDnsReleaseTicket(int t)
{
    if (t >= 0 && t < MAXTICKET)
        tickets[t].used = 0;
    return 0;
}

/* ---- statistics for the test driver ---- */

void net_stats(unsigned long long *tx, unsigned long long *rx)
{
    *tx = stat_tx;
    *rx = stat_rx;
}

void net_log(const char *fmt, ...)
{
    extern void rt_log(const char *fmt, ...);
    char b[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    rt_log("net: %s", b);
}
