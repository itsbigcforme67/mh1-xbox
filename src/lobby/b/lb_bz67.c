/* lb_bz67 - lobby UI/client 0x005B2330-0x005B236C: cnWrap_InitWork (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void cnWrap_InitWork(void) {
    F(s8, (u8 *)cw, 0x2C30) = 0;
    F(u8, (u8 *)cw, 0x2F79) = 0xFF;
    F(s8, (u8 *)cw, 0x2F78) = 0;
    init_set_work();
    Net_all_reset(0);
}
