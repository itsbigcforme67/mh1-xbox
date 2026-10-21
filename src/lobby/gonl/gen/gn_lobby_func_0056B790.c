/* G online (PC): lobby_func_0056B790, written from Monster Hunter G's code at lobby 0x56B790 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_cursorUD();
s32 Lb_select();
s32 Plaza_log_id_chk();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 SetHelpLineMsg();
s32 cnWrap_SoundRequest();
s64 lobby_func_00569320();
s64 lobby_func_00569390();
s32 lobby_func_0056C350();
s32 lobby_func_0056C3F0();
s32 lobby_func_0059F6C0();
s32 lobby_func_0059F6E0();
s32 lobby_func_005C14D0();
s32 lobby_func_005C1510();
s32 lobby_func_005C1560();
s32 lobby_func_005C15F0();
s32 tl_exit_sub_menu();
extern u8 BsProxyUrlstr;
extern u8 cw;
extern u8 pNet;
extern u8 tl_member_buff;
s32 Get_sw2();                               /* extern */
s16 Get_sw_on2();                            /* extern */
s32 SaveNetFile_ForLobby();                         /* extern */
s32 func_00235C80();                            /* extern */
s32 memcpy();         /* extern */
s32 memset();            /* extern */
extern s32 g_lb_636590;
extern s32 g_lb_636594;
extern s32 g_lb_653950;
extern s8 g_lb_6B42BC;
extern s32 g_main_302290;

s32 lobby_func_0056B790(void) {
    u8 *var_s1;   /* (s32 *) */
    u8 *var_s0;   /* (s32 *) */
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 temp_v1_5;
    s32 var_s2;
    s8 temp_s0_2;

    var_s1 = &(*(u8 *)((u8 *)&tl_member_buff + 0x1398));
    temp_s0 = Get_sw2(0) & 0xFFFF;
    switch (g_lb_6B42BC) {                           /* switch 1 */
    case 0:                                         /* switch 1 */
        temp_a1 = temp_s0 & 0xFFFF;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_a1 & 0x20) {
            if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) == 0) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 1;
                memset(&g_lb_653950, 0, 0x100);
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) = Plaza_log_id_chk(&g_lb_653950);
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x606)) = lobby_func_00569320((s64) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)), 7);
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) = 0;
                var_s2 = 0;
                var_s0 = &g_lb_653950;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x610)) = 0;
loop_6:
                temp_v0 = (*(s32 *)var_s0);
                if (temp_v0 != 0) {
                    memcpy(var_s1, temp_v0 + 0x46, 8);
                    memcpy(var_s1 + 8, (*(s32 *)var_s0) + 0x4E, 0x10);
                    var_s0 += 4;
                    var_s2 = (var_s2 + 1) & 0xFF;
                    var_s1 += 0x18;
                    if (var_s2 >= 0x40) {

                    } else {
                        goto loop_6;
                    }
                }
                lobby_func_0059F6C0(g_lb_636590);
                SetHelpLineMsg(0x1C);
            } else {
                lobby_func_005C14D0(&g_main_302290, 8);
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) = (s16) M2C_ERROR(/* Read from unset register $v0 */);
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x613)) = 0;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 3;
                lobby_func_0059F6C0(g_lb_636594);
                SetHelpLineMsg(0x1D);
            }
            cnWrap_SoundRequest(0);
        } else if (temp_a1 & 0x40) {
            tl_exit_sub_menu(0);
        } else {
            temp_s0_2 = (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F));
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) = Lb_cursorUD((s32) temp_s0_2, 2);
            SetDialogYesNo((s32) (u8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)));
            if ((((s8) ((s64) temp_s0_2))) != (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F))) {
                SetHelpLineMsg((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) + 0x27);
            }
        }
block_99:
        return 2;
    case 1:                                         /* switch 1 */
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) = (s16) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x610)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 7));
        temp_v1 = temp_s0 & 0xFFFF;
        if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) {
            if (temp_v1 & 0x40) {
                return 3;
            }
            if (temp_v1 & 0x200) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 2;
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)) = 0U;
                lobby_func_0059F6E0(0xB0462000);
                cnWrap_SoundRequest(6);
            } else if (temp_v1 & 0x60) {
                cnWrap_SoundRequest(7);
            }
            goto block_99;
        }
        temp_v1_2 = temp_s0 & 0xFFFF;
        if (temp_v1_2 & 0x40) {
            return 3;
        }
        if (temp_v1_2 & 0x20) {
            temp_v0_2 = lobby_func_005C1560(&(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), 8, (s32) ((u8 *)&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x18)));
            if (temp_v0_2 == -1) {
                temp_v0_3 = lobby_func_005C1510(&(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), 8);
                if (temp_v0_3 == -1) {
                    SetDialogData(0x33);
                    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = (s8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C));
                    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 7;
                } else {
                    memcpy((u8 *)&(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)) + (temp_v0_3 * 0x18), (u8 *)&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x18), 0x18);
                }
            } else {
                lobby_func_005C15F0((s32) &(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), temp_v0_2 & 0xFF, 8);
            }
            cnWrap_SoundRequest(6);
        } else if (temp_v1_2 & 0x200) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 2;
            (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)) = 0U;
            lobby_func_0059F6E0(0xB0462000);
            cnWrap_SoundRequest(6);
        } else {
            lobby_func_00569390((u8 *)&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)) + 0x610, (u8 *)&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)) + 0x604, 7, (s64) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)), temp_s0);
        }
        goto block_99;
    case 2:                                         /* switch 1 */
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        lobby_func_005C14D0(&(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), 8);
        temp_v1_3 = temp_s0 & 0xFFFF;
        if (M2C_ERROR(/* Read from unset register $v0 */) == 0) {
            if (temp_v1_3 & 0x240) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 1;
                lobby_func_0059F6E0(0xB000240A);
                cnWrap_SoundRequest(3);
            } else if (temp_v1_3 & 0x20) {
                cnWrap_SoundRequest(7);
            }
        } else {
            temp_a0 = temp_s0 & 0xFFFF;
            if (temp_a0 & 0x240) {
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 1;
                lobby_func_0059F6E0(0xB000240A);
                cnWrap_SoundRequest(3);
            } else if (temp_a0 & 0x20) {
                lobby_func_005C15F0((s32) &(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), (s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)), 8);
                if ((s8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)) != 0) {
                    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)) = (u8) ((s8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)) - 1);
                }
                cnWrap_SoundRequest(6);
            } else {
                (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)) = Lb_cursorUD((s32) (s8) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x611)), M2C_ERROR(/* Read from unset register $v0 */));
            }
        }
        goto block_99;
    case 3:                                         /* switch 1 */
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        temp_v1_4 = temp_s0 & 0xFFFF;
        if ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) == 0) {
            if (temp_v1_4 & 0x40) {
                if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) == 1) {
                    return 3;
                }
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 5;
                (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) = lobby_func_0056C350();
                func_00235C80();
                goto block_99;
            }
            if (temp_v1_4 & 0x240) {
                cnWrap_SoundRequest(7);
            }
            goto block_99;
        }
        temp_v1_5 = temp_s0 & 0xFFFF;
        if (temp_v1_5 & 0x40) {
            if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) == 1) {
                return 3;
            }
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 5;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) = lobby_func_0056C350();
            cnWrap_SoundRequest(3);
            func_00235C80();
            goto block_99;
        }
        if (temp_v1_5 & 0x200) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 4;
            SetDialogData(0x1C);
            SetDialogYesNo(1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            cnWrap_SoundRequest(6);
        } else {
            (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) = Lb_cursorUD((s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)));
        }
        goto block_99;
    case 4:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_4 = Lb_select();
        switch (temp_v0_4) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 0U;
            memcpy((*(void **)&cw) + 0xEE7, (u8 *)&g_main_302290 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x18), 8);
            memcpy((*(void **)&cw) + 0xEEF, (u8 *)&g_main_302290 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x18) + 8, 0x10);
            lobby_func_005C15F0((s32) &g_main_302290, (s32) (u8) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)), 8);
            lobby_func_005C14D0(&g_main_302290, 8);
            (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) = (s16) M2C_ERROR(/* Read from unset register $v0 */);
            if (((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) != 0) && ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) != 0)) {
                (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) = (s16) ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) - 1);
            }
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 6;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E)) = 3;
            SetDialogData(0x1D);
            break;
        case 3:                                     /* switch 2 */
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0;
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = 3;
            break;
        }
        goto block_99;
    case 5:                                         /* switch 1 */
        if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) == 0) {
            temp_v0_5 = SaveNetFile_ForLobby();
            if ((temp_v0_5 != -1) && (temp_v0_5 != 1)) {
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                goto block_99;
            }
            lobby_func_0056C3F0();
            return 3;
        }
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) = (s8) ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x612)) - 1);
        goto block_99;
    case 6:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E));
            cnWrap_SoundRequest(6);
        }
        goto block_99;
    case 7:                                         /* switch 1 */
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C)) = (s8) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60E));
            cnWrap_SoundRequest(6);
        }
        goto block_99;
    default:                                        /* switch 1 */
        return 3;
    }
}

