/* lb_bz45 - lobby UI/client 0x005BEDE0-0x005BEE18: To_GoToTop (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void To_GoToTop(void) {
    F(s8, (u8 *)cw, 0x2C31) = 7;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    F(s8, (u8 *)cw, 0x2C36) = 0;
}
