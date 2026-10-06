/* lb_bz90 - lobby UI/client 0x005BCC80-0x005BCDD4: To_LobbyExit (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void To_LobbyExit(s32 arg0) {
    s32 temp_v0;
    s32 var_a0;

    var_a0 = arg0;
    temp_v0 = var_a0 & 0xFF;
    if (temp_v0 != 3) {
        var_a0 = 2;
        switch (temp_v0) {                          /* irregular */
        case 0:
            F(s8, (u8 *)cw, 0x2C31) = 3;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 3;
            F(s8, (u8 *)cw, 0x2C34) = 1;
            F(s8, (u8 *)cw, 0x2C35) = 2;
            break;
        case 1:
            F(s8, (u8 *)cw, 0x2C31) = 3;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 3;
            F(s8, (u8 *)cw, 0x2C34) = 1;
            F(s8, (u8 *)cw, 0x2C35) = 0;
            break;
        case 2:
            F(s8, (u8 *)cw, 0x2C31) = 3;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 3;
            F(s8, (u8 *)cw, 0x2C34) = 2;
            F(s8, (u8 *)cw, 0x2C35) = 0;
            break;
        }
    } else {
        F(s8, (u8 *)cw, 0x2C31) = 3;
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 3;
        F(s8, (u8 *)cw, 0x2C34) = 2;
        F(s8, (u8 *)cw, 0x2C35) = 2;
    }
    Init_InterruptFlag(var_a0);
    F(s8, (u8 *)cw, 0x2C30) = 0;
    F(u8, (u8 *)cw, 0x2F79) = 0xFF;
    F(s8, (u8 *)cw, 0x2F78) = 0;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    cnLbc_EraseDialog(0x4C);
    SoftKeyboard_exit();
    F(s8, (u8 *)cw, 0x2C0A) = 2;
    F(s8, (u8 *)cw, 0x2C0B) = 2;
}
