#include "lobby_a.h"
extern s8 COM_R_No_0;
extern s8 COM_R_No_1;
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;
extern s8 COM_R_No_4;
extern s8 MMBB_LOGIN;
extern u8 net_char_change;
extern char jtbl_3219[];
extern char text_lobby_trans_ot0[];
extern char jtbl_3219[];
extern char jtbl_3219[];
extern char D_4E36F4[];
extern char jtbl_3219[];
void lbc_logout_01(void) {
    s32 temp_a0_3;
    s32 temp_a0_6;
    s32 temp_a1;
    s32 temp_v1_5;
    u8 temp_v1;
    int temp_a0;
    int temp_a0_2;
    int temp_a0_4;
    int temp_a0_5;
    int temp_a1_2;
    int temp_v1_2;
    int temp_v1_3;
    int temp_v1_4;
    int temp_v1_6;

    temp_v1 = F(u8, (u8 *)cw, 0x2C34);
    temp_a1 = Get_sw2(0) & 0xFFFF;
    switch (temp_v1) {
    case 0:
        Lbc_init_network_work();
        F(s8, pNet, 0x11) = 1;
        temp_v1_2 = (int)cw;
        F(u8, temp_v1_2, 0x2C34) = (u8) (F(u8, temp_v1_2, 0x2C34) + 1);
        Lbc_set_prim(&text_lobby_trans_ot0, 0, 0);
        if (F(u8, (u8 *)cw, 0x2C46) != 0) {
            cnWrap_BgmFadeOut(0xF);
        }
        if (net_char_change != 0) {
            fade_set(0xA);
            return;
        }
        F(u8, (u8 *)cw, 0x2C34) = 4U;
        fade_set(0xA);
        return;
    case 1:
        F(s8, pNet, 0x11) = 1;
        if ((Fade_busy_ck(&jtbl_3219) & 0xFF) != 1) {
            temp_v1_3 = (int)cw;
            F(u8, temp_v1_3, 0x2C34) = (u8) (F(u8, temp_v1_3, 0x2C34) + 1);
            if (F(u8, (u8 *)cw, 0x2C46) != 0) {
                str_stop(0);
                str_stop(1);
            }
            SoftKeyboard_exit();
            return;
        }
    default:
        return;
    case 2:
        F(s8, pNet, 0x11) = 1;
        temp_a0 = (int)cw;
        F(u8, temp_a0, 0x2C34) = (u8) (F(u8, temp_a0, 0x2C34) + 1);
        F(s8, (u8 *)cw, 0x2C08) = 0;
        return;
    case 3:
        F(s8, pNet, 0x11) = 1;
        F(s8, (u8 *)cw, 0x2C08) = 1;
        temp_a0_2 = (int)cw;
        F(u8, temp_a0_2, 0x2C34) = (u8) (F(u8, temp_a0_2, 0x2C34) + 1);
        return;
    case 4:
        F(s8, pNet, 0x11) = 1;
        temp_a0_3 = Fade_busy_ck(&jtbl_3219) & 0xFF;
        if (temp_a0_3 != 1) {
            temp_v1_4 = (int)cw;
            F(u8, temp_v1_4, 0x2C34) = (u8) (F(u8, temp_v1_4, 0x2C34) + 1);
            tk_logout_init();
            fade_set(2);
            SetDialogData(7, 5);
            SoftKeyboard_exit();
            return;
        }
        break;
    case 5:
        F(s8, pNet, 0xC) = 1;
        temp_a0_4 = (int)cw;
        F(u8, temp_a0_4, 0x2C34) = (u8) (F(u8, temp_a0_4, 0x2C34) + 1);
        return;
    case 6:
        F(s8, pNet, 0xC) = 1;
        F(u8, (u8 *)cw, 0x2C34) = 7U;
        /* fallthrough */
    case 7:
        F(s8, pNet, 0xC) = 1;
        if (tk_logout(F(u8, (u8 *)cw, 0x2C46), temp_a1) != 0) {
            Set_ErrorDialog((s8) F(u8, (u8 *)cw, 0x2C46));
            temp_a0_5 = (int)cw;
            if (F(u8, temp_a0_5, 0x2C46) == 7) {
                F(u8, temp_a0_5, 0x2C34) = 9U;
                F(s32, (u8 *)cw, 0x2C4C) = 0x5A;
            } else {
                F(u8, temp_a0_5, 0x2C34) = (u8) (F(u8, temp_a0_5, 0x2C34) + 1);
            }
            CpInetTcpAbort(*(s32 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
            return;
        }
        break;
    case 8:
        F(s8, pNet, 0xC) = 1;
        if (temp_a1 & 0xFFFF & 0x20) {
            F(u8, (u8 *)cw, 0x2C34) = 0xAU;
            cnWrap_SoundRequest(0, 1);
            fade_set(1);
            return;
        }
        break;
    case 9:
        F(s8, pNet, 0xC) = 1;
        temp_a1_2 = (int)cw;
        temp_v1_5 = F(s32, temp_a1_2, 0x2C4C) - 1;
        F(s32, temp_a1_2, 0x2C4C) = temp_v1_5;
        if (temp_v1_5 <= 0) {
            temp_v1_6 = (int)cw;
            F(u8, temp_v1_6, 0x2C34) = (u8) (F(u8, temp_v1_6, 0x2C34) + 1);
            fade_set(1);
            return;
        }
        break;
    case 10:
        temp_a0_6 = Fade_busy_ck(&jtbl_3219) & 0xFF;
        if (temp_a0_6 != 1) {
            all_reset();
            F(s8, pNet, 0x11) = 1;
            *(s8 *)0x3F33F1 = 4;
            COM_R_No_0 = 1;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            MMBB_LOGIN = 0;
        }
        break;
    }
}
