/* lb_bz133 - lobby UI/client 0x005C4D40-0x005C4D8C: Lobby_main (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char lobby_main_func_72[];

s32 Lobby_main(void) {
    s32 temp_s0;

    temp_s0 = ((int (**)())&lobby_main_func_72)[F(s8, &lb_sys, 1)]();
    trans();
    return temp_s0;
}
