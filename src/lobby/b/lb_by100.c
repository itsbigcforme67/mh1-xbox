/* lb_by100 - agent B promoted near-match 0x005BEA60-0x005BEDD4: lbc_logout_01 (first drafted by tools/lbauto.py). */
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
extern char D_4E36F4[];
typedef struct { u8 pad0[0x2C08]; s8 x2C08; u8 pad2C09[0x29]; u8 x2C32; u8 x2C33; u8 x2C34; u8 pad2C35[0x11]; u8 x2C46; u8 pad2C47[5]; s32 x2C4C; } CWS_lo1;
#define CWX ((CWS_lo1 *)cw)

void lbc_logout_01(void) {
    s32 sw;

    sw = Get_sw2(0) & 0xFFFF;
    switch (CWX->x2C34) {
    case 0:
        Lbc_init_network_work();
        F(s8, pNet, 0x11) = 1;
        CWX->x2C34++;
        Lbc_set_prim(&text_lobby_trans_ot0, 0, 0);
        if (CWX->x2C46 != 0) {
            cnWrap_BgmFadeOut(0xF);
        }
        if (net_char_change != 0) {
            fade_set(0xA);
            break;
        }
        CWX->x2C34 = 4;
        fade_set(0xA);
        break;
    case 1:
        F(s8, pNet, 0x11) = 1;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            CWX->x2C34++;
            if (CWX->x2C46 != 0) {
                str_stop(0);
                str_stop(1);
            }
            SoftKeyboard_exit();
        }
        break;
    case 2:
        F(s8, pNet, 0x11) = 1;
        CWX->x2C34++;
        CWX->x2C08 = 0;
        break;
    case 3:
        F(s8, pNet, 0x11) = 1;
        CWX->x2C08 = 1;
        CWX->x2C34++;
        break;
    case 4:
        F(s8, pNet, 0x11) = 1;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            CWX->x2C34++;
            tk_logout_init();
            fade_set(2);
            SetDialogData(7, 5);
            SoftKeyboard_exit();
        }
        break;
    case 5:
        F(s8, pNet, 0xC) = 1;
        CWX->x2C34++;
        break;
    case 6:
        F(s8, pNet, 0xC) = 1;
        CWX->x2C34 = 7;
    case 7:
        F(s8, pNet, 0xC) = 1;
        if (tk_logout(CWX->x2C46, sw) != 0) {
            Set_ErrorDialog((s8)CWX->x2C46);
            if (CWX->x2C46 == 7) {
                CWX->x2C34 = 9;
                CWX->x2C4C = 0x5A;
            } else {
                CWX->x2C34++;
            }
            CpInetTcpAbort(*(s32 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
        }
        break;
    case 8:
        F(s8, pNet, 0xC) = 1;
        if ((u16)sw & 0x20) {
            CWX->x2C34 = 0xA;
            cnWrap_SoundRequest(0);
            fade_set(1);
        }
        break;
    case 9:
        F(s8, pNet, 0xC) = 1;
        if (--CWX->x2C4C <= 0) {
            CWX->x2C34++;
            fade_set(1);
        }
        break;
    case 10:
        if ((Fade_busy_ck() & 0xFF) != 1) {
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
