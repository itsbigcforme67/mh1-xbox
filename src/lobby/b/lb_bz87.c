/* lb_bz87 - lobby UI/client 0x005BB520-0x005BB5BC: To_PlazaExit (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void To_PlazaExit(s32 arg0) {
    if (arg0 == 0) {
        F(s8, (u8 *)cw, 0x2C31) = 2;
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 3;
        F(s8, (u8 *)cw, 0x2C34) = 2;
    } else {
        F(s8, (u8 *)cw, 0x2C31) = 2;
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 3;
        F(s8, (u8 *)cw, 0x2C34) = 0;
    }
    Init_InterruptFlag();
    F(s8, (u8 *)cw, 0x2C30) = 0;
    F(u8, (u8 *)cw, 0x2F79) = 0xFF;
    F(s8, (u8 *)cw, 0x2F78) = 0;
    F(s8, (u8 *)cw, 0x2C09) = 2;
}
