/* lb_bz85 - lobby UI/client 0x005B9B70-0x005B9BA8: To_TopMenu (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void To_TopMenu(void) {
    F(s8, (u8 *)cw, 0x2C31) = 1;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    F(s8, (u8 *)cw, 0x2C3F) = 0;
    Init_InterruptFlag();
}
