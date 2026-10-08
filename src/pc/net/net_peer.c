/* net_peer.c - direct-connect co-op transport (ONLINE=1 builds only, docs/network.md 1a / 3.4).
 *
 * On the PS2 every player's in-quest packets go by TCP to Capcom's game server, which only
 * forwards each player's records to the other members (docs/network.md 1a). Here the hosting
 * player's game does that forwarding: joiners open one TCP connection to the host, the host
 * passes every frame on to the other joiners and also hands it to its own game.
 *
 * Frame (both directions, little endian): u16 n, u8 from (sender slot), u8 type, n - 2 bytes.
 *   type 1..10   a game packet for AQ channel `type` (net_send_pl / _em / _sys ...; rt_np.c)
 *   NP_HELLO     joiner -> host: the joiner's mini data (NP_MINI bytes, Lb_set_mini_data's layout)
 *   NP_WELCOME   host -> joiner: u8 your slot
 *   NP_START     host -> all: u8 quest, u8 players, the mini data of each slot
 *   NP_BYE       a peer leaves
 *   NP_READY     u8 round: this player has loaded the stage (the start barrier, as net_start_ck)
 *
 * Safety (CLAUDE.md): only loopback / private addresses (net_dest_allowed in net_cpinet.c,
 * which also refuses the MH Oldschool addresses); the host listens on 127.0.0.1 unless given a
 * private address to bind; joiners are accepted only from loopback / private addresses or ones allowed on
 * purpose (--allow, RT_NET_ALLOW).
 */
#include "../rt/rt_plat.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "net_cpinet.h"
#include "net_peer.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET hsock;
#define HS_BAD INVALID_SOCKET
#define hs_close closesocket
#define WOULDBLOCK() (WSAGetLastError() == WSAEWOULDBLOCK)
#else
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
typedef int hsock;
#define HS_BAD (-1)
#define hs_close close
#define WOULDBLOCK() (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS)
#endif

int net_dest_allowed(uint32_t addr);
#if defined(MSG_NOSIGNAL)
#define SEND_FLAGS MSG_NOSIGNAL
#else
#define SEND_FLAGS 0
#endif

#define RXCAP 0x10000
typedef struct {
    hsock fd;
    int up;
    uint8_t rx[RXCAP];
    int rxn;
} PEER;

static int role;                    /* 0 off, 1 host, 2 joiner */
static hsock lsock = HS_BAD;        /* host: listening socket */
static PEER peer[NP_MAX];           /* host: [slot] for joiners (slot 0 unused); joiner: [0] = the host */
static int my_slot, nplayers = 1, quest, started;
static uint8_t minis[NP_MAX][NP_MINI];
static int gone[NP_MAX];
static int ready[NP_MAX];          /* the last start-barrier round each player reached */           /* players who left (or whose connection broke) */

/* received game packets, in order */
#define QCAP 512
static struct { uint8_t from, type; uint16_t n; uint8_t d[NP_PKT_MAX]; } q[QCAP];
static int qh, qt;

static void nonblock(hsock s)
{
#ifdef _WIN32
    u_long one = 1;
    ioctlsocket(s, FIONBIO, &one);
#else
    fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
#endif
}

static void nodelay(hsock s)
{
    int one = 1;
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char *)&one, sizeof one);
}

static void relay(const uint8_t *f, int len, int except);
static int frame(uint8_t *out, int from, int type, const void *d, int n);
static void drop(PEER *p, const char *why)
{
    int was_up = p->up;
    if (p->fd != HS_BAD)
        hs_close(p->fd);
    p->fd = HS_BAD;
    if (p->up)
        fprintf(stderr, "net_peer: connection closed (%s)\n", why);
    p->up = 0;
    p->rxn = 0;
    if (was_up && role == 1 && p != &peer[0]) {     /* host: a joiner left; tell the others */
        int s = (int)(p - peer);
        uint8_t f[4];
        gone[s] = 1;
        relay(f, frame(f, s, NP_BYE, NULL, 0), s);
    } else if (was_up && role == 2) {               /* joiner: the host is gone, and with it everyone */
        int s;
        for (s = 0; s < NP_MAX; s++)
            if (s != my_slot)
                gone[s] = 1;
    }
}

/* send all of buf (blocking briefly: frames are small and the link is local) */
static int send_all(PEER *p, const uint8_t *buf, int len)
{
    int off = 0, spins = 0;
    while (p->up && off < len) {
        int k = (int)send(p->fd, (const char *)buf + off, len - off, SEND_FLAGS);   /* no SIGPIPE when the peer is gone */
        if (k > 0) {
            off += k;
            continue;
        }
        if (k < 0 && WOULDBLOCK() && spins++ < 2000) {
#ifdef _WIN32
            Sleep(1);
#else
            {
                struct timespec ts = { 0, 1000000L };
                nanosleep(&ts, NULL);
            }
#endif
            continue;
        }
        drop(p, "send failed");
        return -1;
    }
    return 0;
}

static int frame(uint8_t *out, int from, int type, const void *d, int n)
{
    int len = n + 2;
    out[0] = (uint8_t)len;
    out[1] = (uint8_t)(len >> 8);
    out[2] = (uint8_t)from;
    out[3] = (uint8_t)type;
    if (n)
        memcpy(out + 4, d, n);
    return n + 4;
}

static void enqueue(int from, int type, const uint8_t *d, int n)
{
    int nx = (qt + 1) % QCAP;
    if (n > NP_PKT_MAX || nx == qh)
        return;             /* too big or the queue is full: dropped (logged by the caller's counters) */
    q[qt].from = (uint8_t)from;
    q[qt].type = (uint8_t)type;
    q[qt].n = (uint16_t)n;
    memcpy(q[qt].d, d, n);
    qt = nx;
}

/* host: pass a frame on to every joiner except `except` */
static void relay(const uint8_t *f, int len, int except)
{
    int s;
    for (s = 1; s < NP_MAX; s++)
        if (s != except && peer[s].up)
            send_all(&peer[s], f, len);
}

static void handle(int src, int from, int type, const uint8_t *d, int n)
{
    if (role == 1) {
        if (type == NP_HELLO) {
            memcpy(minis[src], d, n < NP_MINI ? n : NP_MINI);
            fprintf(stderr, "net_peer: player %d joined\n", src);
            return;
        }
        if (type == NP_BYE) {
            drop(&peer[src], "left");
            return;
        }
        if (type == NP_READY && n >= 1)
            ready[src] = d[0];
        from = src;     /* the host knows who sent it */
        {
            uint8_t f[NP_PKT_MAX + 4];
            int len = frame(f, from, type, d, n);
            relay(f, len, src);
        }
        enqueue(from, type, d, n);
        return;
    }
    switch (type) {
    case NP_WELCOME:
        if (n >= 1)
            my_slot = d[0];
        break;
    case NP_START:
        if (n >= 2) {
            int k;
            quest = d[0];
            nplayers = d[1];
            for (k = 0; k < nplayers && k < NP_MAX && 2 + NP_MINI * (k + 1) <= n; k++)
                memcpy(minis[k], d + 2 + NP_MINI * k, NP_MINI);
            started = 1;
        }
        break;
    case NP_READY:
        if (n >= 1 && from >= 0 && from < NP_MAX)
            ready[from] = d[0];
        break;
    case NP_BYE:
        fprintf(stderr, "net_peer: player %d left\n", from);
        if (from >= 0 && from < NP_MAX)
            gone[from] = 1;
        break;
    default:
        enqueue(from, type, d, n);
    }
}

static void read_peer(int src)
{
    PEER *p = &peer[src];
    for (;;) {
        int k;
        if (!p->up)
            return;
        k = (int)recv(p->fd, (char *)p->rx + p->rxn, RXCAP - p->rxn, 0);
        if (k == 0) {
            drop(p, "peer closed");
            return;
        }
        if (k < 0) {
            if (!WOULDBLOCK())
                drop(p, "recv failed");
            break;
        }
        p->rxn += k;
        for (;;) {
            int len;
            if (p->rxn < 2)
                break;
            len = p->rx[0] | p->rx[1] << 8;
            if (len < 2 || len > NP_PKT_MAX + 2) {
                drop(p, "bad frame");
                return;
            }
            if (p->rxn < len + 2)
                break;
            handle(src, p->rx[2], p->rx[3], p->rx + 4, len - 2);
            if (!p->up)         /* the frame closed the connection (a goodbye) */
                return;
            memmove(p->rx, p->rx + len + 2, p->rxn - (len + 2));
            p->rxn -= len + 2;
        }
    }
}

static int parse_addr(const char *s, uint32_t *a)
{
    uint32_t v = InetIPAddrFromString(s);   /* first octet in the low byte, as the PS2 stores it */
    if (!v && strcmp(s, "0.0.0.0"))
        return -1;
    *a = v;
    return 0;
}

int np_host(const char *bind_ip, int port, const uint8_t *my_mini)
{
    struct sockaddr_in sa;
    uint32_t a;
    int one = 1, s;
    if (CpInetInitialize() != 0)
        return -1;
    if (!bind_ip)
        bind_ip = "0.0.0.0";       /* every interface: who may join is checked per connection (net_dest_allowed) */
    if (parse_addr(bind_ip, &a) != 0 || (a != 0 && !net_dest_allowed(a))) {    /* 0.0.0.0: every interface (joiners are checked) */
        fprintf(stderr, "net_peer: will not listen on %s (only loopback or a private LAN address)\n", bind_ip);
        return -1;
    }
    for (s = 0; s < NP_MAX; s++)
        peer[s].fd = HS_BAD;
    lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lsock == HS_BAD)
        return -1;
    setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof one);
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_port = htons((uint16_t)port);
    memcpy(&sa.sin_addr, &a, 4);
    if (bind(lsock, (struct sockaddr *)&sa, sizeof sa) != 0 || listen(lsock, 4) != 0) {
        fprintf(stderr, "net_peer: cannot listen on %s:%d\n", bind_ip, port);
        hs_close(lsock);
        lsock = HS_BAD;
        return -1;
    }
    nonblock(lsock);
    role = 1;
    my_slot = 0;
    nplayers = 1;
    memcpy(minis[0], my_mini, NP_MINI);
    fprintf(stderr, "net_peer: hosting on %s:%d\n", bind_ip, port);
    return 0;
}

int np_join(const char *host_ip, int port, const uint8_t *my_mini)
{
    struct sockaddr_in sa;
    uint32_t a;
    uint8_t f[NP_MINI + 4];
    int s;
    if (CpInetInitialize() != 0)
        return -1;
    if (parse_addr(host_ip, &a) != 0) {
        fprintf(stderr, "net_peer: %s is not an IPv4 address\n", host_ip);
        return -1;
    }
    if (!net_dest_allowed(a))
        return -1;
    for (s = 0; s < NP_MAX; s++)
        peer[s].fd = HS_BAD;
    peer[0].fd = socket(AF_INET, SOCK_STREAM, 0);
    if (peer[0].fd == HS_BAD)
        return -1;
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_port = htons((uint16_t)port);
    memcpy(&sa.sin_addr, &a, 4);
    fprintf(stderr, "net_peer: connecting to %s:%d\n", host_ip, port);
    if (connect(peer[0].fd, (struct sockaddr *)&sa, sizeof sa) != 0) {
        fprintf(stderr, "net_peer: cannot connect to %s:%d\n", host_ip, port);
        hs_close(peer[0].fd);
        peer[0].fd = HS_BAD;
        return -1;
    }
    nonblock(peer[0].fd);
    nodelay(peer[0].fd);
    peer[0].up = 1;
    role = 2;
    memcpy(minis[0], my_mini, NP_MINI);     /* (overwritten by the host's START) */
    send_all(&peer[0], f, frame(f, 0xFF, NP_HELLO, my_mini, NP_MINI));
    fprintf(stderr, "net_peer: connected to %s:%d\n", host_ip, port);
    return 0;
}

void np_poll(void)
{
    int s;
    if (role == 1) {
        for (;;) {
            struct sockaddr_in sa;
            socklen_t sl = sizeof sa;
            hsock c = accept(lsock, (struct sockaddr *)&sa, &sl);
            uint8_t f[8], slot;
            if (c == HS_BAD)
                break;
            {   /* a joiner from the internet only when his address is allowed (as for connecting out) */
                uint32_t from;
                memcpy(&from, &sa.sin_addr, 4);
                if (!net_dest_allowed(from)) {
                    hs_close(c);
                    continue;
                }
            }
            for (s = 1; s < NP_MAX && (peer[s].up || s < nplayers); s++)
                ;
            if (started || s >= NP_MAX) {      /* full, or the quest has started: no late joining */
                hs_close(c);
                continue;
            }
            nonblock(c);
            nodelay(c);
            peer[s].fd = c;
            peer[s].up = 1;
            peer[s].rxn = 0;
            nplayers = s + 1;
            slot = (uint8_t)s;
            send_all(&peer[s], f, frame(f, 0, NP_WELCOME, &slot, 1));
        }
        for (s = 1; s < NP_MAX; s++)
            read_peer(s);
    } else if (role == 2)
        read_peer(0);
}

int np_host_start(int quest_no)
{
    uint8_t d[2 + NP_MINI * NP_MAX], f[sizeof d + 4];
    int k, len;
    if (role != 1)
        return -1;
    quest = quest_no;
    d[0] = (uint8_t)quest_no;
    d[1] = (uint8_t)nplayers;
    for (k = 0; k < nplayers; k++)
        memcpy(d + 2 + NP_MINI * k, minis[k], NP_MINI);
    len = frame(f, 0, NP_START, d, 2 + NP_MINI * nplayers);
    relay(f, len, -1);
    started = 1;
    return 0;
}

int np_send(int type, const void *d, int n)
{
    uint8_t f[NP_PKT_MAX + 4];
    int len;
    if (role == 0 || n > NP_PKT_MAX)
        return -1;
    len = frame(f, my_slot, type, d, n);
    if (role == 1)
        relay(f, len, -1);
    else
        send_all(&peer[0], f, len);
    return 0;
}

int np_recv(int *from, int *type, uint8_t *buf, int max)
{
    int n;
    if (qh == qt)
        return -1;
    n = q[qh].n < max ? q[qh].n : max;
    *from = q[qh].from;
    *type = q[qh].type;
    memcpy(buf, q[qh].d, n);
    qh = (qh + 1) % QCAP;
    return n;
}

/* a packet of this machine's own, back to itself (mcsls loops a player's own app data back
 * locally; self_data_ctrl then uses the supply-box channel's own packets) */
void np_loopback(int type, const void *d, int n)
{
    enqueue(my_slot, type, d, n);
}

int np_ready(int round)
{
    uint8_t r = (uint8_t)round;
    int s, n = 0;
    if (ready[my_slot] != round) {
        ready[my_slot] = round;
        np_send(NP_READY, &r, 1);
    }
    for (s = 0; s < nplayers; s++)
        n += ready[s] == round || gone[s];
    return n;
}

int np_role(void) { return role; }
int np_slot(void) { return my_slot; }
int np_players(void) { return nplayers; }
int np_started(void) { return started; }
int np_quest(void) { return quest; }
const uint8_t *np_mini(int slot) { return minis[slot & (NP_MAX - 1)]; }
int np_gone(int slot) { return slot >= 0 && slot < NP_MAX && gone[slot]; }
int np_connected(int slot)
{
    if (role == 1)
        return slot == 0 || (slot > 0 && slot < NP_MAX && peer[slot].up);
    return role == 2 && peer[0].up;
}

void np_close(void)
{
    uint8_t f[4];
    int s;
    for (s = 0; s < NP_MAX; s++)
        if (peer[s].up) {
            send_all(&peer[s], f, frame(f, my_slot, NP_BYE, NULL, 0));
            peer[s].up = 0;
            drop(&peer[s], "closing");
        }
    if (lsock != HS_BAD)
        hs_close(lsock);
    lsock = HS_BAD;
    role = 0;
}
