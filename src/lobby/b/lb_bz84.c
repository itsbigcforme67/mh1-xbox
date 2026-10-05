/* lb_bz84 - lobby UI/client 0x005B96D0-0x005B9708: To_MyLobby (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void To_MyLobby(void) {
    F(s8, (u8 *)cw, 0x2C31) = 8;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    F(s8, (u8 *)cw, 0x2C3F) = 0;
    Init_InterruptFlag();
}
