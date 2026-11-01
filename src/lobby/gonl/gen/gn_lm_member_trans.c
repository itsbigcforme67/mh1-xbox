/* G online (PC): lm_member_trans, written from Monster Hunter G's code at lobby 0x58C460 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispFrameList();
s32 Disp_FriendListEntry();
s32 Lb_PlayerStatus();
s32 Lb_put_icon_free();
s32 Lb_put_status();
extern u8 PitMenu;
extern u8 System_timer;
extern u8 cw;
extern u8 game_w;
extern u8 join_member;
extern u8 pNet;
extern u8 tbl;
s32 Disp_lb_menu();          /* extern */
s32 Lb_checkChatID();                            /* extern */
s32 Lb_get_pl_stat2();                           /* extern */
s32 Online_ck();                                    /* extern */
s32 sprintf(); /* extern */
extern s32 g_lb_63E880;
extern s32 lbCommer;
extern s32 lb_player;
extern s32 lb_quest_color_tbl;
extern s32 plData;
extern s32 *pl_name_tbl;
extern u8 player_work;

void lm_member_trans(s32 unused_a0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    u8 g_frame[0xB0] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(u8 *)(g_frame + 0xA0))
    u8 *var_s1;   /* (s32 **) */
    u8 *var_fp;   /* (s32 *) */
    u8 *var_s2;   /* (s32 *) */
    u8 *var_s2_2;   /* (s32 *) */
    u8 *var_s4;   /* (s32 *) */
    u8 *var_s7;   /* (s32 *) */
    s32 var_a0;
    s32 var_s0;
    s32 var_s1_2;
    s32 var_s3;
    s32 var_s3_2;
    s32 var_s0_2;
    s32 var_v0;
    u8 *var_s5;
    u8 *var_s6;
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_a2_2;
    u8 var_a2;
    u8 *temp_a0_2;
    u8 *temp_a2;
    u8 *temp_v1;

    temp_a2 = (*(void **)&pNet);
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0x22));
    var_a0 = 0x80;
    switch (temp_a1) {                              /* irregular */
    case 0:
        Disp_lb_menu(1, temp_a1, temp_a2);
        var_s5 = &spA0;
        var_s0 = 0;
        var_s4 = &player_work;
        var_s3 = 0;
        var_s7 = &lbCommer;
        var_s2 = &(*(s32 *)((u8 *)&join_member + 0x240));
        var_s6 = var_s5;
        var_s1 = &pl_name_tbl;
        do {
            if (var_s0 != (*(u8 *)((u8 *)&game_w + 0x3A))) {
                if ((*(u8 *)((u8 *)(var_s4) + 0)) != 0) {
                    var_fp = (*(s32 *)&cw) + var_s3 + 0x394;
                    if (Online_ck() == 1) {
                        if ((*(u8 *)((u8 *)&PitMenu + 0x14)) != 0) {
                            var_fp = var_s7;
                        }
                        *var_s6 = (*(u8 *)((u8 *)(((*(s32 *)&cw) + var_s3)) + 0x3AA));
                    }
                    if (Lb_checkChatID(var_s0) == 1) {
                        var_v0 = 6;
                    } else {
                        var_v0 = 0xA;
                        if ((*(u8 *)((u8 *)(var_s4) + 0x736)) == 0) {

                        } else {
                            var_v0 = 0;
                        }
                    }
                    sprintf(var_s2, &g_lb_63E880, ((s16) (var_v0)), var_fp);
                    (*(s32 **)var_s1) = var_s2;
                } else {
                    (*(s32 **)var_s1) = (*(u8 **)&tbl);
                }
                var_s2 += 0x14;
                var_s1 += 4;
            }
            var_s0 += 1;
            var_s4 += 0xA00;
            var_s3 += 0x5C;
            var_s7 += 0x5C;
            var_s6 += 1;
        } while (var_s0 < 0xA);
        var_a2 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29));
        if ((s32) (*(u8 *)((u8 *)&game_w + 0x3A)) < (s32) var_a2) {
            var_a2 -= 1;
        }
        (*(s32 ** *)((u8 *)&plData + 0xC)) = &pl_name_tbl;
        DispFrameList(&plData, NULL, (s32) var_a2);
        var_s3_2 = 0x3E;
        var_s0_2 = 0;
        var_s2_2 = &player_work;
        var_s1_2 = 0;
        do {
            if (var_s0_2 != (*(u8 *)((u8 *)&game_w + 0x3A))) {
                if ((*(u8 *)((u8 *)(var_s2_2) + 0)) != 0) {
                    if (Lb_get_pl_stat2(((s8) (var_s0_2))) == 0) {
                        Lb_put_status(0x1A4, var_s3_2, 0x12, -1, (s32) *var_s5);
                    }
                    if ((*(u8 *)((u8 *)(var_s2_2) + 0x736)) != 0) {
                        temp_a0 = (*(u8 *)((u8 *)(((*(s32 *)&cw) + var_s1_2)) + 0x3B8));
                        if (temp_a0 & 0xC0) {
                            if (!(temp_a0 & 0x40)) {
                                if ((*(u16 *)&System_timer) & 0x10) {
                                    goto block_36;
                                }
                            } else {
block_36:
                                Lb_put_icon_free(0x18E, var_s3_2, 0x12, (*(s32 *)((u8 *)&lb_quest_color_tbl + ((temp_a0 & 0xF) * 4))), 0xF);
                            }
                        } else if (temp_a0 & 0x30) {
                            if (!(temp_a0 & 0x10)) {
                                if ((*(u16 *)&System_timer) & 0x10) {
                                    goto block_42;
                                }
                            } else {
block_42:
                                Lb_put_icon_free(0x18E, var_s3_2, 0x12, (*(s32 *)((u8 *)&lb_quest_color_tbl + ((temp_a0 & 0xF) * 4))), 0xE);
                            }
                        }
                    }
                }
                var_s3_2 = (s32) (((s16) ((var_s3_2 + 0x16))));
            }
            var_s0_2 += 1;
            var_s2_2 += 0xA00;
            var_s5 += 1;
            var_s1_2 += 0x5C;
        } while (var_s0_2 < 0xA);
        return;
    case 1:
        var_a0 = 0;
        /* fallthrough */
    case 2:
        if ((*(u8 *)((u8 *)(temp_a2) + 0x2D)) != 0) {
            var_a0 = 0x80;
        }
        temp_v1 = (*(void **)&pNet);
        temp_a2_2 = (*(u8 *)((u8 *)(temp_v1) + 0x29));
        Lb_PlayerStatus((u8 *)&lb_player + (temp_a2_2 * 0x38), ((*(u8 *)((u8 *)(temp_v1) + 0x23)) | (var_a0 & 0xFF)) & 0xFF, (s32) temp_a2_2, arg3);
        temp_a0_2 = (*(void **)&pNet);
        if ((*(u8 *)((u8 *)(temp_a0_2) + 0x23)) == 0) {
            Disp_FriendListEntry(0x110, ((*(u8 *)((u8 *)(temp_a0_2) + 0x22)) - 1) & 0xFF, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        return;
    }
}

