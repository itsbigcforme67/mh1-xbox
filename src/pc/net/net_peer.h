/* net_peer.h - direct-connect co-op transport (net_peer.c, docs/network.md 3.4). */
#ifndef NET_PEER_H
#define NET_PEER_H
#include <stdint.h>

#define NP_MAX 4            /* players in a quest (PS2: 4) */
#define NP_PKT_MAX 0x200    /* largest game packet carried */
#define NP_DEFAULT_PORT 10300

enum { NP_HELLO = 0x40, NP_WELCOME, NP_START, NP_BYE };

int np_host(const char *bind_ip, int port, int my_weapon);  /* listen; slot 0 */
int np_join(const char *host_ip, int port, int my_weapon);  /* connect to the host */
void np_poll(void);                     /* accept / read / relay; call every tick */
int np_host_start(int quest_no);        /* host: tell everyone the quest, the slots, their weapons */
int np_send(int type, const void *d, int n);    /* a game packet to every other player */
int np_recv(int *from, int *type, uint8_t *buf, int max);  /* next received game packet, -1 none */
int np_role(void);                      /* 0 off, 1 host, 2 joiner */
int np_slot(void);
int np_players(void);
int np_started(void);
int np_quest(void);
int np_weapon(int slot);
int np_connected(int slot);
int np_gone(int slot);                  /* that player left the session */
void np_close(void);
#endif
