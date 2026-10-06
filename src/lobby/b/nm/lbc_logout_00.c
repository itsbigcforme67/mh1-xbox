#include "lobby_a.h"
extern s8 COM_R_No_0;
extern s8 COM_R_No_1;
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;
extern s8 COM_R_No_4;
extern s8 MMBB_LOGIN;
extern char jtbl_3160[];
extern char put_back[];
extern char jtbl_3160[];
extern char D_3E4C05[];
extern char jtbl_3160[];
extern char D_4E36F4[];
extern char jtbl_3160[];
extern char jtbl_3160[];
typedef struct { u8 pad0[0x2C34]; u8 x2C34; u8 pad2C35[0x11]; u8 x2C46; } CWS_lo0;
#define CWX ((CWS_lo0 *)cw)
void lbc_logout_00(void) {
    switch (CWX->x2C34) {
    case 0:
        Lbc_init_network_work();
        if (CWX->x2C46 != 0) {
            CWX->x2C34++;
            Lbc_init_network_work();
            Lbc_set_prim(&put_back, 0, 0);
            cnWrap_BgmFadeOut(0xF);
            fade_set(0xA);
            break;
        }
        CWX->x2C34 = 3;
        break;
    case 1:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            if (CWX->x2C46 != 0) {
                str_stop(0);
                str_stop(1);
                if (*((u8 *)&D_3E4C05 + game_w.master * 0xA00) != 0x34) {
                    fade_set(2);
                    cnWrap_BgmStop();
                    Pit_reset();
                    CWX->x2C34 += 2;
                } else {
                    CWX->x2C34 += 2;
                }
            } else {
                CWX->x2C34 += 2;
            }
            SoftKeyboard_exit();
        }
        break;
    case 2:
        CWX->x2C34++;
        break;
    case 3:
        CWX->x2C34++;
        tk_logout_init();
        SoftKeyboard_exit();
        if (CWX->x2C46 == 3) {
            SetDialogData(8, 0);
            fade_set(2);
            break;
        }
        if (CWX->x2C46 != 0) {
            fade_set(2);
            SetDialogData(7, 5);
        }
        break;
    case 4:
        CWX->x2C34++;
    case 5:
        F(s8, pNet, 0xC) = 1;
        if (tk_logout(CWX->x2C46) != 0) {
            CWX->x2C34++;
            CpInetTcpAbort(*(s32 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
        }
        break;
    case 6:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            CWX->x2C34++;
            if (CWX->x2C46 != 0) {
                F(s8, pNet, 0xC) = 1;
                fade_set(1);
            }
        }
        break;
    case 7:
        if ((Fade_busy_ck() & 0xFF) == 1) {
            if (CWX->x2C46 != 0) {
                F(s8, pNet, 0xC) = 1;
            }
            return;
        }
        if (CWX->x2C46 == 0) {
            *(s8 *)0x3F33F1 = 1;
            COM_R_No_0 = 0;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
        } else if (CWX->x2C46 == 2) {
            *(u8 *)0x3F33F1 = 4;
            COM_R_No_0 = 1;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            MMBB_LOGIN = 0;
        } else if (CWX->x2C46 == 1) {
            *(u8 *)0x3F33F1 = 4;
            COM_R_No_1 = 0;
            COM_R_No_0 = 4;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            MMBB_LOGIN = 0;
        }
        break;
    }
}
