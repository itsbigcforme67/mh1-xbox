/* G online (PC): lobby_func_0056ED50, written from Monster Hunter G's code at lobby 0x56ED50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Eft25_set();
s32 Get_sw2();
s32 Get_sw_on2();
s32 Lb_cursorUD();
s32 Lb_eat_to_bell();
s32 Lbc_ConditionSearch();
s32 Plaza_add_friend();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 SetHelpLineMsg();
s32 cnWrap_SoundRequest();
s32 getUserInfo();
s64 lobby_func_00569320();
s64 lobby_func_00569390();
s32 lobby_func_0056D730();
s32 lobby_func_0056D770();
s32 lobby_func_0056DA20();
s32 lobby_func_0056DA50();
s32 lobby_func_0056DE60();
s32 lobby_func_0056E9E0();
s32 lobby_func_00571450();
s32 lobby_func_0057FEA0();
s32 lobby_func_005807E0();
s32 lobby_func_00580910();
s32 lobby_func_0059F6C0();
s32 memcmp();
void * memcpy();
void * memset();
s32 tl_exit_sub_menu();
extern u8 g_main_2EF390;
extern u8 g_main_2EFA74;
extern u8 g_main_2EFADC;
extern u8 g_main_2EFBB0;
extern u8 g_main_2EFBC4;
extern u8 cw;
extern u8 g_main_2EF390;
extern u8 g_main_2EFA74;
extern u8 g_main_2EFADC;
extern u8 g_main_2EFBB0;
extern u8 g_main_2EFBC4;
extern u8 jtbl_0063E0C0;
extern u8 pNet;
extern u8 tl_member_buff;
s32 Visual_main();                            /* extern */
s32 cnLBS_Get_SerchUserPlace(); /* extern */
extern s32 g_lb_636E10;
extern s8 g_lb_6B46D0;
extern s32 my_user_id;
extern s32 seekStr;

void lobby_func_0056ED50(void) {
    s16 temp_s0;
    s16 temp_s0_3;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_a1_5;
    s32 temp_v0;
    s32 temp_v0_10;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_7;
    s32 temp_v0_8;
    s32 temp_v0_9;
    s32 var_v0;
    s32 var_v0_2;
    s8 *temp_a1_3;
    u8 *temp_v1_3;
    u8 temp_s0_2;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 var_v1;
    u8 *temp_a0;
    u8 *temp_a1_4;
    u8 *temp_v0_6;
    u8 *temp_v1_4;

    temp_v0 = Get_sw2(0);
    (*(s16 *)((u8 *)&tl_member_buff + 0x1380)) = (*(s8 *)((u8 *)&tl_member_buff + 0x138A)) + ((*(s16 *)((u8 *)&tl_member_buff + 0x137C)) * 7);
    temp_s0 = temp_v0 & 0xFFFF;
    switch ((*(u8 *)((u8 *)&tl_member_buff + 0x1382))) {                           /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) + 1);
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        SetDialogData(0x48, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        SetDialogYesNo(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) = 0U;
        return;
    case 1:                                         /* switch 1 */
        temp_a1 = temp_s0 & 0xFFFF;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_a1 & 0x20) {
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) != 2) {
                SetHelpLineMsg(0x24, temp_a1, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                SetHelpLineMsg(0x23, temp_a1, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
            }
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 0) {
                if ((*(u16 *)&g_main_2EFA74) == 0) {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 7U;
                    g_lb_6B46D0 = 1;
                    SetDialogData(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                } else {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)) = 0;
                    (*(u8 **)&g_main_2EFBC4) = (u8 *) (*(u8 **)&g_main_2EF390);
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) = (u8) *(*(u8 **)&g_main_2EFBC4);
                    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)) = lobby_func_00569320((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)), 7, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)) = 0;
                    SetHelpLineMsg(0x16, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                }
            } else if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 1) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 6U;
                lobby_func_0056DA20(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 2U;
                lobby_func_0056DA20(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            }
            lobby_func_0059F6C0((*(s32 *)((u8 *)&g_lb_636E10 + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) * 4))), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371)) = (u8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F));
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        if (temp_a1 & 0x40) {
            tl_exit_sub_menu(0, temp_a1, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        temp_s0_2 = (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) = Lb_cursorUD((s32) temp_s0_2, 3, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) != (temp_s0_2 & 0xFF)) {
            SetHelpLineMsg((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) + 0x1F, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        SetDialogYesNo((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 2:                                         /* switch 1 */
        temp_v0_2 = lobby_func_0056DE60((s32) &jtbl_0063E0C0, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 3U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371)) = 2U;
            } else {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 5U;
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)) = 1;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371)) = 3U;
            }
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) = 0U;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) = 0U;
            (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            SetDialogData(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        case 3:                                     /* switch 2 */
            tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        break;
    case 3:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_3 = lobby_func_005807E0(&seekStr, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_3) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            if (*(*(u8 **)&g_main_2EFBB0) != 0) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 4U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) = (u8) *(*(u8 **)&g_main_2EFBB0);
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)) = lobby_func_00569320((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)), 7, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
                SetHelpLineMsg(0x16, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 0;
                return;
            }
            SetHelpLineMsg(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 2U;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
            SetDialogData(0x1F, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xEU;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
            return;
        case 1:                                     /* switch 3 */
            SetHelpLineMsg(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            Lb_eat_to_bell((*(void **)&cw) + 0xF75D, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xEU;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 2U;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
            return;
        }
        break;
    case 4:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_4 = lobby_func_0056E9E0((s32) &jtbl_0063E0C0, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        if ((temp_v0_4 != 1) && (temp_v0_4 != 0)) {
            return;
        }
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)) = 0;
        return;
    case 5:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_5 = lobby_func_00580910(&g_lb_6B46D0 + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x44) + 4, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_5) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
        case 3:                                     /* switch 4 */
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) + 1);
            memcpy(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x5C), &g_lb_6B46D0 + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x44) + 4, 9);
            temp_v0_6 = &(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x24A);
            cnLBS_Get_SerchUserPlace(temp_v0_6 + 0x154, temp_v0_6 + 0x156, temp_v0_6 + 0x158, temp_v0_6 + 0x35D);
            temp_v1 = (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) + 1;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) = temp_v1;
            if ((temp_v1 & 0xFF) >= 3) {
                SetHelpLineMsg(0x16, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)) = 0;
                return;
            }
            return;
        case 1:                                     /* switch 4 */
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) + 1);
            if (Visual_main(&g_lb_6B46D0 + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x44) + 4) < 6) {
                memset(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x5C), 0, 0x5C);
            } else {
                memcpy(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x5C), &g_lb_6B46D0 + ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x44) + 4, 9);
            }
            (*(s8 *)((u8 *)((((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) * 0x24A) + &(*(u8 *)((u8 *)&tl_member_buff + 0x18)))) + 0x35D)) = 2;
            temp_v1_2 = (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) + 1;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136C)) = temp_v1_2;
            if ((temp_v1_2 & 0xFF) >= 3) {
                SetHelpLineMsg(0x16, 2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)) = 0;
                return;
            }
            break;
        }
        break;
    case 6:                                         /* switch 1 */
        temp_v0_7 = lobby_func_0056DA50((s32) &jtbl_0063E0C0, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_7) {                        /* switch 5; irregular */
        case 0:                                     /* switch 5 */
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) + 1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            SetDialogData(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        case 3:                                     /* switch 5 */
            tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        break;
    case 7:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_8 = Lbc_ConditionSearch(&g_lb_6B46D0, (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 1, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_8) {                        /* switch 6; irregular */
        case 0:                                     /* switch 6 */
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) = (u8) *(*(u8 **)&g_main_2EFBC4);
            SetHelpLineMsg(0x16, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) != 0) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) + 1);
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 0) {
                    memcpy((*(u8 **)&g_main_2EF390), (*(u8 **)&g_main_2EFBC4), 0x23F4);
                    (*(u8 **)&g_main_2EFBC4) = (u8 *) (*(u8 **)&g_main_2EF390);
                    (*(u16 *)&g_main_2EFA74) = (u16) (*(u16 *)&g_main_2EFADC);
                }
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)) = lobby_func_00569320((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)), 7, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)) = 0;
                return;
            }
            SetHelpLineMsg(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 0) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 0xFU;
                SetDialogData(0x4B, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 6U;
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
                }
                SetDialogData(0x1F, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            }
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xEU;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
            return;
        case 1:                                     /* switch 6 */
            SetHelpLineMsg(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            Lb_eat_to_bell((*(void **)&cw) + 0xF75D, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xEU;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 6U;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
                return;
            }
            break;
        }
        break;
    case 8:                                         /* switch 1 */
        temp_a1_2 = temp_s0 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = temp_s0;
        if (temp_a1_2 & 0x20) {
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) != 0) {
                switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371))) { /* switch 7; irregular */
                case 3:                             /* switch 7 */
                    temp_a1_3 = &(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + (((s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) % 3) * 0x5C);
                    if (*temp_a1_3 != 0) {
                        if (memcmp(&my_user_id, temp_a1_3, 8) == 0) {
                            goto block_94;
                        }
block_102:
                        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 9U;
                        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF54A)) = 0;
                        memset((*(void **)&cw) + 0xAF8, 0, 0x62);
                        SetDialogData(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                        cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                        return;
                    }
block_94:
                    cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    return;
                case 2:                             /* switch 7 */
                    if (memcmp(&my_user_id, (*(u8 **)&g_main_2EFBB0) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x28) + 4, 8) == 0) {
                        cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                        return;
                    }
                    memset(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)), 0, 0x114);
                    memcpy(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + (((s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) % 3) * 0x5C), (*(u8 **)&g_main_2EFBB0) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x28) + 4, 8);
                    goto block_102;
                default:                            /* switch 7 */
                    if (memcmp(&my_user_id, (*(u8 **)&g_main_2EFBC4) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x5C) + 4, 8) == 0) {
                        cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                        return;
                    }
                    goto block_102;
                }
            } else {
                cnWrap_SoundRequest(7, temp_a1_2, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
        } else if (temp_a1_2 & 0x40) {
            switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371))) { /* switch 8; irregular */
            case 0:                                 /* switch 8 */
                tl_exit_sub_menu(0, temp_a1_2, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            case 1:                                 /* switch 8 */
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1370)) == 0) {
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
                }
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 6U;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
                SetHelpLineMsg(0x24, temp_a1_2, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
                cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            case 3:                                 /* switch 8 */
            case 2:                                 /* switch 8 */
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 2U;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
                SetHelpLineMsg(0x23, temp_a1_2, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
                cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
        } else if (temp_a1_2 & 0x200) {
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) != 2) {
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)) != 0) {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = (u8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A));
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xCU;
                    lobby_func_00571450((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)), temp_a1_2, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
                    (*(void **)((u8 *)(&(*(s32 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) = (void *) ((*(u8 **)&g_main_2EFBC4) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x5C) + 4);
                    cnWrap_SoundRequest(6, (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    return;
                }
                cnWrap_SoundRequest(7, temp_a1_2, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
        } else {
            temp_s0_3 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364));
            lobby_func_00569390(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + 0x1372, &(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + 0x1364, 7, (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136D)), (s32) temp_s0);
            if (((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371)) == 2) && ((((s16) ((s32) temp_s0_3))) != (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)))) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 4U;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
                SetDialogData(0x1E, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
        }
        break;
    case 9:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371)) != 2) && ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371)) != 3)) {
            var_v0 = getUserInfo((s32) ((*(u8 **)&g_main_2EFBC4) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x5C) + 4), (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        } else {
            var_v0 = getUserInfo((s32) (&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + (((s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) % 3) * 0x5C)), (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        }
        switch (var_v0) {                           /* switch 9; irregular */
        case 0:                                     /* switch 9 */
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 2) {
                var_v1 = (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) + 1;
            } else {
                var_v1 = 0xB;
            }
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = var_v1;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)) = 0;
            return;
        case 1:                                     /* switch 9 */
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xEU;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 8U;
            return;
        }
        break;
    case 10:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1371))) { /* switch 10; irregular */
        case 2:                                     /* switch 10 */
            var_v0_2 = lobby_func_0057FEA0((*(u8 **)&g_main_2EFBB0) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x28) + 4, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        default:                                    /* switch 10 */
        case 3:                                     /* switch 10 */
            var_v0_2 = lobby_func_0057FEA0(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + (((s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) % 3) * 0x5C), (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        }
        if ((var_v0_2 != 1) && (var_v0_2 != 0)) {
            return;
        }
        temp_v1_3 = (*(u8 **)&g_main_2EFBC4);
        temp_a1_4 = temp_v1_3 + 0xC;
        if (*temp_v1_3 != 0) {
            memcpy(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + (((s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) % 3) * 0x5C) + 8, temp_a1_4, 0x11);
            memcpy(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + (((s16) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) % 3) * 0x5C) + 0x1C, (*(u8 **)&g_main_2EFBC4) + 0x20, 0x40);
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) + 1);
            return;
        }
        Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), (s32) temp_a1_4, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xEU;
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 8U;
        return;
    case 11:                                        /* switch 1 */
        temp_a0 = (*(void **)&pNet);
        temp_a1_5 = temp_s0 & 0xFFFF;
        (*(s16 *)((u8 *)(temp_a0) + 0x20)) = Get_sw_on2(0);
        if (temp_a1_5 & 0x40) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
            cnWrap_SoundRequest(3, temp_a1_5, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        if (temp_a1_5 & 0x200) {
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = (u8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xCU;
            lobby_func_00571450((s32) temp_a0, temp_a1_5, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) != 2) {
                (*(void **)((u8 *)(&(*(s32 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) = (void *) ((*(u8 **)&g_main_2EFBC4) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x5C) + 4);
            } else {
                (*(void **)((u8 *)(&(*(s32 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) = (void *) ((*(u8 **)&g_main_2EFBC4) + 4);
            }
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        if (temp_a1_5 & 0x100) {
            temp_v1_4 = (*(void **)&cw);
            if (((*(u16 *)((u8 *)(temp_v1_4) + 0xF544)) != 0) && ((*(u8 *)((u8 *)(temp_v1_4) + 0xF54B)) == 3)) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xDU;
                lobby_func_0056D730((s32) temp_a0, temp_a1_5, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 8U;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xEU;
            if ((*(u16 *)((u8 *)((*(void **)&cw)) + 0xF546)) == 0) {
                SetDialogData(0x4F, temp_a1_5, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                SetDialogData(0x4E, temp_a1_5, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            }
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)) = Eft25_set((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)), 3, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 12:                                        /* switch 1 */
        temp_v0_9 = Plaza_add_friend((s32) &jtbl_0063E0C0, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        if ((temp_v0_9 != 1) && (temp_v0_9 != 0)) {
            return;
        }
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E));
        return;
    case 13:                                        /* switch 1 */
        temp_v0_10 = lobby_func_0056D770((s32) &jtbl_0063E0C0, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0_10) {                       /* switch 11; irregular */
        case 3:                                     /* switch 11 */
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xBU;
            return;
        case 1:                                     /* switch 11 */
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 0xEU;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 0xBU;
            return;
        }
        break;
    case 14:                                        /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) == 0xF) {
                tl_exit_sub_menu(0, 1, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
            } else {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E));
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 0U;
                if (((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) == 6) || ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) == 2)) {
                    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) != 2) {
                        SetHelpLineMsg(0x24, 1, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
                    } else {
                        SetHelpLineMsg(0x23, 1, (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
                    }
                }
            }
            cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        break;
    default:                                        /* switch 1 */
        tl_exit_sub_menu(0, (s32) (*(s16 *)((u8 *)&tl_member_buff + 0x137C)), (s32) (*(s8 *)((u8 *)&tl_member_buff + 0x138A)), M2C_ERROR(/* Read from unset register $a3 */));
        break;
    }
}

