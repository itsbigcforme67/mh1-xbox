/* G online (PC): Lb_make_quest_tbl, written from Monster Hunter G's code at lobby 0x55FA40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFA68;
extern u8 g_main_2EFA6C;
extern u8 g_main_2EFA68;
extern u8 g_main_2EFA6C;
extern u8 guildStr;
extern u8 jtbl_2350;
s32 Quest_clear_bit_ck();                        /* extern */
u16 get_flag_quest();                            /* extern */
s32 get_new_quest();                    /* extern */
s64 lb_get_quest_level();                    /* extern */
s32 lb_guild_check_keyQuest();              /* extern */
s32 lb_key_quest_ck();                           /* extern */
s32 lobby_func_005434A0();                       /* extern */
s32 lobby_func_00543520(); /* extern */
s32 lobby_func_005438F0();                       /* extern */
s32 lobby_func_0055EB40();                       /* extern */
s32 lobby_func_0055FA20();                 /* extern */
s32 memset();             /* extern */
s32 ran_suu();                               /* extern */
extern u16 lb_quest_all;
extern u16 *quest_lv_tbl;

void Lb_make_quest_tbl(void) {
    u8 g_frame[0x140] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(u8 * *)(g_frame + 0xA0))
#define spB0 (*(u8 * *)(g_frame + 0xB0))
#define spC0 (*(u8 * *)(g_frame + 0xC0))
#define spD0 (*(s8 * *)(g_frame + 0xD0))
#define spE0 (*(s8 * *)(g_frame + 0xE0))
#define spF0 (*(s8 * *)(g_frame + 0xF0))
#define sp100 (*(u8 * *)(g_frame + 0x100))
#define sp110 (*(u16 *)(g_frame + 0x110))
#define sp120 (*(s32 *)(g_frame + 0x120))
#define sp130 (*(u32 *)(g_frame + 0x130))
    s16 var_v1;
    s32 temp_a1;
    s32 temp_s1;
    s32 var_a1;
    s32 var_s0_2;
    s32 var_s0_3;
    s32 var_s0_4;
    s32 var_s0_5;
    s32 var_s6;
    s32 var_s6_2;
    s8 *var_s7;
    u8 *var_fp;   /* (u16 **) */
    u8 *var_a0;   /* (u16 *) */
    u8 *var_a0_2;   /* (u16 *) */
    u8 *var_a0_3;   /* (u16 *) */
    u8 *var_s0;   /* (u16 *) */
    u8 *var_s1;   /* (u16 *) */
    u8 *var_s2;   /* (u16 *) */
    u8 *var_s2_2;   /* (u16 *) */
    u8 *var_s3_2;   /* (u16 *) */
    u8 *var_s3_3;   /* (u16 *) */
    u8 *var_s5;   /* (u16 *) */
    u16 temp_a0;
    u16 temp_a0_2;
    u16 temp_a2;
    u16 temp_a2_2;
    u16 temp_s4;
    u16 temp_v1;
    u16 temp_v1_2;
    u16 temp_v1_3;
    u32 temp_fp;
    u32 temp_hi;
    u32 var_s3;

    (*(u16 *)&g_main_2EFA6C) = 0U;
    (*(s16 *)&g_main_2EFA68) = 0;
    sp110 = 0;
    memset(&(*(u8 *)((u8 *)&guildStr + 0x100)), 0, 0xA0);
    sp130 = (u32) ((s64) (lb_get_quest_level(1) << 0x38) >> 0x38);
    var_s6 = 0;
    var_fp = &quest_lv_tbl;
    spE0 = &(*(u8 *)((u8 *)&guildStr + 0x120));
    var_s5 = &lb_quest_all;
    var_s7 = &(*(u8 *)((u8 *)&guildStr + 0x100));
    spD0 = &(*(u8 *)((u8 *)&guildStr + 0x110));
    do {
        var_s2 = (*(u16 **)var_fp);
        var_s3 = 0;
        var_s0 = var_s5;
        var_s1 = var_s2;
        *spE0 = 0;
        *spD0 = 0;
loop_2:
        temp_a0 = (*(u16 *)var_s1);
        if (temp_a0 != 0) {
            if (lobby_func_005434A0(temp_a0) == 0) {
                var_s3 += 1;
                (*(u16 *)var_s0) = (*(u16 *)var_s1);
                var_s0 += 2;
            }
            var_s1 += 2;
            if (var_s3 < 8U) {
                goto loop_2;
            }
        }
        var_s0_2 = 0;
        var_s3_2 = var_s5;
loop_12:
        if (var_s0_2 < 8) {
            temp_a0_2 = (*(u16 *)var_s2);
            if (temp_a0_2 != 0) {
                if (((s32) temp_a0_2 < 0x2710) && (lobby_func_005438F0(temp_a0_2) != 1) && (lobby_func_005434A0((*(u16 *)var_s2)) == 0)) {
                    var_s0_2 += 1;
                    (*(u16 *)var_s3_2) = (*(u16 *)var_s2);
                    var_s3_2 += 2;
                }
                var_s2 += 2;
                goto loop_12;
            }
        }
        if (var_s6 == 7) {
            (*(s16 *)((u8 *)(var_s5) + 0xE)) = 0x802;
        }
        *var_s7 = 8;
        var_s6 += 1;
        var_fp += 4;
        var_s5 += 0x10;
        spE0 += 1;
        spD0 += 1;
        var_s7 += 1;
    } while (var_s6 < 8);
    (*(u8 *)((u8 *)&lb_quest_all + 0x80)) = 0;
    spC0 = &(*(u8 *)((u8 *)&lb_quest_all + 0x82));
    (*(u8 *)((u8 *)&lb_quest_all + 0x90)) = 0;
    spB0 = &(*(u8 *)((u8 *)&lb_quest_all + 0x84));
    var_s6_2 = 0;
    var_s3_3 = &lb_quest_all;
    (*(u8 *)((u8 *)&lb_quest_all + 0x82)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x92)) = 0;
    spA0 = &(*(u8 *)((u8 *)&lb_quest_all + 0x84)) + 2;
    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_quest_all + 0x84))) + 0)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x94)) = 0;
    sp100 = &quest_lv_tbl;
    spF0 = &(*(u8 *)((u8 *)&guildStr + 0x120));
    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&lb_quest_all + 0x84))) + 2)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x96)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x88)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x98)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x8A)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x9A)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x8C)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x9C)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x8E)) = 0;
    (*(u8 *)((u8 *)&lb_quest_all + 0x9E)) = 0;
    do {
        var_s2_2 = (*(u16 **)sp100);
        temp_s4 = get_new_quest(var_s6_2, 0) & 0xFFFF;
        if ((temp_s4 == 0) && !(get_new_quest(var_s6_2, 1) & 0xFFFF)) {
            *spF0 = 1;
        }
        if (sp130 == var_s6_2) {
            sp110 = get_flag_quest(sp130);
            temp_a1 = lobby_func_0055EB40(sp130) & 0xFFFF;
            if ((sp110 == 0) && (lb_guild_check_keyQuest(sp130, temp_a1) == 1) && !(M2C_ERROR(/* Read from unset register $a1 */) & 0xFFFF)) {
                switch (sp130) {
                case 0:
                    if (Quest_clear_bit_ck(0x27U) == 0) {
                        (*(u16 *)&g_main_2EFA6C) = 0x27U;
                    }
                    break;
                case 1:
                    if (Quest_clear_bit_ck(9U) == 0) {
                        (*(u16 *)&g_main_2EFA6C) = 9U;
                    }
                    break;
                case 2:
                    if (Quest_clear_bit_ck(0xBB9U) == 0) {
                        (*(u16 *)&g_main_2EFA6C) = 0xBB9U;
                    }
                    break;
                case 3:
                    if (Quest_clear_bit_ck(0x40BU) == 0) {
                        (*(u16 *)&g_main_2EFA6C) = 0x40BU;
                    }
                    break;
                case 4:
                    if (Quest_clear_bit_ck(0xBBAU) == 0) {
                        (*(u16 *)&g_main_2EFA6C) = 0xBBAU;
                    }
                    break;
                case 5:
                    if (Quest_clear_bit_ck(0x7ECU) == 0) {
                        (*(u16 *)&g_main_2EFA6C) = 0x7ECU;
                    }
                    break;
                case 6:
                    if (Quest_clear_bit_ck(0x802U) == 0) {
                        (*(u16 *)&g_main_2EFA6C) = 0x802U;
                    }
                    break;
                case 7:
                    if (lobby_func_0055FA20(&jtbl_2350) == 1) {
                        (*(u16 *)&g_main_2EFA6C) = 0xBBCU;
                    } else {
                        (*(u16 *)&g_main_2EFA6C) = 0xBBBU;
                    }
                    break;
                }
            } else {
                var_s0_3 = 0;
                var_a0 = var_s3_3;
loop_48:
                if (sp110 == (*(u16 *)var_a0)) {
                    lobby_func_00543520(var_s3_3, sp110, 7, 1);
                } else {
                    var_s0_3 += 1;
                    var_a0 += 2;
                    if (var_s0_3 < 8) {
                        goto loop_48;
                    }
                }
                if ((var_s0_3 == 8) && (sp110 != 0)) {
                    lobby_func_00543520(var_s3_3, sp110, 7, 1);
                }
            }
        }
        if (temp_s4 != 0) {
            var_s0_4 = 0;
            sp120 = 1;
            var_a0_2 = var_s3_3;
loop_57:
            if (temp_s4 == (*(u16 *)var_a0_2)) {
                lobby_func_00543520(var_s3_3, temp_s4, 0, 1);
            } else {
                var_s0_4 += 1;
                var_a0_2 += 2;
                if (var_s0_4 < 8) {
                    goto loop_57;
                }
            }
            if ((var_s0_4 == 8) && (temp_s4 != 0)) {
                lobby_func_00543520(var_s3_3, temp_s4, 0, 1);
            }
        } else {
            sp120 = 0;
        }
        if (var_s6_2 == 0) {
            temp_a2 = (!(ran_suu(1) & 0xFFFF & 1) ? 3 : 1) & 0xFFFF;
            var_a1 = 1;
            var_a0_3 = var_s3_3 + 2;
            sp120 += 1;
loop_68:
            if (temp_a2 == (*(u16 *)var_a0_3)) {
                var_s3_3[var_a1] = (*(u16 *)((u8 *)(var_s3_3) + 0));
                (*(u16 *)((u8 *)(var_s3_3) + 0)) = temp_a2;
            } else {
                var_a1 += 1;
                var_a0_3 += 2;
                if (var_a1 < 8) {
                    goto loop_68;
                }
            }
            if (var_a1 == 8) {
                temp_v1 = (*(u16 *)((u8 *)(var_s3_3) + 0));
                if (temp_a2 != temp_v1) {
                    (*(u16 *)((u8 *)(var_s3_3) + 2)) = temp_v1;
                    (*(u16 *)((u8 *)(var_s3_3) + 0)) = temp_a2;
                }
            }
        }
        var_s0_5 = 0;
        if ((*(u16 *)var_s2_2) != 0) {
            temp_fp = 8 - sp120;
            do {
                if (((u32) (ran_suu(1) & 0xFFFF) >> (var_s0_5 % 16)) & 1) {
                    temp_v1_2 = (*(u16 *)var_s2_2);
                    if ((sp110 != temp_v1_2) && (temp_s4 != temp_v1_2) && ((s32) temp_v1_2 < 0x2710) && ((s32) temp_v1_2 < 0xBBC)) {
                        temp_hi = (u32) (ran_suu(1) & 0xFFFF) % temp_fp;
                        if (temp_fp == 0) {
                            M2C_BREAK(0);
                        }
                        temp_s1 = sp120 + temp_hi;
                        temp_v1_3 = var_s3_3[temp_s1];
                        if ((sp110 != temp_v1_3) && ((temp_s4 == 0) || (temp_s4 != temp_v1_3)) && ((temp_s1 != 0) || (temp_s4 == 0))) {
                            if (lb_key_quest_ck((*(u16 *)var_s2_2)) != 0) {
                                if (Quest_clear_bit_ck((*(u16 *)var_s2_2)) == 1) {
                                    goto block_92;
                                }
                            } else {
block_92:
                                if (lobby_func_005438F0((*(u16 *)var_s2_2)) != 1) {
                                    lobby_func_00543520(var_s3_3, (*(u16 *)var_s2_2), temp_s1 & 0xFF, 0);
                                }
                            }
                        }
                    }
                }
                var_s2_2 += 2;
                var_s0_5 += 1;
            } while ((*(u16 *)var_s2_2) != 0);
        }
        var_s6_2 += 1;
        var_s3_3 += 0x10;
        sp100 += 4;
        spF0 += 1;
    } while (var_s6_2 < 8);
    temp_a2_2 = (*(u16 *)&g_main_2EFA6C);
    if (temp_a2_2 != 0) {
        if (temp_a2_2 != 0xBBC) {
            var_v1 = 1;
            (*(u8 *)((u8 *)&lb_quest_all + 0x80)) = temp_a2_2;
        } else {
            (*(u8 *)((u8 *)&lb_quest_all + 0x80)) = temp_a2_2;
            (*(s16 *)spC0) = temp_a2_2 + 1;
            (*(s16 *)spB0) = temp_a2_2 + 2;
            (*(s16 *)spA0) = temp_a2_2 + 3;
            var_v1 = 5;
            (*(u8 *)((u8 *)&lb_quest_all + 0x88)) = 0xBBB;
        }
        (*(s16 *)&g_main_2EFA68) = var_v1;
    }
    (*(u8 *)((u8 *)&guildStr + 0x108)) = (s8) (*(s16 *)&g_main_2EFA68);
}

