/* lb_bz97 - lobby UI/client 0x005C4D90-0x005C4DCC: server_select, lobby_top (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { s8 x00; s8 x01; } LBS1;

int server_select() {
    ((LBS1 *)&lb_sys)->x01++;
    return 0;
}

int lobby_top() {
    ((LBS1 *)&lb_sys)->x01++;
    return 0;
}
