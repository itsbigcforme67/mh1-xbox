/* net_cpinet.h - the host network backend (net_cpinet.c, docs/network.md). */
#ifndef NET_CPINET_H
#define NET_CPINET_H
#include <stdint.h>

/* the game's TCPOPEN struct (cpinet08.c): address as the PS2 stores it (first octet in the
 * low byte), ports in network byte order */
typedef struct {
    int32_t addr;
    uint16_t port;
    uint16_t lport;
} CPINET_TCPOPEN;

int CpInetInitialize(void);
int CpInetTerminate(void);
int CpInetTcpOpen(CPINET_TCPOPEN *t);
int CpInetTcpGetStatus(int sock, int16_t *st);
int CpInetTcpRecv(int sock, void *buf, int len);
int CpInetTcpSend(int sock, const void *buf, int len);
int CpInetTcpClose(int *sock);
uint32_t InetIPAddrFromString(const char *s);
void net_stats(unsigned long long *tx, unsigned long long *rx);
void net_allow_server(const char *host);    /* the server the player configured: its addresses count as allowed */
#endif
