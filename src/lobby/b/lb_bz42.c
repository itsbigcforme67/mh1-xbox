/* lb_bz42 - lobby UI/client 0x005BE130-0x005BE14C: lbc_game_ready_03 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void lbc_game_ready_03(void) {
    void *temp_a0;

    temp_a0 = (u8 *)cw;
    F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
    F(s8, (u8 *)cw, 0x2C34) = 0;
}
