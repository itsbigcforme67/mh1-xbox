/* lb_bz93 - lobby UI/client 0x005BE2D0-0x005BE2F8: To_MatchingFailed (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void To_MatchingFailed(arg0)
int arg0;
{
    F(s8, (u8 *)cw, 0x2C31) = 6;
    F(s8, (u8 *)cw, 0x2C32) = arg0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    Init_InterruptFlag(arg0);
}
