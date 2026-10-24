/* G online (PC): lbc_logout_01, written from Monster Hunter G's code at lobby 0x584790 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CpInetTcpAbort();
s32 CpInetTcpDelete();
#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 SetDialogData();
s32 cnWrap_SoundRequest();
s32 fade_set();
s32 str_stop();
s32 text_lobby_trans_ot0();
s32 tk_logout();
extern u8 COM_R_No_0;
extern u8 COM_R_No_1;
extern u8 COM_R_No_2;
extern u8 COM_R_No_3;
extern u8 COM_R_No_4;
extern u8 CnetWork;
extern u8 ConnWork;
extern u8 MMBB_LOGIN;
extern u8 cw;
extern u8 game_w;
extern u8 jtbl_3219;
extern u8 net_char_change;
extern u8 pNet;
s32 Fade_busy_ck();                   /* extern */
s32 Set_ErrorDialog();                        /* extern */
s32 SoftKeyboard_exit();                        /* extern */
s32 all_reset();                             /* extern */
s32 cnWrap_BgmFadeOut();                 /* extern */
s32 tk_logout_init();                        /* extern */

void lbc_logout_01(void) {
    s32 temp_a0_3;
    s32 temp_a0_6;
    s32 temp_a1;
    s32 temp_v1_5;
    u8 temp_a1_3;
    u8 temp_v1;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a0_4;
    u8 *temp_a0_5;
    u8 *temp_a1_2;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;
    u8 *temp_v1_6;

    temp_v1 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C));
    temp_a1 = Get_sw2(0) & 0xFFFF;
    switch (temp_v1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        Lbc_init_network_work((s32) &jtbl_3219, temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
        temp_v1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) + 1);
        Lbc_set_prim((s32) text_lobby_trans_ot0, 0, 0, 0);
        if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0)) != 0) {
            cnWrap_BgmFadeOut(0xF);
        }
        if ((*(u8 *)&net_char_change) != 0) {
            fade_set(0xA);
            return;
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 4U;
        fade_set(0xA);
        return;
    case 1:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
        if ((Fade_busy_ck(&jtbl_3219, temp_a1) & 0xFF) != 1) {
            temp_v1_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) + 1);
            if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0)) != 0) {
                str_stop(0);
                str_stop(1);
            }
            SoftKeyboard_exit();
            return;
        }
    default:                                        /* switch 1 */
        return;
    case 2:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
        temp_a0 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
        return;
    case 3:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
        temp_a0_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) + 1);
        return;
    case 4:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
        temp_a0_3 = Fade_busy_ck(&jtbl_3219, temp_a1) & 0xFF;
        if (temp_a0_3 != 1) {
            temp_v1_4 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_4) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0xB9C)) + 1);
            tk_logout_init(temp_a0_3);
            fade_set(2);
            SetDialogData(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            SoftKeyboard_exit();
            return;
        }
        break;
    case 5:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_a0_4 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_4) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_4) + 0xB9C)) + 1);
        return;
    case 6:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 7U;
        /* fallthrough */
    case 7:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (tk_logout((s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0)), temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) != 0) {
            Set_ErrorDialog((s8) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0)));
            temp_a0_5 = (*(void **)&cw);
            if ((*(u8 *)((u8 *)(temp_a0_5) + 0xBB0)) == 7) {
                (*(u8 *)((u8 *)(temp_a0_5) + 0xB9C)) = 9U;
                (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x5A;
            } else {
                (*(u8 *)((u8 *)(temp_a0_5) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_5) + 0xB9C)) + 1);
            }
            CpInetTcpAbort((*(s32 *)((u8 *)&ConnWork + 0x4)));
            CpInetTcpDelete(&(*(s32 *)((u8 *)&ConnWork + 0x4)));
            return;
        }
        break;
    case 8:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_a1 & 0xFFFF & 0x20) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0xAU;
            cnWrap_SoundRequest(0, 1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            fade_set(1);
            return;
        }
        break;
    case 9:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_a1_2 = (*(void **)&cw);
        temp_v1_5 = (*(s32 *)((u8 *)(temp_a1_2) + 0xBB4)) - 1;
        (*(s32 *)((u8 *)(temp_a1_2) + 0xBB4)) = temp_v1_5;
        if (temp_v1_5 <= 0) {
            temp_v1_6 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_6) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_6) + 0xB9C)) + 1);
            fade_set(1);
            return;
        }
        break;
    case 10:                                        /* switch 1 */
        temp_a0_6 = Fade_busy_ck(&jtbl_3219, temp_a1) & 0xFF;
        if (temp_a0_6 != 1) {
            all_reset(temp_a0_6);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
            temp_a1_3 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0));
            switch (temp_a1_3) {                    /* switch 2; irregular */
            case 3:                                 /* switch 2 */
                (*(s8 *)((u8 *)&game_w + 0x1)) = 4;
                (*(s8 *)&COM_R_No_0) = 1;
                (*(s8 *)&COM_R_No_1) = 0;
                (*(s8 *)&COM_R_No_2) = 0;
                (*(s8 *)&COM_R_No_3) = 0;
                (*(s8 *)&COM_R_No_4) = 0;
                (*(s8 *)&MMBB_LOGIN) = 0;
                return;
            case 7:                                 /* switch 2 */
            case 6:                                 /* switch 2 */
            case 5:                                 /* switch 2 */
            case 4:                                 /* switch 2 */
                (*(s8 *)&COM_R_No_0) = 0;
                (*(u8 *)((u8 *)&game_w + 0x1)) = 1;
                (*(s8 *)&COM_R_No_1) = 0;
                (*(s8 *)&MMBB_LOGIN) = 3;
                (*(s8 *)&COM_R_No_2) = 0;
                (*(s8 *)&COM_R_No_3) = 0;
                (*(s8 *)&COM_R_No_4) = 0;
                (*(s8 *)((u8 *)&CnetWork + 0x5)) = 0;
                break;
            }
        }
        break;
    }
}

