/* G online (PC): internet_lobby_act, written from Monster Hunter G's code at lobby 0x579C70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). HAND-FIXED: the call through the mode table. */
#include "../gonl.h"
s32 lobby_func_0057A370();
s32 lobby_func_0057D2E0();
s32 lobby_func_0057E1E0();
s32 lobby_client_game_in_lobby();
s32 lobby_client_game_ready();
s32 lbc_logout_00();
s32 lobby_client_matching_failed();
s32 lobby_client_goto_top();
s32 lobby_return_to_lobby();
static s32 (*const network_lobby_client_main_jmp_207_g[])() = { lobby_func_0057A370, lobby_func_0057D2E0, lobby_func_0057E1E0, lobby_client_game_in_lobby, lobby_client_game_ready, lbc_logout_00, lobby_client_matching_failed, lobby_client_goto_top, lobby_return_to_lobby };   /* G's table, by name (HAND-FIXED) */

extern u8 BsLbsErrNum;
extern u8 ConnWork;
extern u8 Psw;
extern u8 cw;
extern u8 lb_prim;
extern u8 lb_sys;
extern u8 lpSKey;
extern u8 netr_ret;
extern u8 pNet;
extern u8 system_w;
s32 CpInetGetStatus();                              /* extern */
s32 Get_sw();                                /* extern */
s32 To_LogOut();                         /* extern */
s32 add_prim2(); /* extern */
s32 cnLBS_RecvData();                            /* extern */
s32 cnLbc_SetIspRestTime();               /* extern */
s32 cnWrap_IsAccessMemoryCard();              /* extern */
s32 font_set_stack_no();                 /* extern */
s32 lbc_text_lobby_trans();                     /* extern */
s32 lobby_client_admin_message();                   /* extern */
extern u8 g_main_2EF31C;
extern u8 network_lobby_client_main_jmp_207;

s32 internet_lobby_act(void) {
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s32 var_a1;
    s32 var_a2;
    s32 var_t0;
    s8 temp_v1_4;
    s8 temp_v1_6;
    u8 *var_a3;   /* (u16 *) */
    u16 temp_v0_3;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_5;

    temp_s0 = Get_sw(0) & 0xFFFF;
    if (((*(s8 *)((u8 *)&system_w + 0x3C)) == 0) && (cnLBS_RecvData((*(s32 *)((u8 *)&ConnWork + 0x4))) == 1)) {
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA80)) = 0xE10;
    }
    temp_a0 = (*(void **)&cw);
    (*(s32 *)((u8 *)(temp_a0) + 0xFA80)) = (s32) ((*(s32 *)((u8 *)(temp_a0) + 0xFA80)) - 1);
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB99)) != 5) {
        if (cnWrap_IsAccessMemoryCard(temp_a0) == 0) {
            temp_v1 = (*(void **)&cw);
            if ((*(s8 *)((u8 *)(temp_v1) + 0xB70)) != 0) {
                if ((temp_s0 == 0) && ((*(u8 *)((u8 *)((*(void **)&lpSKey)) + 0x664)) == 0)) {
                    if ((*(u16 *)((u8 *)&Psw + 0x8)) & 0x3C3C) {
                        goto block_10;
                    }
                } else {
block_10:
                    cnLbc_SetIspRestTime(temp_v1 + 0xFA84);
                }
                temp_v1_2 = (*(void **)&cw);
                temp_v0 = (*(s32 *)((u8 *)(temp_v1_2) + 0xFA84)) - 1;
                (*(s32 *)((u8 *)(temp_v1_2) + 0xFA84)) = temp_v0;
                if (temp_v0 < 0) {
                    To_LogOut(3);
                    return 1;
                }
                if (CpInetGetStatus() == 0) {
                    if ((*(u8 *)&BsLbsErrNum) != 0) {
                        goto block_17;
                    }
                    temp_v1_3 = (*(void **)&cw);
                    if ((*(s32 *)((u8 *)(temp_v1_3) + 0xFA80)) < 0) {
                        To_LogOut(6);
                        return 1;
                    }
                    temp_v1_4 = (*(s8 *)((u8 *)(temp_v1_3) + 0xB76));
                    if (temp_v1_4 == 1) {
                        To_LogOut(5);
                        return 1;
                    }
                    if (temp_v1_4 == 2) {
                        To_LogOut(7);
                        return 1;
                    }
                    goto block_43;
                }
block_17:
                To_LogOut(6);
                return 1;
            }
        }
        temp_a0_2 = (*(void **)&cw);
        if (((*(u8 *)((u8 *)(temp_a0_2) + 0xBA8)) == 1) && ((*(u8 *)&BsLbsErrNum) == 0)) {
            if ((temp_s0 != 0) || ((*(u8 *)((u8 *)((*(void **)&lpSKey)) + 0x664)) != 0)) {
                cnLbc_SetIspRestTime(temp_a0_2 + 0xFA84);
            }
            temp_v1_5 = (*(void **)&cw);
            if ((*(s32 *)((u8 *)(temp_v1_5) + 0xFA80)) < 0) {
                goto block_42;
            }
            temp_v0_2 = (*(s32 *)((u8 *)(temp_v1_5) + 0xFA84)) - 1;
            (*(s32 *)((u8 *)(temp_v1_5) + 0xFA84)) = temp_v0_2;
            if (temp_v0_2 < 0) {
                goto block_42;
            }
            temp_v1_6 = (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB76));
            if (temp_v1_6 != 1) {
                if (temp_v1_6 == 2) {
                    goto block_38;
                }
                if (CpInetGetStatus() != 0) {
                    goto block_42;
                }
            } else {
block_38:
block_42:
                (*(u8 *)&BsLbsErrNum) = 1U;
            }
        }
        goto block_43;
    }
block_43:
    if (lobby_client_admin_message() != 0) {
        if (((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x31)) == 0) && ((*(s32 *)((u8 *)&lb_prim + 0x74)) != 0)) {
            add_prim2(&g_main_2EF31C, &(*(u8 *)((u8 *)&lb_prim + 0x60)), 0, 1);
        }
        return (*(s32 *)&netr_ret);
    }
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0;
    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 0U;
    font_set_stack_no(0);
    var_a1 = 0;
    var_a3 = &(*(u8 *)((u8 *)&lb_sys + 0xA0));
    var_a2 = 0;
    do {
        temp_v0_3 = (*(u16 *)var_a3);
        if (temp_v0_3 != 0) {
            (*(u16 *)var_a3) = temp_v0_3 - 1;
        } else {
            var_t0 = 0;
            do {
                temp_v0_4 = var_t0 + var_a2;
                if (temp_v0_4 < 0x65) {
                    (*(s8 *)((u8 *)(((*(void **)&cw) + (temp_v0_4 * 0x24A))) + 0x1062)) = 0;
                }
                var_t0 += 1;
            } while (var_t0 < 7);
        }
        var_a1 += 1;
        var_a3 += 2;
        var_a2 += 7;
    } while (var_a1 < 0xC);
    network_lobby_client_main_jmp_207_g[(*(u8 **)&cw)[0xB99]]();   /* HAND-FIXED: a jump through G's table */
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x31)) == 0) {
        lbc_text_lobby_trans();
    }
    return (*(s32 *)&netr_ret);
}

