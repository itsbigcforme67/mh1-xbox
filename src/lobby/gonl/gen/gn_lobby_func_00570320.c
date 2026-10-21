/* G online (PC): lobby_func_00570320, written from Monster Hunter G's code at lobby 0x570320 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw2();
s32 Lb_cursorUD();
s32 Lb_eat_to_bell();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 SetHelpLineMsg();
s32 cnWrap_SoundRequest();
s64 lobby_func_00569320();
s64 lobby_func_00569390();
s32 lobby_func_0056D730();
s32 lobby_func_0056D770();
s32 lobby_func_0056DA20();
s32 lobby_func_0056FDE0();
s32 lobby_func_00570000();
s32 lobby_func_00570260();
s32 lobby_func_005802F0();
s32 lobby_func_005804E0();
s32 lobby_func_0059F6C0();
s32 tl_exit_sub_menu();
extern u8 g_main_2EFBC0;
extern u8 LobbyInfo;
extern u8 cw;
extern u8 g_main_2EFBC0;
extern u8 jtbl_0063E100;
extern u8 pNet;
extern u8 tl_member_buff;
extern u8 ClassInfo;
extern void *g_lb_636E1C;
extern void *g_lb_636E20;
extern s8 g_lb_6B46A5;
extern s8 g_lb_6B46A6;

void lobby_func_00570320(void) {
    u8 *var_a0;   /* (s32 *) */
    s16 temp_a4;
    s16 var_a1;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_a1_4;
    s32 temp_a1_5;
    s32 temp_a3;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s8 var_a2;
    u16 temp_a2;
    u8 temp_a0_2;
    u8 temp_s1;
    u8 temp_v1;
    u8 var_v0;
    u8 *temp_a0_3;
    u8 *temp_a1_3;
    u8 *temp_a1_6;
    u8 *temp_a1_7;

    temp_v0 = Get_sw2(0);
    var_a2 = (*(s8 *)((u8 *)&tl_member_buff + 0x138A));
    var_a1 = (*(s16 *)((u8 *)&tl_member_buff + 0x137C));
    (*(s16 *)((u8 *)&tl_member_buff + 0x1380)) = var_a2 + (var_a1 * 7);
    temp_a4 = temp_v0 & 0xFFFF;
    if ((u8) (*(u8 *)((u8 *)&tl_member_buff + 0x1382)) < 9U) {
        var_a0 = &jtbl_0063E100;
        switch ((*(u8 *)((u8 *)&tl_member_buff + 0x1382))) {                       /* switch 1 */
        case 0:                                     /* switch 1 */
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 1U;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)) = 0;
            SetDialogData(0x49, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
            SetDialogYesNo(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) = 0U;
            return;
        case 1:                                     /* switch 1 */
            temp_a1 = temp_a4 & 0xFFFF;
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            if (temp_a1 & 0x20) {
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 1) {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 2U;
                    lobby_func_0056DA20(1, temp_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                    lobby_func_0059F6C0(g_lb_636E20, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                } else {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 3U;
                    g_lb_6B46A5 = 2;
                    g_lb_6B46A6 = 0;
                    SetDialogData(0x4A, temp_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                    lobby_func_0059F6C0(g_lb_636E1C, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                }
                cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            if (temp_a1 & 0x40) {
                tl_exit_sub_menu(0, temp_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            temp_s1 = (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F));
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) = Lb_cursorUD((s32) temp_s1, 2, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
            SetDialogYesNo((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            if ((((s8) ((s32) temp_s1))) != (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F))) {
                SetHelpLineMsg((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) + 0x25, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            break;
        case 2:                                     /* switch 1 */
            temp_v0_2 = lobby_func_00570000((s32) &jtbl_0063E100, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
            switch (temp_v0_2) {                    /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) + 1);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                SetDialogData(0x4A, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            case 3:                                 /* switch 2 */
                tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            break;
        case 3:                                     /* switch 1 */
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            temp_v0_3 = lobby_func_005802F0((s32) &jtbl_0063E100, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
            switch (temp_v0_3) {                    /* switch 3; irregular */
            case 0:                                 /* switch 3 */
                if (*(*(u8 **)&g_main_2EFBC0) == 0) {
                    var_v0 = 9;
                    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) == 0) {

                    } else {
                        var_v0 = 2;
                    }
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = var_v0;
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
                    SetDialogData(0x4B, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    return;
                }
                lobby_func_0056FDE0(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) + 1);
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)) = lobby_func_00569320((s32) *(*(u8 **)&g_main_2EFBC0), 7, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)) = 0;
                SetHelpLineMsg(0x17, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            case 1:                                 /* switch 3 */
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 2U;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
                Lb_eat_to_bell((*(void **)&cw) + 0xF75D, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            break;
        case 4:                                     /* switch 1 */
            temp_a0 = temp_a4 & 0xFFFF;
            (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = temp_a4;
            if (temp_a0 & 0x20) {
                if (*(*(u8 **)&g_main_2EFBC0) != 0) {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 5U;
                    SetDialogData(0x69, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                    cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    return;
                }
                cnWrap_SoundRequest(7, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            if (temp_a0 & 0x40) {
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) != 0) {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 2U;
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) = 0;
                    cnWrap_SoundRequest(3, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                    SetHelpLineMsg(0x26, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    return;
                }
                tl_exit_sub_menu(0, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            lobby_func_00569390(&(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + 0x1372, &(*(u8 *)((u8 *)&tl_member_buff + 0x18)) + 0x1364, 7, (s32) *(*(u8 **)&g_main_2EFBC0), (s32) temp_a4);
            return;
        case 5:                                     /* switch 1 */
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            temp_v0_4 = lobby_func_005804E0((s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)), (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
            switch (temp_v0_4) {                    /* switch 4; irregular */
            case 0:                                 /* switch 4 */
                if (lobby_func_00570260(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) == 1) {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) + 1);
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)) = 0;
                    return;
                }
                SetDialogData(0x4D, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 4U;
                return;
            case 1:                                 /* switch 4 */
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 4U;
                Lb_eat_to_bell((*(void **)&cw) + 0xF75D, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            break;
        case 6:                                     /* switch 1 */
            temp_a3 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x10;
            temp_a2 = (*(u16 *)((u8 *)((temp_a3 + (*(u8 **)&g_main_2EFBC0))) + 8));
            temp_a1_2 = temp_a2 * 0x15C;
            temp_a0_2 = *(&(*(u8 *)((u8 *)&LobbyInfo + 0x16)) + temp_a1_2);
            if ((temp_a0_2 != 3) && (temp_a0_2 != 4)) {
                SetDialogData(0x4C, temp_a1_2, (s32) temp_a2, temp_a3);
                temp_a1_3 = (*(void **)&pNet);
                (*(s8 *)((u8 *)(temp_a1_3) + 0x2D)) = 1;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 4U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                cnWrap_SoundRequest(3, (s32) temp_a1_3, 1, M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            temp_a0_3 = (*(void **)&pNet);
            temp_a1_4 = temp_a4 & 0xFFFF;
            (*(s16 *)((u8 *)(temp_a0_3) + 0x20)) = temp_a4;
            if (temp_a1_4 & 0x40) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 4U;
                cnWrap_SoundRequest(3, temp_a1_4, (s32) temp_a2, temp_a3);
                return;
            }
            if (temp_a1_4 & 0x100) {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 7U;
                lobby_func_0056D730((s32) temp_a0_3, temp_a1_4, (s32) temp_a2, temp_a3);
                (*(s16 *)((u8 *)((*(void **)&cw)) + 0xF544)) = (s16) ClassInfo;
                temp_a1_5 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x10;
                (*(s16 *)((u8 *)((*(void **)&cw)) + 0xF546)) = (s16) ((*(u16 *)((u8 *)((temp_a1_5 + (*(u8 **)&g_main_2EFBC0))) + 8)) + 1);
                cnWrap_SoundRequest(6, temp_a1_5, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            if (temp_a1_4 & 0x200) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)) ^ 1);
                cnWrap_SoundRequest(6, temp_a1_4, (s32) temp_a2, temp_a3);
                return;
            }
            break;
        case 7:                                     /* switch 1 */
            if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x25)) == 0) {
                var_a2 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1368)) * 0x10;
                var_a1 = (s16) (*(u16 *)((u8 *)((var_a2 + (*(u8 **)&g_main_2EFBC0))) + 8));
                var_a0 = (s32 *) (var_a1 * 0x15C);
                temp_v1 = *((u8 *)&LobbyInfo + 0x16 + (s32) var_a0);
                if (temp_v1 != 3) {
                    var_a0 = (s32 *)0x4C;
                    if (temp_v1 != 4) {
                        SetDialogData(0x4C, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                        temp_a1_6 = (*(void **)&pNet);
                        (*(s8 *)((u8 *)(temp_a1_6) + 0x2D)) = 1;
                        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 4U;
                        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                        cnWrap_SoundRequest(3, (s32) temp_a1_6, 1, M2C_ERROR(/* Read from unset register $a3 */));
                        return;
                    }
                }
            }
            temp_v0_5 = lobby_func_0056D770((s32) var_a0, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
            switch (temp_v0_5) {                    /* switch 5; irregular */
            case 3:                                 /* switch 5 */
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 6U;
                return;
            case 1:                                 /* switch 5 */
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = 8U;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 6U;
                return;
            }
            break;
        case 8:                                     /* switch 1 */
            temp_a1_7 = (*(void **)&pNet);
            (*(s8 *)((u8 *)(temp_a1_7) + 0x2D)) = 1;
            if (temp_a4 & 0xFFFF & 0x20) {
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) == 9) {
                    tl_exit_sub_menu(1, (s32) temp_a1_7, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                    cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    return;
                }
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)) = (u8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E));
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136E)) = 0U;
                cnWrap_SoundRequest(6, (s32) temp_a1_7, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            break;
        }
    } else {
        tl_exit_sub_menu(0, (s32) var_a1, (s32) var_a2, M2C_ERROR(/* Read from unset register $a3 */));
    }
}

