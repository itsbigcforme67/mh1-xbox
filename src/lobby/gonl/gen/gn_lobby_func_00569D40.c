/* G online (PC): lobby_func_00569D40, written from Monster Hunter G's code at lobby 0x569D40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EF390;
extern u8 ClassInfo;
extern u8 LobbyInfo;
extern u8 cw;
extern u8 g_main_2EF390;
extern u8 jtbl_0063DFE0;
extern u8 lb_quest_all;
extern u8 pNet;
s32 Get_sw2();                           /* extern */
s16 Get_sw_on2();             /* extern */
s32 Lb_eat_to_bell();                     /* extern */
s32 Lb_select();                    /* extern */
s32 Lbs_GetLobbyMemberList();     /* extern */
s32 SetDialogData();    /* extern */
s32 cnWrap_SoundRequest(); /* extern */
s32 lb_chatMemberCheck();         /* extern */
s32 lbc_in_plaza_03();            /* extern */
s16 lobby_func_00569320();          /* extern */
s32 lobby_func_00569390(); /* extern */
u8 lobby_func_00569C20();           /* extern */
s32 lobby_func_00569D00();    /* extern */
s32 lobby_func_0056A810();             /* extern */
s32 lobby_func_0056A850();  /* extern */
s32 lobby_func_0057FBB0();         /* extern */
s32 lobby_func_0057FC80();        /* extern */
s32 lobby_func_0057FD30();        /* extern */
s32 lobby_func_005802F0();        /* extern */
s32 lobby_func_00580FA0();        /* extern */
s32 lobby_func_00581210();        /* extern */
s32 memset();             /* extern */
extern s8 g_lb_6B46A0;
extern s8 g_lb_6B46A1;
extern s16 g_lb_6B46A2;
extern s8 g_lb_6B46A4;
extern s8 g_lb_6B46A5;
extern s8 g_lb_6B46A6;
extern u8 tl_member_buff;

s32 lobby_func_00569D40(void) {
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a1_6;
    s32 temp_a2_4;
    s32 temp_s0;
    s32 temp_v0_10;
    s32 temp_v0_11;
    s32 temp_v0_12;
    s32 temp_v0_13;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v0_8;
    s32 temp_v0_9;
    s32 temp_v1_5;
    s32 var_a2;
    s64 temp_s1;
    s64 var_a1;
    s8 *var_a0;
    u32 temp_a1_2;
    u32 temp_a1_4;
    u32 temp_a1_5;
    u32 temp_a1_7;
    u32 temp_a1_8;
    u32 temp_a3;
    u32 temp_a3_2;
    u8 temp_a0_3;
    u8 temp_a0_4;
    u8 temp_a1;
    u8 temp_a1_9;
    u8 temp_a2;
    u8 temp_v1;
    u8 *temp_a1_3;
    u8 *temp_a2_2;
    u8 *temp_a2_3;
    u8 *temp_v0;
    u8 *temp_v0_2;
    u8 *temp_v0_3;
    u8 *temp_v1_10;
    u8 *temp_v1_11;
    u8 *temp_v1_12;
    u8 *temp_v1_13;
    u8 *temp_v1_14;
    u8 *temp_v1_15;
    u8 *temp_v1_16;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;
    u8 *temp_v1_6;
    u8 *temp_v1_7;
    u8 *temp_v1_8;
    u8 *temp_v1_9;

    temp_v0 = (*(void **)&pNet);
    temp_a1 = (*(u8 *)((u8 *)(temp_v0) + 0x2B));
    temp_s1 = temp_a1 + ((*(s16 *)((u8 *)(temp_v0) + 0x1C)) * 7);
    temp_s0 = Get_sw2(0, temp_a1) & 0xFFFF;
    temp_v0_2 = (*(void **)&pNet);
    temp_a2 = (*(u8 *)((u8 *)(temp_v0_2) + 0x23));
    if (temp_a2 < 0x10U) {
        temp_a0 = *((temp_a2 * 4) + &jtbl_0063DFE0);
        switch (temp_a2) {                          /* switch 1 */
        case 0:                                     /* switch 1 */
            (*(u8 *)((u8 *)(temp_v0_2) + 0x23)) = (u8) (temp_a2 + 1);
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x32)) = 0U;
            (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1E)) = lobby_func_00569320((*(s16 *)((u8 *)&ClassInfo + 0x6)), 7, temp_a2);
            (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) = 0;
block_128:
            return 2;
        case 1:                                     /* switch 1 */
            temp_a1_2 = temp_s0 & 0xFFFF;
            (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0, &jtbl_0063DFE0, temp_a2);
            if (temp_a1_2 & 0x20) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = lobby_func_00569C20(temp_s1, temp_a1_2);
                goto block_128;
            }
            if (temp_a1_2 & 0x40) {
                return 3;
            }
            if (temp_a1_2 & 0x200) {
                temp_a0_2 = temp_s1 * 0x15C;
                temp_v1 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&LobbyInfo + 0x16)) + temp_a0_2));
                if ((temp_v1 != 3) && (temp_v1 != 4)) {
                    cnWrap_SoundRequest(7, temp_a1_2);
                } else {
                    temp_a1_3 = (*(void **)&pNet);
                    (*(u8 *)((u8 *)(temp_a1_3) + 0x32)) = 0U;
                    (*(u8 *)((u8 *)&ClassInfo + 0x4)) = temp_s1 + 1;
                    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = (s8) (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&LobbyInfo + 0x6)) + temp_a0_2));
                    cnWrap_SoundRequest(6, (u32) temp_a1_3);
                    memset(&(*(s8 *)((u8 *)&lb_quest_all + 0xA0)), 0, 0x398);
                    temp_v1_2 = (*(void **)&pNet);
                    (*(u8 *)((u8 *)(temp_v1_2) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0x23)) + 1);
                }
            } else {
                temp_v0_3 = (*(void **)&pNet);
                lobby_func_00569390(temp_v0_3 + 0x2B, temp_v0_3 + 0x1C, 7, (*(s16 *)((u8 *)&ClassInfo + 0x6)), temp_s0);
            }
            goto block_128;
        case 2:                                     /* switch 1 */
            if ((*(u8 *)((u8 *)(temp_v0_2) + 0x32)) & 1) {
                (*(u8 *)((u8 *)(temp_v0_2) + 0x23)) = (u8) (temp_a2 + 1);
            } else if (lobby_func_0057FD30(((s16) (temp_s1)), &jtbl_0063DFE0, temp_a2) == 1) {
                temp_v1_3 = (*(void **)&pNet);
                var_a1 = 0;
                var_a2 = 0;
                var_a0 = &(*(s8 *)((u8 *)&lb_quest_all + 0xA0));
                (*(u8 *)((u8 *)(temp_v1_3) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x23)) + 1);
                temp_v1_4 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_4) + 0x32)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0x32)) | 1);
                do {
                    if (*var_a0 != 0) {
                        var_a1 = (var_a1 + 1) & 0xFFFF;
                    }
                    var_a2 = (var_a2 + 1) & 0xFFFF;
                    var_a0 += 0x5C;
                } while (var_a2 < 0xA);
                (*(s16 *)((u8 *)(&tl_member_buff) + 6)) = 0;
                (*(s16 *)((u8 *)(&tl_member_buff) + 8)) = lobby_func_00569320((s16) (((s16) (var_a1))), 8, (u8) var_a2);
            }
            goto block_128;
        case 3:                                     /* switch 1 */
            temp_a2_2 = (*(void **)&pNet);
            temp_a1_4 = temp_s1 * 0x15C;
            (*(s16 *)((u8 *)(temp_a2_2) + 0x20)) = Get_sw_on2(0, &jtbl_0063DFE0, temp_a2);
            temp_a0_3 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&LobbyInfo + 0x16)) + temp_a1_4));
            if ((temp_a0_3 != 3) && (temp_a0_3 != 4)) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0xFU;
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 1U;
                SetDialogData(0x40, 0xF, temp_a2_2);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            } else {
                temp_v1_5 = temp_s0 & 0xFFFF;
                if (temp_v1_5 & 0x20) {
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = lobby_func_00569C20(temp_s1, temp_a1_4, temp_a2_2);
                } else if (temp_v1_5 & 0x40) {
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 1U;
                    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1E)) = lobby_func_00569320((*(s16 *)((u8 *)&ClassInfo + 0x6)), 7, (u8) temp_a2_2);
                    temp_a3 = (*(u8 *)((u8 *)&ClassInfo + 0x4)) - 1;
                    temp_a1_5 = temp_a3 >> 0x1F;
                    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = (s16) ((((*(u8 *)((u8 *)&ClassInfo + 0x4)) - 1) / 7) + temp_a1_5);
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B)) = (u8) ((s32) temp_a3 % 7);
                    cnWrap_SoundRequest(3, temp_a1_5, (void *)7, temp_a3);
                } else if (temp_v1_5 & 0x200) {
                    temp_a2_3 = (*(void **)&pNet);
                    if ((*(u8 *)((u8 *)(temp_a2_3) + 0x32)) & 2) {
                        (*(u8 *)((u8 *)(temp_a2_3) + 0x23)) = 5U;
                        cnWrap_SoundRequest(6, temp_a1_4, temp_a2_3);
                    } else {
                        (*(u8 *)((u8 *)(temp_a2_3) + 0x23)) = 4U;
                        g_lb_6B46A0 = 5;
                        g_lb_6B46A1 = 1;
                        g_lb_6B46A2 = temp_s1 + 1;
                        g_lb_6B46A4 = 2;
                        g_lb_6B46A5 = 4;
                        g_lb_6B46A6 = 0xFF;
                        cnWrap_SoundRequest(6, 4U, temp_a2_3);
                    }
                } else if ((temp_v1_5 & 0xC00) && ((*(s16 *)((u8 *)(&tl_member_buff) + 8)) >= 2)) {
                    (*(s16 *)((u8 *)(&tl_member_buff) + 6)) = (s16) ((*(s16 *)((u8 *)(&tl_member_buff) + 6)) ^ 1);
                    cnWrap_SoundRequest(1, temp_a1_4, temp_a2_2);
                }
            }
            goto block_128;
        case 4:                                     /* switch 1 */
            if ((*(u8 *)((u8 *)(temp_v0_2) + 0x32)) & 2) {
                lobby_func_00569D00(temp_a0, &jtbl_0063DFE0, temp_a2);
            } else {
                temp_v0_4 = lobby_func_005802F0(temp_a0, &jtbl_0063DFE0, temp_a2);
                switch (temp_v0_4) {                /* switch 2; irregular */
                case 0:                             /* switch 2 */
                    temp_v1_6 = (*(void **)&pNet);
                    (*(u8 *)((u8 *)(temp_v1_6) + 0x32)) = (u8) ((*(u8 *)((u8 *)(temp_v1_6) + 0x32)) | 2);
                    lobby_func_00569D00();
                    break;
                case 1:                             /* switch 2 */
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 3U;
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0xFU;
                    Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
                    break;
                }
            }
            goto block_128;
        case 5:                                     /* switch 1 */
            temp_a1_6 = temp_s1 * 0x15C;
            temp_a0_4 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&LobbyInfo + 0x16)) + temp_a1_6));
            if ((temp_a0_4 != 3) && (temp_a0_4 != 4)) {
                (*(u8 *)((u8 *)(temp_v0_2) + 0x23)) = 0xFU;
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 1U;
                SetDialogData(0x40, 1, (void *) temp_a2);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            } else {
                temp_a1_7 = temp_s0 & 0xFFFF;
                (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0, (s32 *) temp_a1_6, temp_a2);
                if (temp_a1_7 & 0x20) {
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = lobby_func_00569C20(temp_s1, temp_a1_7);
                } else if (temp_a1_7 & 0x40) {
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 1U;
                    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1E)) = lobby_func_00569320((*(s16 *)((u8 *)&ClassInfo + 0x6)), 7);
                    temp_a3_2 = (*(u8 *)((u8 *)&ClassInfo + 0x4)) - 1;
                    temp_a1_8 = temp_a3_2 >> 0x1F;
                    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = (s16) ((((*(u8 *)((u8 *)&ClassInfo + 0x4)) - 1) / 7) + temp_a1_8);
                    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B)) = (u8) ((s32) temp_a3_2 % 7);
                    cnWrap_SoundRequest(3, temp_a1_8, (void *)7, temp_a3_2);
                } else if (temp_a1_7 & 0x200) {
                    temp_v1_7 = (*(void **)&pNet);
                    if ((*(u8 *)((u8 *)(temp_v1_7) + 0x32)) & 1) {
                        (*(u8 *)((u8 *)(temp_v1_7) + 0x23)) = 3U;
                    } else {
                        (*(u8 *)((u8 *)(temp_v1_7) + 0x23)) = 2U;
                        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
                    }
                    cnWrap_SoundRequest(0, temp_a1_7);
                }
            }
            goto block_128;
        case 6:                                     /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            temp_v0_5 = Lbs_GetLobbyMemberList(temp_s1 & 0xFFFF, &jtbl_0063DFE0, temp_a2);
            switch (temp_v0_5) {                    /* switch 3; irregular */
            case 0:                                 /* switch 3 */
                temp_v1_8 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_8) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_8) + 0x23)) + 1);
                lobby_func_0056A810(&tl_member_buff);
                break;
            case 1:                                 /* switch 3 */
                Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 1U;
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0xFU;
                break;
            }
            goto block_128;
        case 7:                                     /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            temp_v0_6 = lobby_func_0056A850(&tl_member_buff, &jtbl_0063DFE0, temp_a2);
            switch (temp_v0_6) {                    /* switch 4; irregular */
            case 0:                                 /* switch 4 */
                temp_v1_9 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_9) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_9) + 0x23)) + 1);
                SetDialogData(0x1B);
                break;
            case 3:                                 /* switch 4 */
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 1U;
                break;
            }
            goto block_128;
        case 8:                                     /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            temp_v0_7 = lobby_func_0057FBB0((*(s8 *)((u8 *)(((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + (*(s16 *)((u8 *)(&tl_member_buff) + 0)))) + 0x208)), &jtbl_0063DFE0, temp_a2);
            switch (temp_v0_7) {                    /* switch 5; irregular */
            case 0:                                 /* switch 5 */
                temp_v1_10 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_10) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_10) + 0x23)) + 1);
                break;
            case 1:                                 /* switch 5 */
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0xFU;
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 1U;
                (*(u8 *)((u8 *)&ClassInfo + 0x4)) = 0;
                Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
                break;
            }
            goto block_128;
        case 9:                                     /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            temp_v0_8 = lbc_in_plaza_03((*(s16 *)((u8 *)(&tl_member_buff) + 0)), &jtbl_0063DFE0, temp_a2);
            switch (temp_v0_8) {                    /* switch 6; irregular */
            case 0:                                 /* switch 6 */
                temp_v1_11 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_11) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_11) + 0x23)) + 1);
                break;
            case 1:                                 /* switch 6 */
                Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 1U;
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0xFU;
                break;
            }
            goto block_128;
        case 10:                                    /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            temp_v0_9 = lobby_func_0057FC80(temp_a0, &jtbl_0063DFE0, temp_a2);
            switch (temp_v0_9) {                    /* switch 7; irregular */
            case 0:                                 /* switch 7 */
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0xDU;
                break;
            case 1:                                 /* switch 7 */
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0xFU;
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 0U;
                (*(u8 *)((u8 *)&ClassInfo + 0x4)) = 0;
                Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
                break;
            }
            goto block_128;
        case 11:                                    /* switch 1 */
            temp_a2_4 = temp_s1 * 0x15C;
            temp_a1_9 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&LobbyInfo + 0x16)) + temp_a2_4));
            if ((temp_a1_9 != 3) && (temp_a1_9 != 4)) {
                (*(u8 *)((u8 *)(temp_v0_2) + 0x23)) = 0xFU;
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 1U;
                SetDialogData(0x40, 1, (void *) temp_a2_4);
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
            } else {
                (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
                temp_v0_10 = Lb_select(4, temp_a1_9, temp_a2_4);
                switch (temp_v0_10) {               /* switch 8; irregular */
                case 0:                             /* switch 8 */
                    SetDialogData(0x1B);
                    temp_v1_12 = (*(void **)&pNet);
                    (*(u8 *)((u8 *)(temp_v1_12) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_12) + 0x23)) + 1);
                    break;
                case 3:                             /* switch 8 */
                    temp_v1_13 = (*(void **)&pNet);
                    (*(u8 *)((u8 *)(temp_v1_13) + 0x23)) = (u8) (*(u8 *)((u8 *)(temp_v1_13) + 0x26));
                    break;
                }
            }
            goto block_128;
        case 12:                                    /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            temp_v0_11 = lb_chatMemberCheck(temp_a0, &jtbl_0063DFE0, temp_a2);
            switch (temp_v0_11) {                   /* switch 9; irregular */
            case 0:                                 /* switch 9 */
                temp_v1_14 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_14) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_14) + 0x23)) + 1);
                break;
            case 1:                                 /* switch 9 */
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0xFU;
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x26)) = 0U;
                (*(u8 *)((u8 *)&ClassInfo + 0x4)) = 0;
                Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
                break;
            }
            goto block_128;
        case 13:                                    /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            temp_v0_12 = lobby_func_00581210(temp_a0, &jtbl_0063DFE0, temp_a2);
            if ((temp_v0_12 != 1) && (temp_v0_12 != 0)) {

            } else {
                temp_v1_15 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_15) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_15) + 0x23)) + 1);
            }
            goto block_128;
        case 14:                                    /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            temp_v0_13 = lobby_func_00580FA0(temp_a0, &jtbl_0063DFE0, temp_a2);
            if ((temp_v0_13 != 0) && (temp_v0_13 != 1)) {
                goto block_128;
            }
            (*(s32 *)((u8 *)((*(void **)&pNet)) + 0x14)) = 0;
            return 0;
        case 15:                                    /* switch 1 */
            (*(s8 *)((u8 *)(temp_v0_2) + 0x2D)) = 1;
            if (temp_s0 & 0xFFFF & 0x20) {
                temp_v1_16 = (*(void **)&pNet);
                (*(u8 *)((u8 *)(temp_v1_16) + 0x23)) = (u8) (*(u8 *)((u8 *)(temp_v1_16) + 0x26));
                cnWrap_SoundRequest(0, (u32) &jtbl_0063DFE0, (void *) temp_a2);
            }
            goto block_128;
        }
    } else {
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
        return 1;
    }
}

