#include "lobby_a.h"
extern char lit_547_0065E850[];
extern char jtbl_548_0065E870[];
extern char jtbl_548_0065E870[];
void lbc_login_warning_message(void) {
    s32 temp_t0;
    u8 temp_a1;
    int temp_a0;
    int temp_a0_2;
    int temp_a1_2;
    int temp_a1_3;
    int temp_a2;
    int temp_a3;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;

    temp_a2 = (int)cw;
    temp_t0 = Get_sw2(0) & 0xFFFF;
    temp_a1 = F(u8, temp_a2, 0x2C34);
    temp_a3 = temp_a2 + 0x2C34;
    switch (temp_a1) {
    case 0:
        if (check_warning_level(F(u8, temp_a2, 0x35FE)) == 0) {
            F(u8, (u8 *)cw, 0x2C34) = 7U;
            CallBackWaitInit();
            cnLBS_Answer_LoginWarningMessage(0);
            return;
        }
        temp_a0 = (int)cw;
        F(u8, temp_a0, 0x2C34) = (u8) (F(u8, temp_a0, 0x2C34) + 1);
        return;
    case 1:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        /* fallthrough */
    case 2:
        temp_a1_2 = (int)cw;
        F(u8, temp_a1_2, 0x2C34) = (u8) (F(u8, temp_a1_2, 0x2C34) + 1);
        F(s32, (u8 *)cw, 0x2C4C) = 0x3C;
        return;
    case 3:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        temp_v1 = (int)cw;
        if (F(u8, temp_v1, 0x35FE) != 0) {
            F(s32, temp_v1, 0x2C50) = 8;
            SetDialogData_HTML((u8 *)cw + 0x3602, temp_a1, temp_a2, temp_a3);
            return;
        }
        To_LogOut(1);
        return;
    case 4:
        F(s8, pNet, 0xC) = 1;
        temp_a1_3 = (int)cw;
        if (F(u16, temp_a1_3, 0x3600) != 0) {
            F(s32, temp_a1_3, 0x2C4C) = (F(s32, temp_a1_3, 0x2C4C) - 1);
            temp_v1_2 = (int)cw;
            if (0 >= (s8) F(s32, temp_v1_2, 0x2C4C)) {
                F(s32, temp_v1_2, 0x2C4C) = 0x3C;
                temp_a0_2 = (int)cw;
                F(u16, temp_a0_2, 0x3600) = (u16) (F(u16, temp_a0_2, 0x3600) - 1);
                temp_v1_3 = (int)cw;
                if (0 > (s16) F(u16, temp_v1_3, 0x3600)) {
                    F(u16, temp_v1_3, 0x3600) = 0U;
                    return;
                }
            }
        default:
            return;
        }
        if (temp_t0 & 0xFFFF & 0x20) {
            F(u8, temp_a1_3, 0x2C34) = (u8) (F(u8, temp_a1_3, 0x2C34) + 1);
            cnWrap_SoundRequest(0);
        }
        cnWrap_SetFontColor(5);
        flfntSetSize(0x14, 0x14);
        cnWrap_FontDisp(0x43520000, 0x40000000, &lit_547_0065E850);
        return;
    case 5:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        return;
    case 6:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        CallBackWaitInit();
        cnLBS_Answer_LoginWarningMessage(1);
        return;
    case 7:
        Check_CallBackWait(&jtbl_548_0065E870);
        break;
    }
}
