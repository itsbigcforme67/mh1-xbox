/* G online (PC): lbc_logout_00, written from Monster Hunter G's code at lobby 0x5843F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CpInetTcpAbort();
s32 CpInetTcpDelete();
s32 Fade_busy_ck();
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 Pit_reset();
s32 SetDialogData();
s32 SoftKeyboard_exit();
s32 fade_set();
s32 str_stop();
s32 tk_logout();
extern u8 COM_R_No_0;
extern u8 COM_R_No_1;
extern u8 COM_R_No_2;
extern u8 COM_R_No_3;
extern u8 COM_R_No_4;
extern u8 ConnWork;
extern u8 MMBB_LOGIN;
extern u8 cw;
extern u8 game_w;
extern u8 jtbl_3160;
extern u8 pNet;
extern u8 player_work;
s32 cnWrap_BgmFadeOut();                 /* extern */
s32 cnWrap_BgmStop();                           /* extern */
s32 tk_logout_init();      /* extern */
extern u8 put_back;

void lbc_logout_00(s64 arg_sp0) {
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a1;
    u8 *temp_a0;
    u8 *temp_a1_2;
    u8 *temp_v0;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;
    u8 *temp_v1_5;

    temp_v0 = (*(void **)&cw);
    if ((s32) (*(u8 *)((u8 *)(temp_v0) + 0xB9B)) >= 8) {
        (*(u8 *)((u8 *)(temp_v0) + 0xB9B)) = 0U;
    }
    switch ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9B))) { /* switch 1 */
    case 0:                                         /* switch 1 */
    case 1:                                         /* switch 1 */
    case 2:                                         /* switch 1 */
        break;
    }
    temp_v1 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_a1) {                              /* switch 2 */
    case 0:                                         /* switch 2 */
        Lbc_init_network_work();
        temp_a0 = (*(void **)&cw);
        if ((*(u8 *)((u8 *)(temp_a0) + 0xBB0)) != 0) {
            (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9C)) + 1);
            Lbc_init_network_work();
            Lbc_set_prim((s32) &put_back, 0, 0, 0);
            cnWrap_BgmFadeOut(0xF);
            fade_set(0xA);
            return;
        }
        (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = 3U;
        return;
    case 1:                                         /* switch 2 */
        if ((Fade_busy_ck() & 0xFF) != 1) {
            temp_v1_2 = (*(void **)&cw);
            if ((*(u8 *)((u8 *)(temp_v1_2) + 0xBB0)) != 0) {
                str_stop(0);
                str_stop(1);
                if ((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&player_work + 0x15)) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00))) != 0x34) {
                    fade_set(2);
                    cnWrap_BgmStop();
                    Pit_reset();
                    temp_v1_3 = (*(void **)&cw);
                    (*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) + 2);
                } else {
                    temp_v1_4 = (*(void **)&cw);
                    (*(u8 *)((u8 *)(temp_v1_4) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0xB9C)) + 2);
                }
            } else {
                (*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) + 2);
            }
            SoftKeyboard_exit();
            return;
        }
    default:                                        /* switch 2 */
        return;
    case 2:                                         /* switch 2 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_a1 + 1);
        return;
    case 3:                                         /* switch 2 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_a1 + 1);
        tk_logout_init(&jtbl_3160, temp_a1, temp_v1 + 0xB9C);
        SoftKeyboard_exit();
        temp_a0_2 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0));
        if (temp_a0_2 == 3) {
            SetDialogData(9);
            fade_set(2);
            return;
        }
        if (temp_a0_2 != 0) {
            fade_set(2);
            SetDialogData(7);
            return;
        }
        break;
    case 4:                                         /* switch 2 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_a1 + 1);
        /* fallthrough */
    case 5:                                         /* switch 2 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (tk_logout((s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0))) != 0) {
            temp_v1_5 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_5) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_5) + 0xB9C)) + 1);
            CpInetTcpAbort((*(s32 *)((u8 *)&ConnWork + 0x4)));
            CpInetTcpDelete(&(*(u8 *)((u8 *)&ConnWork + 0x4)));
            return;
        }
        break;
    case 6:                                         /* switch 2 */
        if ((Fade_busy_ck() & 0xFF) != 1) {
            temp_a1_2 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a1_2) + 0xB9C)) + 1);
            if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0)) != 0) {
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                fade_set(1);
                return;
            }
        }
        break;
    case 7:                                         /* switch 2 */
        if ((Fade_busy_ck() & 0xFF) == 1) {
            if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0)) != 0) {
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            }
            return;
        }
        temp_a0_3 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB0));
        switch (temp_a0_3) {                        /* switch 3; irregular */
        case 2:                                     /* switch 3 */
        case 0:                                     /* switch 3 */
            (*(s8 *)&COM_R_No_0) = 0;
            (*(s8 *)&COM_R_No_1) = 0;
            (*(s8 *)((u8 *)&game_w + 0x1)) = 1;
            (*(s8 *)&COM_R_No_2) = 0;
            (*(s8 *)&COM_R_No_3) = 0;
            (*(s8 *)&COM_R_No_4) = 0;
            return;
        case 1:                                     /* switch 3 */
            (*(s8 *)&COM_R_No_1) = 0;
            (*(s8 *)&COM_R_No_2) = 0;
            (*(u8 *)((u8 *)&game_w + 0x1)) = 4;
            (*(s8 *)&COM_R_No_0) = 4;
            (*(s8 *)&COM_R_No_3) = 0;
            (*(s8 *)&COM_R_No_4) = 0;
            (*(s8 *)&MMBB_LOGIN) = 0;
            break;
        }
        break;
    }
}

