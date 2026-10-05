/* lb_bz40 - lobby UI/client 0x005BDCF0-0x005BDD44: To_ReadyBattle (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void To_ReadyBattle(void) {
    F(s8, (u8 *)cw, 0x2C31) = 4;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    Init_InterruptFlag();
    F(s8, (u8 *)cw, 0x2C30) = 0;
    F(s8, (u8 *)cw, 0x35D5) = 0;
    cnLbc_EraseDialog(0x4C);
}
