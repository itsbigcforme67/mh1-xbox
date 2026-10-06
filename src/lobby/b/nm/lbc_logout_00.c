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
void lbc_logout_00(void) {
    int temp_a0;
    int temp_a1_3;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;
    int temp_v1_4;
    int temp_v1_5;
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a1;
    u8 temp_a1_2;
    int temp_a2;

    temp_v1 = (int)cw;
    temp_a1 = F(u8, temp_v1, 0x2C34);
    temp_a2 = temp_v1 + 0x2C34;
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        Lbc_init_network_work();
        temp_a0 = (int)cw;
        if (F(u8, temp_a0, 0x2C46) != 0) {
            F(u8, temp_a0, 0x2C34) = (u8) (F(u8, temp_a0, 0x2C34) + 1);
            Lbc_init_network_work();
            Lbc_set_prim(&put_back, 0, 0);
            cnWrap_BgmFadeOut(0xF);
            fade_set(0xA);
            return;
        }
        F(u8, temp_a0, 0x2C34) = 3U;
        return;
    case 1:                                         /* switch 1 */
        if ((Fade_busy_ck(&jtbl_3160) & 0xFF) != 1) {
            temp_v1_2 = (int)cw;
            if (F(u8, temp_v1_2, 0x2C46) != 0) {
                str_stop(0);
                str_stop(1);
                temp_a1_2 = game_w.master;
                if ((*(s8 *)((u8 *)&D_3E4C05 + (temp_a1_2 * 0xA00))) != 0x34) {
                    fade_set(2);
                    cnWrap_BgmStop();
                    Pit_reset();
                    temp_v1_3 = (int)cw;
                    F(u8, temp_v1_3, 0x2C34) = (u8) (F(u8, temp_v1_3, 0x2C34) + 2);
                } else {
                    temp_v1_4 = (int)cw;
                    F(u8, temp_v1_4, 0x2C34) = (u8) (F(u8, temp_v1_4, 0x2C34) + 2);
                }
            } else {
                F(u8, temp_v1_2, 0x2C34) = (u8) (F(u8, temp_v1_2, 0x2C34) + 2);
            }
            SoftKeyboard_exit();
            return;
        }
    default:                                        /* switch 1 */
        return;
    case 2:                                         /* switch 1 */
        F(u8, temp_v1, 0x2C34) = (u8) (temp_a1 + 1);
        return;
    case 3:                                         /* switch 1 */
        F(u8, temp_v1, 0x2C34) = (u8) (temp_a1 + 1);
        tk_logout_init();
        SoftKeyboard_exit();
        temp_a0_2 = F(u8, (int)cw, 0x2C46);
        if (temp_a0_2 == 3) {
            SetDialogData(8, 0);
            fade_set(2);
            return;
        }
        if (temp_a0_2 != 0) {
            fade_set(2);
            SetDialogData(7, 5);
            return;
        }
        break;
    case 4:                                         /* switch 1 */
        F(u8, temp_v1, 0x2C34) = (u8) (temp_a1 + 1);
        /* fallthrough */
    case 5:                                         /* switch 1 */
        F(s8, pNet, 0xC) = 1;
        if (tk_logout(F(u8, (int)cw, 0x2C46), temp_a1, temp_a2) != 0) {
            temp_v1_5 = (int)cw;
            F(u8, temp_v1_5, 0x2C34) = (u8) (F(u8, temp_v1_5, 0x2C34) + 1);
            CpInetTcpAbort(*(s32 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
            return;
        }
        break;
    case 6:                                         /* switch 1 */
        if ((Fade_busy_ck(&jtbl_3160) & 0xFF) != 1) {
            temp_a1_3 = (int)cw;
            F(u8, temp_a1_3, 0x2C34) = (u8) (F(u8, temp_a1_3, 0x2C34) + 1);
            if (F(u8, (int)cw, 0x2C46) != 0) {
                F(s8, pNet, 0xC) = 1;
                fade_set(1, (u8) temp_a1_3);
                return;
            }
        }
        break;
    case 7:                                         /* switch 1 */
        if ((Fade_busy_ck(&jtbl_3160) & 0xFF) == 1) {
            if (F(u8, (int)cw, 0x2C46) != 0) {
                F(s8, pNet, 0xC) = 1;
            }
            return;
        }
        temp_a0_3 = F(u8, (int)cw, 0x2C46);
        switch (temp_a0_3) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            COM_R_No_0 = 0;
            *(s8 *)0x3F33F1 = 1;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            return;
        case 2:                                     /* switch 2 */
            COM_R_No_0 = 1;
            *(u8 *)0x3F33F1 = 4;
            COM_R_No_1 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
block_42:
            MMBB_LOGIN = 0;
            break;
        case 1:                                     /* switch 2 */
            COM_R_No_1 = 0;
            *(u8 *)0x3F33F1 = 4;
            COM_R_No_0 = 4;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            goto block_42;
        }
        break;
    }
}
