/* Player code (f_pl.s, 0x134950..): working file; matched functions are moved
 * to plX.c, what is left here is near-match (not built). */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
f32 flSqrt(f32);
extern u8 Gun_data[26][0x14];

void player_init0(PLW *pl) {
    u32 i;
    s8 *eq;
    u8 *mx;
    if (game_w.pl_state[pl->id] == 0xFF && Pl_master_ck(pl) == 0) {
        pl->be_flag = 0;
        pl->x01 = 0;
        return;
    }
    pl->x10 = 0;
    pl->work01E = 0;
    pl->work300 = 2;
    pl->work350 = 0;
    pl->work351 = -1;
    if (pl->work616 != 0) {
        mx = parts_max_tbl;
        eq = &equip_set[pl->work616 * 6 + pl->work011 * 0x24];
        for (i = 0; i < 6; i++) {
            if (*eq < 0) {
                pl->work352[i] = (ran_suu(1) & 0xFFFF) % mx[pl->work011 * 6] + 1;
            } else if (i == 2 && pl->work011 != 0 && softdip_ck(0x50) != 0) {
                pl->work352[i] = 10;
            } else {
                pl->work352[i] = *eq;
            }
            mx++;
            eq++;
        }
        pl->work5FC = test_hair_col[pl->work616 + pl->work011 * 6] | 0xFF000000;
    }
    weapon_create_model(pl->work34C, pl->id, 0);
    armor_create_model(pl);
    yure_init(pl);
}

void pl_work_clr(PLW *pl, u8 no, PLPROG *prog) {
    pl->id = (u8)no;
    if (game_w.pl_state[pl->id] == 1 || Pl_master_ck(pl) == 1) {
        pl->x01 = 1;
    } else {
        pl->x01 = 0;
    }
    pl->prog = prog;
    pl_init_sub(pl);
    pl->chr_no0 = (u8)no + 1;
    pl->prog->init(pl);
    pl->prog->init2(pl);
}

void timer_calc_sub_pl(PLW *pl) {
    s16 temp_v0_5;
    s16 temp_v0_6;
    s16 temp_v1_4;
    s16 temp_v1_5;
    int temp_s1;
    int var_a1;
    s8 temp_v1_10;
    u16 temp_v0_10;
    u16 temp_v0_11;
    u16 temp_v0_12;
    u16 temp_v0_7;
    u16 temp_v0_8;
    u16 temp_v0_9;
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_a2;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 temp_v0_3;
    u8 temp_v0_4;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;
    u8 temp_v1_6;
    u8 temp_v1_7;
    u8 temp_v1_8;
    u8 temp_v1_9;

    temp_v0 = pl->work936;
    if (temp_v0 != 0) {
        pl->work936 = (u8) (temp_v0 - 1);
    }
    temp_v0_2 = pl->work7ED;
    if (temp_v0_2 != 0) {
        pl->work7ED = (u8) (temp_v0_2 - 1);
    }
    temp_v0_3 = pl->work4DD;
    if (temp_v0_3 != 0) {
        pl->work4DD = (u8) (temp_v0_3 - 1);
    }
    temp_v0_4 = pl->x8C6;
    if (temp_v0_4 != 0) {
        pl->x8C6 = (u8) (temp_v0_4 - 1);
    }
    temp_v0_5 = pl->work760;
    if (temp_v0_5 != 0) {
        pl->work760 = (s16) (temp_v0_5 - 1);
    }
    temp_v0_6 = pl->work8CC;
    if (temp_v0_6 != 0) {
        pl->work8CC = (s16) (temp_v0_6 - 1);
        if ((pl->work8CC == 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(0, 0x15, 0);
        }
    }
    temp_v0_7 = pl->work918;
    if (temp_v0_7 != 0) {
        pl->work918 = (u16) (temp_v0_7 - 1);
        if ((pl->work918 == 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(0, 0x12, 0);
        }
    }
    temp_v0_8 = pl->work91A;
    if (temp_v0_8 != 0) {
        pl->work91A = (u16) (temp_v0_8 - 1);
        if ((pl->work91A == 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(0, 0x13, 0);
        }
    }
    temp_v0_9 = pl->work91C;
    if (temp_v0_9 != 0) {
        pl->work91C = (u16) (temp_v0_9 - 1);
        if ((pl->work91C == 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(0, 0x16, 0);
        }
    }
    temp_v0_10 = pl->work930;
    if (temp_v0_10 != 0) {
        pl->work930 = (u16) (temp_v0_10 - 1);
    }
    temp_v0_11 = pl->work6A6;
    if (temp_v0_11 != 0) {
        pl->work6A6 = (u16) (temp_v0_11 - 1);
        if (pl->work6A6 == 0) {
            pl->work6A5 = 0;
        }
    }
    temp_v0_12 = pl->work6AA;
    if (temp_v0_12 != 0) {
        pl->work6AA = (u16) (temp_v0_12 - 1);
        if (pl->work6AA == 0) {
            pl->work6A9 = 0;
        }
    }
    if (pl_flag_ck(pl, 0x200) == 0) {
        temp_v1 = pl->flag14;
        if ((temp_v1 != 3) && (temp_v1 != 2)) {
            switch (temp_v1) {                      /* irregular */
            case 0:
                temp_v1_2 = pl->flag15;
                if ((temp_v1_2 != 0x6E) && (temp_v1_2 != 0x6D) && (temp_v1_2 != 0x6C) && (temp_v1_2 != 0x6B) && (temp_v1_2 != 0x44) && (temp_v1_2 != 0x32) && (temp_v1_2 != 0x31) && (temp_v1_2 != 0x2D) && (temp_v1_2 != 0x1C)) {
                    Pl_stamina_calc(pl, 2);
                }
                break;
            case 1:
                temp_v1_3 = pl->flag15;
                if ((temp_v1_3 != 0x4C) && (temp_v1_3 != 0x27) && (temp_v1_3 != 0x26) && (temp_v1_3 != 0x25) && (temp_v1_3 != 0x22) && (temp_v1_3 != 0x1B) && (temp_v1_3 != 0x1A) && (temp_v1_3 != 0x18)) {
                    Pl_stamina_calc(pl, 2);
                }
                break;
            default:
                Pl_stamina_calc(pl, 2);
                break;
            }
        }
    }
    Pl_stamina_reduce(pl);
    temp_a0 = pl->flag14;
    if ((temp_a0 != 2) && (temp_a0 != 3)) {
        if (((((int) (Stage_env_ck(pl->stg) << 0x30) >> 0x30) != 1) || (Pl_Skill_ck(pl, 0x2D) != 0) || (pl->work918 != 0)) && (pl->x7BA == 0) && !(PU16(&game_w, 0x1E) & 0x3F) && (pl->vital < pl->vital_red)) {
            if ((Pl_Skill_ck(pl, 0x1B) == 1) || (pl->work91C != 0)) {
                var_a1 = (int) ((pl->vital_red - pl->vital) << 0x30) >> 0x30;
                if (var_a1 >= 4) {
                    var_a1 = 3;
                }
                Pl_vital_calc(pl, var_a1);
            } else {
                Pl_vital_calc(pl, 1);
            }
        }
        temp_v1_4 = pl->x7AC;
        if (temp_v1_4 > 0) {
            pl->x7AC = (s16) (temp_v1_4 - 1);
        } else {
            pl->x7AC = 0;
            temp_v1_5 = pl->x7AA;
            if (temp_v1_5 != 0) {
                pl->x7AA = (s16) (temp_v1_5 - 1);
            }
        }
    }
    temp_v1_6 = pl->x763;
    if (temp_v1_6 != 0) {
        pl->x763 = (u8) (temp_v1_6 - 1);
    }
    temp_v1_7 = pl->work8BE;
    if (temp_v1_7 != 0) {
        pl->work8BE = (u8) (temp_v1_7 - 1);
    }
    temp_v1_8 = pl->work886;
    if (temp_v1_8 != 0) {
        pl->work886 = (u8) (temp_v1_8 - 1);
    }
    temp_a2 = pl->work56B;
    temp_a1 = temp_a2 & 0xF;
    if ((temp_a1 != 0) && (pl->flag14 == 5)) {
        pl->work56B = temp_a1;
        pl->work56B = (u8) (pl->work56B - 1);
        pl->work56B = (u8) (pl->work56B | (temp_a2 & 0xF0 & 0xFF));
    } else if ((((int) (act_ck(pl, 0, 0x47) << 0x30) >> 0x30) == 0) && (((int) (act_ck(pl, 0, 0x4A) << 0x30) >> 0x30) == 0) && (((int) (act_ck(pl, 0, 0x51) << 0x30) >> 0x30) == 0) && (((int) (act_ck(pl, 0, 0x5D) << 0x30) >> 0x30) == 0) && (((int) (act_ck(pl, 0, 0x60) << 0x30) >> 0x30) == 0)) {
        temp_s1 = Pl_hold_item_ck(pl) & 0xFFFF;
        if (temp_s1 != 0xFFFF) {
            if ((Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
                unmei_se(pl);
                set01_set(1, 5, (int) (temp_s1 << 0x30) >> 0x30);
            }
            Pl_item_stack(pl, temp_s1, -0xA);
        }
    }
    temp_v1_9 = pl->x56F;
    if (temp_v1_9 != 0) {
        pl->x56F = (u8) (temp_v1_9 - 1);
    }
    temp_v1_10 = (*(s8 *)&pl->x881);
    if (temp_v1_10 != 0) {
        pl->x881 = (s8) (temp_v1_10 - 1);
    }
}

void pl_dm_value_sub(PLW *pl) {
    f32 pos[3];
    s16 e, t;
    if (pl->flag14 == 3) return;
    if (pl->x7BA > 0) {
        t = pl->work7BE - 1;
        pl->work7BE = t;
        if (t <= 0) {
            pl->x7BA = pl->x7BA - 1;
            pl->work7BE = 0x1E;
            if (Pl_master_ck(pl) == 1) {
                Pl_vital_calc(pl, -1);
                pos[1] = 20.0f;
                pos[0] = 0;
                pos[2] = 0;
                Eft06_set2(1.0f, pl, 3, 0x14, pos);
            }
            if (pl->vital <= 0 && Pl_master_ck(pl) == 1) {
                pl->vital_red = 0;
                if (pl->st != 2) {
                    func_639F20(pl);
                } else if (act_ck(pl, 0, 9) == 0) {
                    Pl_act_set(pl, 0, 9, 0);
                }
            }
        }
    }
    if (Pl_master_ck(pl) == 0 && (pl->work4D5 & 1) && (*(u16 *)&game_w.x1E % 30) == 0) {
        pos[1] = 20.0f;
        pos[0] = 0;
        pos[2] = 0;
        Eft06_set2(1.0f, pl, 3, 0x14, pos);
    }
    if (Pl_master_ck(pl) == 1) {
        e = Stage_env_ck(pl->stg);
        switch (e) {
        case 2:
            break;
        case 1:
            if (Pl_Skill_ck(pl, 0x2D) != 1 && pl->work918 == 0) {
                t = pl->work90C - 1;
                pl->work90C = t;
                if (t <= 0) {
                    pl->work90C = 0x5A;
                    Pl_vital_calc(pl, -1);
                    if (pl->vital <= 0) {
                        pl->vital_red = 0;
                        if (pl->st != 2) {
                            func_639F20(pl);
                            return;
                        }
                        Pl_act_set(pl, 0, 9, 0);
                    }
                }
            }
            break;
        default:
            return;
        }
    }
}

int stick_pow_get(PLW *pl, int arg1) { /* arg1 unused, callers pass 0 */
    u8 r = 0;
    u16 p;
    switch (pl->kind) {
    case 1:
    case 5:
        if (pl->flag12 != 0 && pl->pch_on == 1) return 0;
    default:
    case 0:
    case 2:
    case 3:
    case 4:
        pl->x8C8 = 0;
        p = pl->sw.pow[0];
        if (p >= 0x78) {
            r = 3;
        } else if (p >= 0x55) {
            r = 1;
        } else if (p >= 0x28) {
            r = 1;
        }
        if (pl->st == 0 && pl->flag12 == 0 && (pl->sw.now & 0x10) && r && (act_ck(pl, 0, 0x24) == 0 || pl->work760 == 0)) return 5;
        return r;
    }
}

void em_ninshiki_ck(PLW *pl) {
    int i;
    EMW *em;
    int bit;
    u8 k;
    em = em_work;
    pl->work81F = pl->work81E;
    bit = (1 << pl->id) & 0xFF;
    pl->work81E = 0;
    for (i = 0; i < 20; i++) {
        if (em->stg == pl->stg && em->be_flag != 0 && em->x01 != 0 && em->x9EC != 0 && em->mode != 5
            && (em->x88F & bit) && em->x888 == 1
            && (em->x7EE |= bit, pl->work81E = 1, pl->x3B0 = em,
                pl->x3A8 = ((calc_vec_ang2(em->pos, pl->pos) & 0xFFFF) + 0x4000) & 0xFFFF,
                k = em->kind, (u32)(k - 0x1B) < 2 || k == 0x1F)) {
            pl->work7EE = 1;
        }
        em++;
    }
}

u16 calc_vec_ang(f32, f32, f32, f32);

/* near-match: one commutated addu (original: (base+id*252)+off, ours off+(base+id*252)) */
s32 wall_act_ck(PLW *pl, s16 mode) {
    s16 i;
    int off;
    u16 flag;
    u16 ang;
    PL_WALL *w;

    if (!(Pl_stg_ck(pl) & 0xFF)) {
        return 0;
    }
    if (pl->work74C != 0) {
        for (i = 0, off = 0; i < 20; i++, off += 12) {
            w = pl_wall_mat[pl->id];
            w = (PL_WALL *)((u8 *)w + off);
            flag = w->flag;
            if (flag == 0) {
                break;
            }
            ang = calc_vec_ang(w->vec[0], w->vec[2], 0.0f, 0.0f);
            if ((flag & 2) && (pl->work74C & 0xE0000007)) {
                if (mode == 0) {
                    pl->ang[1] = (u16)(ang - 0x4000);
                } else {
                    pl->ang[1] = (u16)(ang + 0x4000);
                }
                pl->ang_y = pl->ang[1];
                return 1;
            }
            if (flag != 0) {
                if (mode == 1) {
                    pl->ang[1] = (u16)(ang + 0x4000);
                    pl->ang_y = pl->ang[1];
                }
                return 2;
            }
        }
    }
    return 0;
}

s32 wall_vec_set(PLW *pl, s16 mode) {
    s16 i;
    int off;
    u16 flag;
    u16 ang;
    PL_WALL *w;

    if (!(Pl_stg_ck(pl) & 0xFF)) {
        return 0;
    }
    if (pl->work74C & (mode == 0 ? 0xE0000007 : 0x3E000)) {
        for (i = 0, off = 0; i < 20; i++, off += 12) {
            w = pl_wall_mat[pl->id];
            w = (PL_WALL *)((u8 *)w + off);
            flag = w->flag;
            if (flag == 0) {
                break;
            }
            ang = calc_vec_ang(w->vec[0], w->vec[2], 0.0f, 0.0f);
            if ((flag & 2) && mode == 0) {
                pl->ang[1] = (u16)(ang - 0x4000);
                pl->ang_y = pl->ang[1];
                return 1;
            }
            if (mode == 1) {
                pl->ang[1] = (u16)(ang + 0x4000);
                pl->ang_y = pl->ang[1];
                return 2;
            }
        }
    }
    return 0;
}

/* near-match (not built): ~592 instructions, 32 extra nops: original fills many branch delay slots with the
   following call's `move a0,s2` (e.g. after Online_ck/bne), ours keeps nop and puts it in the jal slot. Logic believed equal. */
void basic_com_ck(PLW *pl) {
    EMW *var_s0;
    s32 temp_s0;
    s32 var_s1;
    s16 var_a0;
    u16 temp_v1_2;
    u16 var_v0;
    u32 temp_s0_2;
    u32 temp_v1_6;
    u8 temp_v0;
    u8 temp_v1;
    u8 temp_v1_3;
    u8 temp_v1_4;
    u8 temp_v1_5;

    var_s0 = em_work;
    if (Online_ck() == 0) {
        if (Pl_master_ck(pl) == 0) {
            return;
        }
    } else if (Pl_master_ck(pl) == 0) {
        return;
    }
    {
        var_s1 = 0;
        if ((game_w.x0D5 == 4) && (pl->work88D == 0)) {
            Pl_act_set2(pl, 4, 2, 0);
            return;
        }
        pl->work8F0 = 1;
        if (pl->flag604 != 0) {
            for (var_a0 = 0; var_a0 < 0x14; var_a0++, var_s0++) {
                if ((pl->work605 == var_s0->x13) && (var_s0->kind == 7)) {
                    if (var_s0->x87F != 0) {
                        Pl_act_set(pl, 2, 7, 0);
                        return;
                    }
                    break;
                }
            }
            {
                if (((stick_pow_get(pl, 1) & 0xFF) > 0) && (pl->flag15 != 0x12)) {
                    pl->ang_y = stick_dir_set(pl, 0);
                    Pl_act_set(pl, 0, 0x12, 0);
                }
                pl_ride_ck(pl);
                if ((Game_clear_ck(1) != 1) && (pl->sw.trg & 0x20) && (trade_get_ck_00139680(pl) == 0)) {
                    unique_act_set(pl);
                    search_act_set(pl, 0);
                }
            }
        } else {
            switch (game_w.x0D5) {
            case 5:
                /* fallthrough */
            case 6:
                Pl_act_set2(pl, 4, 3, 0);
                return;
            case 8:
                /* fallthrough */
            case 7:
                Pl_act_set2(pl, 4, 5, 0);
                return;
            default:
                if (pl->flag12 != 0) {
                    temp_s0 = stick_pow_get(pl, 1) & 0xFF;
                    switch (temp_s0) {
                    case 4:
                    case 0:
                        break;
                    case 3:
                    case 2:
                    case 1:
                        temp_v1 = pl->flag15;
                        if ((temp_v1 != 0x24) && (temp_v1 != 3) && (pl->x714 == 0)) {
                            Pl_act_set(pl, 0, 3, 0);
                        }
                        break;
                    }
                    if ((temp_s0 != 0) && (pl->x714 != 0) && (pl->flag15 != 0x3B)) {
                        Pl_act_set(pl, 0, 0x3B, 0);
                    }
                    if (Game_clear_ck(1) != 1) {
                        if (pl->work8C4 != 0) {
                            Pl_chat_act_set(pl);
                        }
                        temp_v1_2 = pl->sw.trg;
                        if ((temp_v1_2 & 0x220) || ((temp_v1_2 & 0x10) && (pl->sw.pow[0] >= 0x28))) {
                            if (pl_flag_ck(pl, 0x1200) == 0) {
                                Pl_act_set(pl, 0, 5, 0);
                            } else {
                                Pl_act_set(pl, 0, 0xA, 0);
                            }
                        }
                        job_special_com_ck(pl, 1);
                        if (!(shell_chg_ck(pl) & 0xFF)) {
                            basic_atack_ck(pl);
                        }
                        if (pl->work908 != 0) {
                            pl->work90B = 1;
                            Pl_act_set2(pl, 0, 0x67, 0x10);
                        }
                        if (func_639DD0(pl) & 0xFF) {
                            Pl_act_set2(pl, 2, 0x13, 0);
                            return;
                        }
                    }
                } else {
                    temp_s0_2 = stick_pow_get(pl, 0) & 0xFF;
                    switch (temp_s0_2) {
                    case 0:
                    case 4:
                        break;
                    case 1:
                        temp_v1_3 = pl->flag15;
                        if ((temp_v1_3 != 0x49) && (temp_v1_3 != 0x24) && (pl->x714 == 0)) {
                            Pl_act_set(pl, 0, 0x49, 0);
                            pl->ang_y = stick_dir_set(pl, 0);
                        }
                        break;
                    case 2:
                        temp_v1_4 = pl->flag15;
                        if ((temp_v1_4 != 2) && (temp_v1_4 != 0x24) && (pl->x714 == 0)) {
                            pl->ang_y = stick_dir_set(pl, 0);
                            Pl_act_set(pl, 0, 2, 0);
                        }
                        break;
                    case 3:
                        temp_v1_5 = pl->flag15;
                        if ((temp_v1_5 != 1) && (temp_v1_5 != 0x3F) && (temp_v1_5 != 0x13) && (temp_v1_5 != 0x24) && (temp_v1_5 != 0x1F) && (pl->x714 == 0)) {
                            pl->ang_y = stick_dir_set(pl, 0);
                            temp_v1_6 = (((*(u16 *)&pl->ang_y) + 0x10000) - pl->ang[1]) & 0xFFFF;
                            if ((temp_v1_6 >= 0x6000U) && (temp_v1_6 < 0xA001U)) {
                                Pl_act_set(pl, 0, 0x3F, 0);
                            } else {
                                Pl_act_set(pl, 0, 1, 0);
                            }
                        }
                        break;
                    case 5:
                        if (pl_flag_ck(pl, 0x200) == 0) {
                            if ((pl->work81E != 0) && ((((pl->x3A8 - pl->ang[1]) + 0x2000) & 0xFFFF) >= 0x4001)) {
                                Pl_act_set(pl, 0, 0x20, 0);
                            } else {
                                Pl_act_set(pl, 0, 0x1F, 0);
                            }
                            var_s1 = 1;
                        }
                        break;
                    }
                    if (pl->work8C4 != 0) {
                        Pl_chat_act_set(pl);
                    }
                    if ((temp_s0_2 != 0) && (pl->x714 != 0) && (pl->flag15 != 0x3B)) {
                        Pl_act_set(pl, 0, 0x3B, 0);
                    }
                    if (Game_clear_ck(1) != 1) {
                        if (pl->sw.trg & 0x40) {
                            if (pl->sw.pow[0] >= 0x55) {
                                if (pl->stamina >= 0x4B) {
                                    Pl_act_set(pl, 0, 0x1C, 4);
                                }
                            } else {
                                Pl_act_set(pl, 0, 8, 0);
                            }
                        }
                        if ((pl->sw.trg & 0x200) && (pl->work886 == 0)) {
                            search_act_set(pl, 2);
                            item_action_set(pl, 0);
                            pl->work8F2 = (u8) (pl->work8F2 | 1);
                        }
                        job_special_com_ck(pl, 0);
                        if (!(var_s1 & 0xFF)) {
                            var_v0 = pl->sw.trg;
                        } else {
                            var_v0 = pl->sw.now;
                        }
                        if (var_v0 & 0xFFFF & 0x20) {
                            basic_kabe_ck(pl);
                        }
                        if (pl->sw.an_trg & 0x3C) {
                            if (pl_flag_ck(pl, 0x02000600) == 0) {
                                Pl_act_set(pl, 0, 4, 0);
                            } else {
                                temp_v0 = pl->kind;
                                switch (temp_v0) {
                                default:
                                    Pl_act_set(pl, 1, 3, 4);
                                    break;
                                case 1:
                                    Pl_act_set(pl, 0, 4, 0);
                                    break;
                                case 5:
                                    Pl_act_set(pl, 0, 0x17, 0);
                                    break;
                                case 4:
                                    Pl_act_set(pl, 1, 0x3E, 4);
                                    break;
                                case 3:
                                    Pl_act_set(pl, 1, 0x2E, 4);
                                    break;
                                case 2:
                                    Pl_act_set(pl, 1, 0x20, 4);
                                    break;
                                }
                            }
                        }
                        if (pl->sw.trg & 1) {
                            Pl_act_set(pl, 1, 0x14, 4);
                        }
                        if (pl->sw.trg & 0x20) {
                            if (trade_get_ck_00139680(pl) == 0) {
                                unique_act_set(pl);
                                search_act_set(pl, 0);
                            }
                        } else if (pl->work908 != 0) {
                            pl->work90B = 1;
                            Pl_act_set2(pl, 0, 0x67, 0x10);
                        }
                        if ((em_ninshiki_ck2(pl) & 0xFF) == 1) {
                            Pl_act_set(pl, 0, 0x3E, 0);
                            WyvernFindPlayer(pl);
                        }
                        if (func_639DD0(pl) & 0xFF) {
                            Pl_act_set2(pl, 2, 0x13, 0);
                        }
                    }
                }
                break;
            }
        }
    }
}

/* near-matches gun_adj_sub / sougun_adj_sub (not built). Both only match their register use if the callees
   (blend_set/blend_calc, scope_add) are `static` functions defined earlier in the SAME translation unit:
   MWCC then knows the callee's clobber set and keeps `step` in a3/t0 across the call. Separate plNN.c files
   cannot give that. Remaining known diffs: `v = -v` is constant-folded here (original negates at run time via
   dsll32/dsra32/negu), and `if (kind == 1 || kind == 5)` instead of a switch (gun_adj_sub). */
void gun_adj_sub(PLW *pl) {
    s8 v;
    u16 step;
    u16 now;
    u16 an;
    s16 pw;

    if (Pl_master_ck(pl) != 0) {
        switch (pl->kind) {
        case 5:
        case 1:
            if (pl->char0 == 0x3E9) {
                if (pl->x763 != 0 || pl->pch_on != 0) {
                    pl->x763 = 10;
                }
                blend_set(pl, 0x3ED, 0x3EE);
                if (pl->x763 != 0 || pl->pch_on != 0 || ((now = pl->sw.now, (now & 4) == 0) && (now & 8))) {
                    now = pl->sw.now;
                    if (now & 0x3000) {
                        v = 5;
                        if (pl->pch_on != 0 && game_w.x1DD != 0) {
                            v = -v;
                        }
                        if (now & 0x2000) {
                            blend_calc(pl, v);
                        } else {
                            blend_calc(pl, -v);
                        }
                        pl->x763 = 5;
                    }
                    if (pl->sw.now & 0x800) {
                        pl->x763 = 10;
                        if (pl->pch_on != 0 && game_w.x1DD == 2) {
                            pl->ang_y -= 0x240;
                        } else {
                            pl->ang_y += 0x240;
                        }
                    }
                    if (pl->sw.now & 0x400) {
                        pl->x763 = 10;
                        if (pl->pch_on != 0 && game_w.x1DD == 2) {
                            pl->ang_y += 0x240;
                        } else {
                            pl->ang_y -= 0x240;
                        }
                    }
                    blend_set(pl, 0x3ED, 0x3EE);
                    if (pl->x763 != 0 || pl->pch_on != 0) {
                        an = pl->sw.an_now;
                        if (!(an & 0x3C00) || (pw = pl->sw.pow[0]) < 0x28) {
                            return;
                        }
                        v = 1;
                        step = 0x4C;
                        if (pw >= 0x78) {
                            v = 8;
                            step = 0x200;
                        } else if (pw >= 0x5A) {
                            v = 4;
                            step = 0xC0;
                        } else if (pw >= 0x3C) {
                            v = 2;
                            step = 0x80;
                        }
                        if (an & 0x3000) {
                            if (game_w.x1DD != 0) {
                                v = -v;
                            }
                            if (an & 0x2000) {
                                blend_calc(pl, v);
                            } else {
                                blend_calc(pl, -v);
                            }
                            pl->x763 = 5;
                            blend_set(pl, 0x3ED, 0x3EE);
                        }
                        if (pl->sw.an_now & 0x800) {
                            pl->x763 = 10;
                            if (game_w.x1DD == 2) {
                                pl->ang_y -= step;
                            } else {
                                pl->ang_y += step;
                            }
                        }
                        if (pl->sw.an_now & 0x400) {
                            pl->x763 = 10;
                            if (game_w.x1DD == 2) {
                                pl->ang_y += step;
                            } else {
                                pl->ang_y -= step;
                            }
                        }
                    }
                    blend_set(pl, 0x3ED, 0x3EE);
                }
            }
            break;
        }
    }
}

void sougun_adj_sub(PLW *pl, u16 id) {
    s8 v;
    u16 step;
    u16 now;
    u16 an;
    s16 pw;
    u16 t;
    u16 d;

    if (Pl_master_ck(pl) != 0) {
        if (pl->char0 == id) {
            if (pl->x763 != 0 || pl->pch_on != 0) {
                pl->x763 = 10;
            }
            if (pl->x763 != 0 || pl->pch_on != 0 || ((now = pl->sw.now, (now & 4) == 0) && (now & 8))) {
                now = pl->sw.now;
                if ((now & 0x3000) && id != 0x19E) {
                    v = 4;
                    if (pl->pch_on != 0 && game_w.x1DD != 0) {
                        v = -v;
                    }
                    if (now & 0x2000) {
                        scope_add(pl, (s16)(v << 8));
                    } else {
                        scope_add(pl, (s16)(-v << 8));
                    }
                    pl->x763 = 5;
                }
                if (pl->sw.now & 0x800) {
                    pl->x763 = 10;
                    if (game_w.x1DD == 2) {
                        pl->ang_y -= 0x180;
                    } else {
                        pl->ang_y += 0x180;
                    }
                }
                if (pl->sw.now & 0x400) {
                    pl->x763 = 10;
                    if (game_w.x1DD == 2) {
                        pl->ang_y += 0x180;
                    } else {
                        pl->ang_y -= 0x180;
                    }
                }
                if (pl->x763 != 0 || pl->pch_on != 0) {
                    an = pl->sw.an_now;
                    if ((an & 0x3C00) && (pw = pl->sw.pow[0]) >= 0x28) {
                        v = 1;
                        step = 0x4C;
                        if (pw >= 0x78) {
                            v = 8;
                            step = 0x200;
                        } else if (pw >= 0x5A) {
                            v = 4;
                            step = 0xC0;
                        } else if (pw >= 0x3C) {
                            v = 2;
                            step = 0x80;
                        }
                        if ((an & 0x3000) && id != 0x19E) {
                            if (game_w.x1DD != 0) {
                                v = -v;
                            }
                            if (an & 0x2000) {
                                scope_add(pl, (s16)(v << 8));
                            } else {
                                scope_add(pl, (s16)(-v << 8));
                            }
                            pl->x763 = 5;
                        }
                        if (pl->sw.an_now & 0x800) {
                            pl->x763 = 10;
                            if (game_w.x1DD == 2) {
                                pl->ang_y -= step;
                            } else {
                                pl->ang_y += step;
                            }
                        }
                        if (pl->sw.an_now & 0x400) {
                            pl->x763 = 10;
                            if (game_w.x1DD == 2) {
                                pl->ang_y += step;
                            } else {
                                pl->ang_y -= step;
                            }
                        }
                    }
                }
                if (id == 0x19E) {
                    t = pl->work2D4;
                    d = pl->ang_y - t;
                    if (d < 0x8001 && d >= 0x4001) {
                        pl->ang_y = t + 0x4000;
                    } else if (d >= 0x8000 && d < 0xC000) {
                        pl->ang_y = t - 0x4000;
                    }
                }
            }
        }
    }
}

/* near-match (not built): pl_mv021 - same instructions, but the original keeps x05 in a3 and moves arg1 into s0 later
   (register naming/schedule shift of the prologue; 230/247 lines differ because of it). */
void pl_mv021(PLW *pl, s32 arg1) {
    int sp4C;
    f32 sp40[3];
    f32 sp30[3];
    f32 f;
    u8 s;

    pl->work08++;
    s = pl->x05;
    switch (s) {
    case 0:
        switch (arg1) {
        default:
            pl->x05++;
            pl->x06 = 0;
            pl_chr_set2(pl, 0xB, 2, 0);
            Pl_basic_flagset(pl, 0, 0, 0);
            break;
        case 2:
            pl->pos[1] = pl->pos[1] - 195.0f;
            sp40[2] = -30.0f;
            sp40[0] = 0.0f;
            sp40[1] = 0.0f;
            flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x60));
            pl->pos[0] = pl->pos[0] + sp30[0];
            pl->pos[2] = pl->pos[2] + sp30[2];
        case 1:
            pl->x05 = 2;
            pl->x06 = 0;
            Pl_basic_flagset(pl, 2, 0, 0);
            rate_clear(pl);
            pl->vel[1] = -9.0f;
            pl->acc[1] = -0.72727275f;
            if (arg1 == 1) {
                pl_chr_set2(pl, 0xC, 4, 0x16);
            } else {
                pl_chr_set2(pl, 0xC, 0, 0x16);
            }
            break;
        }
        Pl_view_reset(pl);
        action_timer_calc(pl, 0);
        pl->work08 = 0;
        break;
    case 1:
        if (pl->work194 == 0 || (pl->x06 < 6 && pl->x06 != 0)) {
            pl->x05++;
            rate_clear(pl);
            if (pl->flag12 != 0) {
                f = 0.8f;
            } else {
                f = 1.0f;
            }
            pl->vel[1] = 2.0f * (15.0f * f);
            if (pl->x06 != 0) {
                pl->vel[1] = 50.0f;
            }
            rate_g_calc(pl, 0x14);
            pl_chr_set2(pl, 0xC, 2, 0);
            pl->st = 2;
            if (pl->flag604 != 0) {
                pl->flag604 = 2;
            }
            action_timer_calc(pl, 0);
            pl->work08 = 0;
        }
        break;
    case 2:
        rate_add_g(pl);
        if (pl->vel[1] < 0.0f) {
            if ((arg1 != 2) && (front_land_ck(55.0f, 180.0f, 30.0f, pl, &sp4C) != 0) && (pl->vital > 0)) {
                pl->ang_y = pl->ang[1];
                Pl_act_set2(pl, 0, 0x1B, 0xC);
                break;
            }
            f = pl->x5AC;
            if (!(f < pl->pos[1])) {
                pl->pos[1] = f;
                pl->st = 0;
                pl->flag604 = 0;
                if ((pl->vital <= 0) && (Pl_master_ck(pl) == 1)) {
                    func_639F20(pl);
                    break;
                }
                if (pl->work08 >= 0x1E) {
                    Pl_act_set(pl, 0, 0x26, 0);
                    break;
                }
                pl->x05++;
                pl_chr_set2(pl, 0xD, 0, 0);
            }
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 3, 0);
        }
        break;
    }
}

/* near-match (not built): pl_mv060 - the original computes the s16 `v` (8 or 0xC) in v0 and sign-extends it into s1 only
   across the Pl_basic_flagset call (dsll32 before, dsra32 in the jal delay slot); we constant-fold it (15 lines differ). */
void pl_mv060(PLW *pl) {
    s16 v;
    u8 s;

    pl->work08++;
    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        v = (pl->char0 == 0x25) ? 0xC : 8;
        Pl_basic_flagset(pl, 0x8001, 0, 0);
        pl_chr_set2(pl, 8, v, 0);
        pl->work08 = 0;
        break;
    case 1:
        if (pl->work08 >= 0xC) {
            Pl_act_set(pl, 0, 0x1D, 0);
        }
        break;
    }
}

/* near-matches (not built): pl_at008 (original keeps arg1 in s1/pl in s0 and the idx*6 offset in a3 with three separate lui/addiu bases for
   at008_tbl+0/+2/+4; ours CSEs the base) and pl_at009 (x05 store sits in the blend_set jal delay slot in the original, before the argument setup in ours). */
void pl_at008(PLW *pl, s32 arg1) {
    u8 s;
    s32 o;

    if (pl->x763 != 0) {
        pl->x763 = 5;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        o = arg1 * 6;
        blend_set(pl, *(u16 *)((u8 *)at008_tbl + 2 + o), *(u16 *)((u8 *)at008_tbl + 4 + o));
        pl_chr_set2(pl, *(s16 *)((u8 *)at008_tbl + o), 0, 0);
        break;
    case 1:
        if (frame_check(8.0f, pl, 1) != 0) {
            vib_set_pl(pl, 0);
            Pachinger_set_quake_sub(pl, 1);
            func_62A6C0(pl, 0, 0xE);
            pl->work01C = pl->work01C - 1;
            if (Game_clear_ck(1) == 0) {
                Pl_item_stack(pl, pl->item[pl->work88E].id, -1);
            }
            if (pl->work8BC == 0) {
                pl->work01C = 0;
            }
            pl->work8CE = pl->work8BC;
            pl->work8D2 = pl->work01D;
            pl->work8D1 = pl->work01C;
        }
        if (pl->work194 == 0) {
            blend_set(pl, 0x3ED, 0x3EE);
            pl_to_normal_b(pl, 0, 0, 0);
        }
        break;
    }
}

void pl_at009(PLW *pl) {
    u8 s;

    if (pl->x763 != 0) {
        pl->x763 = 5;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        blend_set(pl, 0x586, 0x587);
        pl_chr_set2(pl, 0x57C, 0, 0);
        pl->work01C = pl->work01D;
        if (pl->work8BC < pl->work01C) {
            pl->work01C = pl->work8BC;
        }
        Pl_basic_flagset(pl, 0, 0, 0);
        switch (Get_string_pow(pl, pl->ammo_type) & 0xFF) {
        case 0:
            pl->chr_spd0 = 2.5f;
            pl->chr_spd1 = 2.5f;
            break;
        default:
        case 1:
            pl->chr_spd0 = 2.0f;
            pl->chr_spd1 = 2.0f;
            break;
        case 2:
            pl->chr_spd0 = 1.5f;
            pl->chr_spd1 = 1.5f;
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal_b(pl, 0, 0, 0);
        }
        break;
    }
}

/* near-match (not built): pl_at012 - calls blend_set (same IPA effect as gun_adj_sub: the original keeps a0 live across blend_set, which
   MWCC only does for a static callee defined earlier in the same TU). Same cause for pl_at008/pl_at009's delay-slot differences. */
void pl_at012(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        blend_set(pl, 0x588, 0x589);
        pl_chr_set2(pl, 0x584, 0, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal_b(pl, 0, 0, 0);
        }
        break;
    }
}

/* near-match (not built): pl_dm003 - the original compares kind with constants kept in v0 and loads the default call's `3` into a2 later;
   ours hoists the constant 3 into a2 for both (121/144 lines differ because of the shifted register use). */
void pl_dm003(PLW *pl, s32 arg1) {
    u8 s;
    u8 k;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            switch (pl->kind) {
            default:
                pl_chr_set2(pl, 0x584, 3, 0);
                break;
            case 4:
                pl_chr_set2(pl, 0x3ED, 2, 0);
                break;
            case 3:
                pl_chr_set2(pl, 0x3ED, 2, 0);
                break;
            }
        } else {
            k = pl->kind;
            if ((k != 3) && (k != 4)) {
                pl_chr_set2(pl, 0x584, 6, 0x28);
            } else {
                pl_chr_set2(pl, 0x3EE, 2, 0);
            }
        }
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x8000);
        pl->flag12 = 1;
        break;
    case 1:
        if (Pl_master_ck(pl) != 1) {
            if (Online_ck() == 0) {
                goto a;
            }
            goto b;
        }
a:
        if ((pl->work39C >= 0x13) && !(pl->sw.now & 0x80)) {
            Pl_act_set2(pl, 2, 8, 0);
            break;
        }
b:
        switch (pl->kind) {
        case 3:
        case 4:
            if ((pl->char0 == 0x3ED) && (pl->work194 == 0)) {
                pl_chr_set2(pl, 0x3EE, 2, 0);
            }
            if (pl->work39C >= 0x13) {
                guard_atk_ck(pl);
            }
            break;
        case 0:
            if (pl->work39C >= 0x13) {
                guard_atk_ck(pl);
            }
            break;
        }
        break;
    }
}

/* near-match (not built): pl_dm008 - same shape problem as pl_dm003: the original's kind compare chain keeps nops in the delay slots. */
void pl_dm008(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        switch (pl->kind) {
        default:
            pl_chr_set2(pl, 0x586, 2, 0);
            break;
        case 4:
        case 3:
            pl_chr_set2(pl, 0x3EF, 2, 0);
            break;
        }
        pl_flag_set(pl, 0x8000);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}

/* pl_demo000 matches since `q = (u8 *)game_w.x28 + (int)i` (linked as pl_demo.c) */
typedef struct { u8 _pad00[0x14]; s32 x14; } PL_QUEST_W_UNUSED;
EMW *pull_enemy_work(void);
void enemy_mv(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void PlComebackCameraRequest(void);
void pl_demo000(PLW *pl) {
    EMW *e;
    u8 *q;
    s16 i;
    s16 k;
    u8 *g;
    u8 s;

    pl->work40E = 0xA;
    pl->work40C = 0xA;
    pl->x01 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        k = -1;
        pl->x06 = 0;
        i = 0;
        g = (u8 *)&game_w;
        do {
            if ((g[0x28] == 0x12) && ((e = pull_enemy_work()) != 0)) {
                q = (u8 *)game_w.x28 + (int)i;
                e->mdl_no = i;
                e->kind = *q;
                e->stg = game_w.stage;
                e->type = 0;
                e->type = 0xFF;
                e->x616 = pl->id;
                enemy_mv(e);
                e->pos[0] = pl->pos[0];
                e->pos[1] = pl->pos[1];
                e->pos[2] = pl->pos[2];
                e->ang[1] = pl->ang[1];
                pl->x824 = e;
                k = *q;
                break;
            }
            i++;
            g++;
        } while (i < 4);
        if (k != 0x12) {
            pl_to_normal(pl, 0, 4, 0);
        } else {
            pl->x07 = 0;
            pl_chr_set2(pl, 0xDD, 0, 0);
        }
        if (Pl_master_ck(pl) == 1) {
            PlComebackCameraRequest();
        }
        break;
    case 1:
        e = pl->x824;
        if (e == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        get_joint_pos_em(e, 0x1D, pl->pos);
        if (e->char0 == 0x3EB) {
            Pl_act_set2(pl, 4, 1, 2);
        }
        pl->ang[1] = e->ang[1];
        pl->ang_y = pl->ang[1];
        break;
    }
}

/* near-match (not built): egg_com_ck - 4 lines differ: the original has an extra `b end` stub after the St_unique_ck switch that some branches jump
   to (ours jumps straight to the epilogue). */
void egg_com_ck(PLW *pl, int arg1) {
    u8 sp3F;
    u16 sp3C;
    f32 sp30[3];
    s32 u;
    u16 v;

    if (Pl_master_ck(pl) != 0) {
        switch (game_w.x0D5) {
        case 6:
        case 5:
            Pl_act_set2(pl, 4, 3, 0);
            break;
        case 7:
        case 8:
            Pl_act_set2(pl, 4, 5, 0);
            break;
        default:
            if ((Pl_hold_item_ck(pl) & 0xFFFF) == 0xFFFF) {
                Pl_act_set2(pl, 5, 3, 0);
                break;
            }
            if (pl->sw.pow[0] >= 0x28) {
                pl->ang_y = stick_dir_set(pl, 0);
                if ((pl->sw.now & 0x10) && (pl->work760 == 0)) {
                    if (act_ck(pl, 5, 2) == 0) {
                        Pl_act_set2(pl, 5, 2, 0);
                    }
                } else if ((act_ck(pl, 5, 1) == 0) && (pl->work760 == 0)) {
                    Pl_act_set2(pl, 5, 1, 0);
                }
            } else if (pl_flag_ck(pl, 0x02001200) != 0) {
                if ((s16)arg1 == 1) {
                    if (pl->work08 == 0) {
                        goto go;
                    }
                } else {
go:
                    Pl_act_set2(pl, 5, 0, 0);
                }
            }
            if (Game_clear_ck(1) != 1) {
                v = pl->sw.trg;
                if (v & 0x40) {
                    if (pl->sw.pow[0] >= 0x55) {
                        if (pl->stamina >= 0x4B) {
                            Pl_act_set2(pl, 0, 0x1C, 4);
                        }
                    } else {
                        Pl_act_set2(pl, 0, 8, 0);
                    }
                    break;
                }
                if (pl->sw.an_trg & 0x3C) {
                    Pl_act_set2(pl, 0, 4, 0);
                    break;
                }
                if (v & 0x200) {
                    Pl_act_set2(pl, 5, 3, 0);
                }
                job_special_com_ck(pl, 2);
                if (pl->sw.trg & 0x20) {
                    u = St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF;
                    switch (u) {
                    case 21:
                        Share_item_conv(pl);
                        if ((Pl_hold_item_ck(pl) & 0xFFFF) == 0xFFFF) {
                            Pl_act_set2(pl, 5, 0xA, 0);
                            return;
                        }
                        break;
                    case 24:
                        if ((Pl_hold_item_ck(pl) & 0xFFFF) == 0xA3) {
                            pl->work56B = pl->work56B & 0xF0;
                            Pl_item_stack(pl, 0xA3, -0xA);
                            pl->ang_y = sp3C;
                            pl->cnt39A = sp3F;
                            Pl_act_set2(pl, 0, 0x37, 0x20);
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
}

/* near-matches (not built): pl_egg03 (the original keeps `mtc1 v0,f0` right after the constant load and fills the bne/jal slots differently, 19 lines)
   and pl_egg05 (20 lines: scheduling of the a0 setup after egg_set; same pattern as pl_at009/012). */
void pl_egg03(PLW *pl, s32 arg1) {
    s32 v;
    s32 id;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 1) {
            pl_chr_set2(pl, 0x37, 4, 0);
        } else {
            pl_chr_set2(pl, 0x36, 4, 0);
        }
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        v = 0x72;
        if (arg1 == 1) {
            v = 4;
        }
        if (frame_check((f32)v, pl, 0) != 0) {
            pl->x05++;
            pl->work56B = pl->work56B & 0xF0;
            if (arg1 != 2) {
                func_549200(pl, 4);
            }
            if ((arg1 == 0) && (Pl_master_ck(pl) == 1)) {
                id = Pl_hold_item_ck(pl) & 0xFFFF;
                if (id != 0xFFFF) {
                    set01_set(1, 0xC, (s16)id);
                    Pl_item_stack(pl, id, -0x64);
                }
            }
            break;
        }
        egg_set(pl);
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}

void pl_egg05(PLW *pl, s32 arg1) {
    f32 sp40[3];
    f32 sp30[3];
    f32 f;
    s32 w;
    u8 s;

    egg_set(pl);
    pl->work08++;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->st = 2;
        if (arg1 != 1) {
            pl_chr_set2(pl, 0x38, 0, 0);
            pl->vel[0] = 0.0f;
            pl->vel[2] = 20.0f;
            pl->vel[1] = 18.0f;
            pl->acc[0] = 0.0f;
            pl->acc[2] = -(pl->vel[2] / 80.0f);
            rate_g_calc(pl, 0x13);
        } else {
            pl_chr_set2(pl, 0x38, 8, 0x20);
            pl->flag604 = 0;
            pl->vel[0] = 0.0f;
            pl->vel[2] = 8.0f;
            pl->vel[1] = -10.0f;
            pl->acc[2] = 0.0f;
            pl->acc[0] = 0.0f;
            pl->acc[1] = -0.72727275f;
        }
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->work08 = 0;
        pl->x07 = 0;
        break;
    case 1:
        pl->vel[0] = pl->vel[0] + pl->acc[0];
        pl->vel[1] = pl->vel[1] + pl->acc[1];
        pl->vel[2] = pl->vel[2] + pl->acc[2];
        if (pl->vel[0] < 0.0f) {
            pl->vel[0] = 0.0f;
        }
        if (pl->vel[2] < 0.0f) {
            pl->vel[2] = 0.0f;
        }
        sp40[0] = pl->vel[0];
        sp40[1] = pl->vel[1];
        sp40[2] = pl->vel[2];
        flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
        pl->pos[0] = pl->pos[0] + sp30[0];
        pl->pos[1] = pl->pos[1] + sp30[1];
        pl->pos[2] = pl->pos[2] + sp30[2];
        if (pl->vel[1] < 0.0f) {
            pl->work08++;
            f = pl->x5AC;
            if (!(f < pl->pos[1])) {
                pl->pos[1] = f;
                pl->st = 0;
                pl->flag604 = 0;
                w = pl->work08;
                if (w >= 0x32) {
                    pl->work56B = pl->work56B & 0xF0;
                    func_549200(pl, 4);
                    Pl_act_set2(pl, 0, 0x26, 0);
                    break;
                }
                if (w >= 0x1E) {
                    Pl_act_set2(pl, 5, 7, 2);
                    break;
                }
                Pl_act_set2(pl, 5, 8, 2);
            }
        }
        break;
    }
}

/* near-match (not built): pl_turn_sub - structure and instructions match, 44 lines differ by register naming (a/b/t/d/v locals land in
   a2/t0/v1/a0 differently); declaration-order search (40 random permutations) did not fix it. */
void pl_turn_sub(PLW *pl) {
    u8 k;
    s32 t;
    u16 v;
    u16 c;
    u32 d;
    s32 b;
    u16 a1;
    s32 a;

    if (pl->x738 == 0) {
        if ((pl->flag12 != 0) && (pl->flag14 == 0)) {
            k = pl->flag15;
            if ((u32)(k - 1) > 1) {
                if (k == 0xC) {
                    goto b71;
                }
                goto other;
            }
b71:
            t = 0x71C;
        } else {
other:
            if ((act_ck(pl, 0, 1) != 0) || (act_ck(pl, 0, 0x24) != 0)) {
                t = 0x71C;
            } else if (act_ck(pl, 0, 0x1F) != 0) {
                t = 0x5B0;
            } else if (act_ck(pl, 0, 0x3F) != 0) {
                t = 0;
            } else {
                t = 0xFA4;
            }
        }
        a = pl->ang[1];
        b = *(u16 *)&pl->ang_y;
        c = pl->char0;
        d = (b - (a & 0xFFFF)) & 0xFFFF;
        switch (c) {
        case 3:
            a1 = 0x80;
            break;
        case 4:
            a1 = 0xA0;
            break;
        default:
            a1 = 0;
            break;
        }
        if ((u32)((d + t) & 0xFFFF) < (u32)(t * 2)) {
            pl->ang[1] = b;
            pl->work2F8 = 0;
            v = pl->work750;
            if (v + 0x300 < 0x601) {
                pl->work750 = 0;
            } else {
                if (v < 0x8000) {
                    pl->work750 = v - 0x300;
                } else {
                    pl->work750 = v + 0x300;
                }
            }
        } else {
            if (d < 0x8000) {
                pl->ang[1] = (a + t) & 0xFFFF;
                pl->work750 = pl->work750 - a1;
            } else {
                pl->ang[1] = (a - t) & 0xFFFF;
                pl->work750 = pl->work750 + a1;
            }
        }
        v = pl->work750;
        if ((v < 0xF601) && (v >= 0xA00)) {
            if (v < 0x8000) {
                pl->work750 = 0xA00;
            } else {
                pl->work750 = 0xF600;
            }
        }
    }
}

/* near-match (not built): pl_horm_sub - 93 lines differ (register naming: original keeps em pointer in s0 and the index in s2, ours the reverse;
   declaration permutations did not fix it). */
void pl_horm_sub(PLW *pl) {
    f32 dmin;
    f32 d;
    u32 best;
    u32 i;
    EMW *e;
    s32 t;
    u16 v;
    u16 ang;

    best = 0xFF;
    pl->work81C = 0;
    dmin = 1500.0f;
    e = em_work;
    if (pl->char0 == 0x26) {
        i = 0;
        do {
            if ((e->be_flag != 0) && (e->x01 != 0)) {
                d = flvecCalcDistance(pl->pos, e->pos);
                if (!(dmin < d)) {
                    best = i;
                    dmin = d;
                    pl->work81C = 1;
                }
            }
            i++;
            e++;
        } while (i < 0x14);
    }
    if (pl->work81C == 0) {
        v = pl->work81A;
        t = v + 0x400;
        if (v != 0) {
            if ((t < 0x801) && (t >= 0)) {
                pl->work81A = 0;
                return;
            }
            if ((s16)v >= 0) {
                pl->work81A = pl->work81A - 0x400;
            } else {
                pl->work81A = pl->work81A + 0x400;
            }
        }
    } else {
        ang = (((calc_vec_ang2(em_work[best].pos, pl->pos) & 0xFFFF) + 0x4000) & 0xFFFF) + 0x10000 - pl->ang[1];
        if (ang >= 0x8000) {
            if (ang < 0xD556) {
                ang = 0xD556;
            }
            v = pl->work81A;
            if (v >= 0x8000) {
                t = v - 0x800;
                if (ang >= t) {
                    pl->work81A = ang;
                    return;
                }
                pl->work81A = t;
                return;
            }
            pl->work81A = v - 0x800;
        } else {
            if (ang >= 0x2AAC) {
                ang = 0x2AAB;
            }
            v = pl->work81A;
            t = v + 0x800;
            if (v < 0x8000) {
                if (t >= ang) {
                    pl->work81A = ang;
                    return;
                }
                pl->work81A = t;
                return;
            }
            pl->work81A = v + 0x800;
        }
    }
}


#include "f_game.h"
extern u8 ot0[], ot1[];
void add_prim(void *, void *, int, int);
/* pl_move_sub (0x14C500, 2144 bytes): near-match, 469/536 insns differ only through delay slots.
 * The original never hoists the `daddu a0,s0,zero` argument copy into a branch delay slot (we do:
 * `bne; daddu a0,s0` vs original `bne; nop; jal; daddu a0,s0`), same effect as timer_calc_sub_pl.
 * Stack layout also differs (original: slide at sp+32, pos[3] at sp+48, gy at sp+60). Tried K&R
 * prototypes, switch form of the work8C2 test: no change. Prototypes below are K&R-style guesses. */
void em_ninshiki_ck();
void pl_timer_calc();
void pl_item_sel();
void pl_shell_sel();
void func_63A260();
u8 pl_status_ck();
void Pl_atck_adj_calc();
void Pl_def_adj_calc();
void pl_dm_value_sub();
void pl_move_sub_sub();
void Pl_pos_adj();
void Pl_status_set();
void hit_stop_calc();
void pl_chr_sub();
void hit_timer_calc();
void HitWallPlayer();
void GetFloorSlide();
void St_unique_adr_set();
int GetGroundHitStatusAreaPl();
void GetPlayerMaterialData();
void pl_light_ck();
f32 Get_dist_to_view();
void Pl_act_set();
void Pl_view_reset();
void pl_move_sub(PLW *pl) {
    int pad[3];
    f32 gy;
    f32 pos[3];
    int slide;
    u8 a, b;

    if (PU8(pl, 0) != 0) {
    em_ninshiki_ck(pl); /* a0 = pl passed through */
    pl_timer_calc(pl);
    pl->work8F2 = 0;
    pl->work90B = 0;
    if (pl->work8C2 == 0) {
        pl_item_sel(pl);
        pl_shell_sel(pl);
    }
    pl->work5A0 = pl->pos[0];
    pl->work5A4 = pl->pos[1];
    pl->work5A8 = pl->pos[2];
    pl->work608 = -1;
    pl->work60A = -1;
    pl->work601 = 0;
    pl->work3F4 = 0;
    pl->work8ED = 0;
    pl->work8F0 = 0;
    pl->work917 = 0;
    if (Pl_master_ck(pl) == 1) {
        if (act_ck(pl, 0, 9) == 0 && act_ck(pl, 0, 0x1B) == 0) pl->work937 = 0;
    } else {
        pl->work937 = 0;
    }
    func_63A260(pl);
    switch (pl_status_ck(pl)) {
    case 1:
        Pl_act_set2(pl, 2, 0x15, 0);
        break;
    case 2:
        if (act_ck(pl, 2, 0x16) == 0 && act_ck(pl, 2, 0x18) == 0) Pl_act_set2(pl, 2, 0x16, 0);
        break;
    }
    Pl_atck_adj_calc(pl);
    Pl_def_adj_calc(pl);
    if (pl->x40A == 0) {
        pl_dm_value_sub(pl);
        pl_move_sub_sub(pl);
        if (pl->work6FF != 0) {
            pl_move_sub_sub(pl);
            pl->work6FF = 0;
        }
        pl_turn_sub(pl);
        pl_horm_sub(pl);
        if (pl->flag14 != 4) Pl_pos_adj(pl);
    }
    Pl_status_set(pl);
    pl->work8C4 = 0;
    pl->work908 = 0;
    hit_stop_calc(pl);
    if (act_ck(pl, 0, 0x27) == 0) pl_chr_sub(pl);
    hit_timer_calc(pl);
    pl->prog->init2(pl);
    if ((act_ck(pl, 0, 0x1B) == 0 || act_ck(pl, 0, 0x1E) == 0) && (Pl_stg_ck(pl) & 0xFF)) HitWallPlayer(pl, 0);
    if (pl->flag14 == 0) {
        if ((u32)(pl->flag15 - 1) < 3U || pl->flag15 == 0x13) {
            if (Pl_stg_ck(pl) & 0xFF) GetFloorSlide(pl, &slide, 1);
        }
    }
    St_unique_adr_set(pl);
    if (GetGroundHitStatusAreaPl(pl, pl->pos, (f32 *)((u8 *)pl + 0x70C), &gy) == 1 && act_ck(pl, 4, 0) == 0) pl->x5AC = gy;
    if (Pl_stg_ck(pl) & 0xFF) GetPlayerMaterialData(pl);
    if (pl->st != 2 && pl->flag604 == 0 && act_ck(pl, 0, 0x3A) == 0 && pl->flag14 != 4 && (Pl_stg_ck(pl) & 0xFF)) {
        f32 fy = pl->x5AC;
        f32 py = pl->pos[1];
        if (py < fy) {
            pl->pos[1] = fy;
            pl->flag604 = 0;
        } else if (py - fy < 30.0f) {
            pl->pos[1] = fy;
            pl->flag604 = 0;
        } else if (pl->vital > 0) {
            if (pl->flag14 != 5) Pl_act_set(pl, 0, 9, 0);
            else Pl_act_set(pl, 5, 6, 0);
        }
    }
    pl->work615 = 1;
    World_calc(pl);
    a = pl->flag14;
    if (a != 2 && a != 3) {
        b = pl->flag12;
        if (b != 0 && (pl->kind == 1 || pl->kind == 5)) {
            if (pl->sw.trg & 0x80) {
                if (a == 0) {
                    u8 f = pl->flag15;
                    if (f != 0x6A && f != 0x69 && f != 0x68 && f != 0x67 && f != 0x1C) pl->pch_on ^= 1;
                } else if (act_ck(pl, 1, 0x3D) == 0) {
                    pl->pch_on ^= 1;
                }
            }
        } else if (b == 0) {
            if (act_ck(pl, 0, 0x36) == 0 && act_ck(pl, 0, 0x48) == 0) {
                if (act_ck(pl, 0, 0x65) != 0 || act_ck(pl, 0, 0x66) != 0) pl->pch_on = 1;
                else Pl_view_reset(pl);
            } else if (pl->sw.trg & 0x80) {
                pl->pch_on ^= 1;
            }
        } else if (pl->kind != 1 && pl->kind != 5) {
            Pl_view_reset(pl);
        }
    }
    if (pl->pch_on == 0) pl->x763 = 0;
    if (pl->stg == game_w.stage) {
        pl_light_ck(pl);
        PF32(pl->work564, 8) = pl->pos[0];
        PF32(pl->work564, 0xC) = pl->pos[1];
        PF32(pl->work564, 0x10) = pl->pos[2];
        if (Pl_master_ck(pl) == 1) {
            pl->work739 = 0;
        } else {
            pos[0] = pl->pos[0];
            pos[1] = pl->pos[1] + 80.0f;
            pos[2] = pl->pos[2];
            if (Get_dist_to_view(pos) <= 200.0f || act_ck(pl, 4, 4) != 0) pl->work739 = 1;
            else pl->work739 = 0;
        }
        if (Pl_master_ck(pl) == 0 && pl_flag_ck(pl, 0x02001600) != 0)
            pl->ang[1] = ((calc_vec_ang2(pl->pos, &pl->work5A0) & 0xFFFF) + 0x4000) & 0xFFFF;
        if (pl->work739 == 0) add_prim(ot1, pl->work564, 0x20, 0);
        else add_prim(ot0, pl->work564, 0x40, 0);
        if (pl->work90E != 0) pl->work90E--;
        if (pl->work90E == 0 && System_timer % 20 == PU8(&game_w, 0xD1)) net_send_pl(pl, 2, 0);
    }
    }
}

#include "flow.h"





s32 pl_flag_ck(PLW *pl, int f) {
    if ((f & 0x80000000) == 0) {
        return pl->act_flag & f;
    } else {
        return pl->work394 & (f & 0x7FFFFFFF);
    }
}



void to_normal(PLW *pl, s32 blend, s16 tm) {
    int e;
    if (pl->flag604 != 0) {
        pl->char0 = 0x18;
        pl->char1 = 0x7C;
    } else if (pl->flag12 != 0) {
        pl->char0 = 0x3E9;
        pl->char1 = 0x44D;
    } else if (pl->work882 < 0x4C) {
        pl->char0 = 0x193;
        pl->char1 = 0x1F7;
    } else {
        e = Stage_env_ck(pl->stg);
        switch (e) {
        default:
            pl->char0 = 1;
            pl->char1 = 0x65;
            break;
        case 1:
            pl->char0 = 0x15;
            pl->char1 = 0x79;
            break;
        case 2:
            pl->char0 = 0x1AF;
            pl->char1 = 0x213;
            break;
        }
    }
    pl->blend0 = blend / 2;
    pl->blend1 = blend / 2;
    pl->act_tm0 = tm;
    pl->act_tm1 = tm;
    pl->flag14 = 0;
    pl->flag15 = 0;
    if (Pl_master_ck(pl) == 1) {
        net_send_pl(pl, 1, 0);
    }
}

extern u8 Equip_Bonus[0x630];
s16 Get_equip_value(u8 kind);
s16 Pl_item_num_ck(PLW *, int);

void Pl_basic_flagset(PLW *pl, int a, int b, int c) {
    u16 w = a;
    switch (a & 0xFF) {
    case 2:
        pl->st = 2;
        break;
    case 1:
        pl->st = 1;
        break;
    default:
        pl->st = 0;
        break;
    }
    if (w & 0x8000) {
        pl_flag_clr(pl, 8);
    } else {
        pl_flag_set(pl, 8);
    }
    pl_flag_clr(pl, 1);
    if ((s16)c == 0) {
        pl_flag_clr(pl, 2);
    } else {
        pl_flag_set(pl, 2);
    }
}

void pad_timer_calc_sub(PLW *pl, int mask);
f32 GetGroundHit(f32 *);
extern u16 for_pad_timer_tbl[4];





void pad_timer_calc_sub(PLW *pl, int mask) {
    u16 *t = &pl->work5B8;
    u16 *tbl = for_pad_timer_tbl;
    if ((u16)mask & *tbl) {
        *t = 0;
    } else if (*t < 0xFFFF) {
        (*t)++;
    }
}


int rate_g_calc(PLW *pl, int t) {
    int n;
    f32 v;
    f32 a;
    n = (s16)t;
    n = (s16)(n / 2);
    v = pl->vel[1];
    a = -1.0f * v;
    if (n < 2 || v < 0.0f) {
        pl->acc[1] = a;
        return 1;
    }
    a /= (f32)n;
    pl->acc[1] = a;
    return 0;
}

f32 *Stage_data_get(int stg);
void flmatGetTrans(f32 *, u8 *);

#include "hit.h"
u8 Get_hit_id(void);
typedef struct W24 { s32 a, b, c, d, e, f; } W24;


void pl_atck_data_set_shl2(HSHL *sh, u16 *hd, int idx) {
    int amask = sh->ailment & 0xF7;
    int aval;
    s32 *src;
    s32 *dst;
    if (amask == 0) {
        aval = 0;
    } else {
        aval = sh->ailment_val;
    }
    aval = (u8)aval;
    src = (s32 *)(**(u8 ***)((u8 *)sh + 0x90) + idx * 0x18);
    dst = (s32 *)&sh->hit_time;
    *(W24 *)dst = *(W24 *)src;
    sh->x08 = hd[6];
    sh->hit_mode = 1;
    sh->x1E = 0;
    sh->hit_chr = 0;
    sh->hit_body = 0;
    if (aval != 0) {
        sh->ailment |= amask;
        sh->ailment_val = aval;
    }
    sh->hit_time >>= 1;
    sh->hit_wait >>= 1;
    if (sh->x75 != 0xFF) {
        sh->x75 >>= 1;
    }
    sh->x76 >>= 1;
    sh->hit_id = Get_hit_id();
}


void atck_data_set_shl2(HSHL *sh, int idx) {
    s32 *src;
    s32 *dst;
    src = (s32 *)(**(u8 ***)((u8 *)sh + 0x90) + idx * 0x18);
    dst = (s32 *)&sh->hit_time;
    *(W24 *)dst = *(W24 *)src;
    sh->x08 = 0xFF;
    sh->hit_mode = 1;
    sh->x1E = 0;
    sh->hit_chr = 0;
    sh->hit_body = 0;
    sh->hit_time >>= 1;
    sh->hit_wait >>= 1;
    if (sh->x75 != 0xFF) {
        sh->x75 >>= 1;
    }
    sh->x76 >>= 1;
    sh->hit_id = Get_hit_id();
}

ST_ITEM *Stage_item_data_get(u8);
ST_UNIQ *Stage_unique_data_get(u8);
u16 *Stage_item_probability_get(int);






long Pl_item_num_ck2(PLW *pl, u16 id) {
    s16 i;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id == id) {
            if (Item_data[id][3] == 0xFF) {
                return 0xFF;
            }
            return (s16)(Item_data[id][3] - pl->item[i].num);
        }
    }
    return Item_data[id][3];
}

long Pl_item_num_ck3(PLW *pl, u16 id) {
    s16 i;
    s16 free = 0;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id == id) {
            if (Item_data[id][3] == 0xFF) {
                if ((pl && pl) && pl) { /* permuter no-op: changes only instruction scheduling */
                }
                return 0xFF;
            }
            return (s16)(Item_data[id][3] - pl->item[i].num);
        }
        if (pl->item[i].id == 0) {
            free++;
        }
    }
    if (free == 0) {
        return -1;
    }
    return Item_data[id][3];
}


s16 Get_Use_itemnum(PLW *pl) {
    s16 i;
    s16 n = 0;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].num > 0 && (s16)pl->item[i].id != 0 && Item_data[(s16)pl->item[i].id][1] == 1) {
            n++;
        }
    }
    return n;
}




int Share_item_stack();

int Pl_item_stack(PLW *pl, int id, int num) {
    u8 *typ = Item_data[(u16)id];
    u8 *mx;
    int ret;
    s16 i;
    s16 j;
    s16 cnt;
    s16 m;
    s16 sv;
    if (typ[0] == 5) {
        return Share_item_stack(pl, id, num);
    }
    mx = &Item_data[(u16)id][3];
    if (*mx == 0xFF) {
        num = (s16)0xFF;
    }
    if (Pl_item_num_ck(pl, id) == 0) {
        ret = 5;
        i = 0;
        do {
            if (pl->item[i].id == 0 && (s16)num > 0) {
                pl->item[i].id = id;
                if ((s16)*mx < (s16)num) {
                    num = (s16)*mx;
                }
                pl->item[i].num = (s16)num;
                if (typ[0] == 4) {
                    sv = pl->work88E;
                    if (i == sv) {
                        pl->work88E = Pl_shell_set(pl, (u16)i, 2);
                        Shell_type_set(pl, 1);
                        pl->work8CE = pl->work8BC;
                        pl->work8D2 = pl->work01D;
                        pl->work8D1 = 0;
                    } else if (pl->kind == 1 || pl->kind == 5) {
                        if (sv == 0xFF) {
                            pl->work88E = Pl_shell_set(pl, 0, 2);
                            Shell_type_set(pl, 0);
                        } else if (Item_data[pl->item[sv].id][0] != 4) {
                            pl->work88E = Pl_shell_set(pl, sv, 0);
                            Shell_type_set(pl, 0);
                        }
                    }
                }
                sv = pl->work888;
                ret = 0;
                if (Item_data[pl->item[sv].id][1] != 1) {
                    pl->work888 = item_sel_sub(pl, sv, 0);
                }
                break;
            }
            i++;
        } while (i < 20);
    } else {
        j = 0;
        do {
            if (pl->item[j].id == (u16)id) {
                cnt = (s16)num;
                m = *mx;
                if (cnt > 0 && pl->item[j].num >= m) {
                    pl->item[j].num = m;
                    ret = 3;
                    goto post;
                }
                if (cnt < 0 && *mx == 0xFF) {
                    ret = 1;
                    break;
                }
                i = j;
                pl->item[i].num += (s16)num;
                if (pl->item[i].num <= 0) {
                    Pl_item_erase(pl, j);
                    ret = 4;
                    pl->item[i].id = 0;
                    pl->item[i].num = 0;
                } else if (m < pl->item[i].num) {
                    pl->item[i].num = m;
                    ret = 2;
                } else {
                    ret = 1;
                }
post:
                if (typ[0] == 4 && i == pl->work88E) {
                    pl->work8BC = pl->item[pl->work88E].num;
                    pl->work8CE = pl->work8BC;
                    if (pl->work8BC < pl->work01C) {
                        pl->work01C = pl->work8BC;
                        pl->work8D1 = pl->work01C;
                    }
                }
                break;
            }
            j++;
        } while (j < 20);
    }
    return ret;
}

int St_pick_ck2(PLW *pl) {
    ST_ITEM *d = Stage_item_data_get(pl->stg);
    f32 dx;
    f32 dz;
    int r;
    u16 rn;
    if (d == 0) {
        return 0xFFFF;
    }
    while (d->pos[0] != -1.0f) {
        if (!(pl->pos[1] < d->pos[1] - 200.0f) && pl->pos[1] < 100.0f + d->pos[1]) {
            dx = pl->pos[0] - d->pos[0];
            dz = pl->pos[2] - d->pos[2];
            if (flSqrt(dx * dx + dz * dz) <= d->r) {
                if (d->num > 0) {
                    r = Item_get_ck(d->id & 0x7FFF);
                    if (r != 0 && r != 0xFFFF && d->num != 0xFF) {
                        rn = ran_suu(1);
                        if (!(rn & 7)) {
                            if (Pl_Skill_ck(pl, 0x2F) == 1) {
                                goto dec;
                            }
                            d->num = 0;
                        } else {
dec:
                            d->num--;
                        }
                    }
                    return r;
                }
                return 0xFFFE;
            }
        }
        d++;
    }
    return 0;
}

#include "flow.h"
#define ANG2RAD(a) (2.0f * (3.1415927f * (((360.0f * (f32)(a)) / 65536.0f) / 360.0f)))
void *get_joint_mat(PLW *, int, int);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
int Ana_ok_ck();
extern s16 *stg_eft_mdl_no[0x58];

void Pl_horm_adj(PLW *pl, int part) {
    void *mat;
    switch ((s16)part) {
    case 9:
        flmatRotY33(mat = get_joint_mat(pl, 9, pl->work81A), 0.3f * ANG2RAD(pl->work81A));
        flmatRotZ33(mat, ANG2RAD(pl->work750));
        break;
    case 10:
        flmatRotY33(get_joint_mat(pl, 10, pl->work81A), 0.3f * ANG2RAD(pl->work81A));
        break;
    case 19:
        flmatRotY33(get_joint_mat(pl, 19, pl->work81A), 0.3f * ANG2RAD(pl->work81A));
        break;
    case 20:
        flmatRotY33(get_joint_mat(pl, 20, pl->work81A), 0.1f * ANG2RAD(pl->work81A));
        break;
    }
}






void Pl_vital_calc_item(PLW *pl, int dv) {
    int n;
    s16 v;
    if (Pl_Skill_ck(pl, 0x1A) == 1 && (n = (s16)dv, n > 0)) {
        v = (s16)(n + n / 4);
    } else {
        v = (s16)dv;
    }
    Pl_vital_calc(pl, v);
}


extern s16 *Pl_slash_tbl[6];
extern char lit_1805_0035B1B0[];
extern char lit_1806_0035B1D0[];
extern char lit_1807_0035B1F0[];

u8 Pl_slash_lv_ck(PLW *pl) {
    s16 *t;
    s16 cur;
    u8 k = pl->kind;
    if (k == 1 || k == 5) {
        return 0;
    }
    cur = pl->work87E;
    t = (s16 *)((u8 *)Pl_slash_tbl[k] + Ken_data[pl->wpn_kind][2] * 8);
    if (t[0] >= cur) {
        return 0;
    }
    if (t[1] >= cur) {
        return 1;
    }
    if (t[2] >= cur) {
        return 2;
    }
    return 3;
}

void Pl_slash_calc(PLW *pl, int dv) {
    u8 up = 0;
    u8 lv;
    if (Pl_master_ck(pl) != 0) {
        if (pl->kind != 1) {
            if (pl->kind == 5) {
            } else {
                pl->work87E = pl->work87E + dv;
                if (pl->work87E <= 0) {
                    pl->work87E = 0;
                }
                if (pl->work884 < pl->work87E) {
                    pl->work87E = pl->work884;
                    up = 1;
                }
                lv = Pl_slash_lv_ck(pl);
                if (lv != pl->work887) {
                    if (lv < pl->work887) {
                        set01_set2(lit_1805_0035B1B0);
                    } else {
                        set01_set2(lit_1806_0035B1D0);
                    }
                    pl->work887 = lv;
                }
                if (up != 0) {
                    set01_set2(lit_1807_0035B1F0);
                }
            }
        }
    }
}


u16 Pl_shell_set(PLW *pl, int slot, int dir) {
    s16 n;
    u16 s;
    u8 d;
    if (pl->kind != 1 && pl->kind != 5) {
        return 0xFF;
    }
    if (pl->work35F != 7) {
        return 0xFF;
    }
    s = slot;
    if (s >= 20) {
        s = 0;
    }
    d = dir;
    switch (d) {
    case 0:
        s = (s + 1) % 20;
        break;
    case 1:
        if (s == 0) {
            s = 19;
        } else {
            s = s - 1;
        }
        break;
    case 3:
    case 2:
        break;
    }
    for (n = 0; n < 20; n++) {
        if (pl->item[s].id != 0 && pl->item[s].num > 0 && Item_data[pl->item[s].id][1] == 2
            && (*(s32 *)&Gun_data[pl->wpn_kind][0x10] & (1 << *(s16 *)&Item_data[pl->item[s].id][8]))) {
            return s;
        }
        switch (d) {
        case 2:
        case 0:
            s = (s + 1) % 20;
            break;
        case 3:
        case 1:
            if (s == 0) {
                s = 19;
            } else {
                s = s - 1;
            }
            break;
        }
    }
    return 0xFF;
}

extern u8 chat_act_tbl_002F1860[0xD];
void Pl_se_req2(PLW *, int, int, f32 *, int, int);

#include "flow.h"

s32 Pl_hold_item_ck(PLW *pl) {
    s16 i;
    u16 r = 0xFFFF;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].num != 0) {
            switch (pl->item[i].id) {
            case 0x91:
            case 0x92:
            case 0xA3:
            case 0x94:
            case 0x93:
            case 0x95:
                r = pl->item[i].id;
                break;
            default:
                break;
            }
        }
    }
    return r;
}

typedef struct TUTO_REC {
    u16 id;     /* 0x00 */
    u16 _02;
    f32 x;      /* 0x04 */
    f32 y;      /* 0x08 */
    f32 z;      /* 0x0C */
    f32 r;      /* 0x10 */
} TUTO_REC;
s16 Pl_item_num_ck(PLW *, int);
void adx_se_set(PLW *, int);
void init_set_work();
void init_eft_work();
void init_shell_work();
void init_item_work();
void clr_set_work();
void clr_eft_work();
void clr_shell_work();
void clr_item_work();
void clr_used_heap(int, int);

s32 Pl_scope_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x40) != 0;
}

s32 Pl_silencer_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x10) != 0;
}

s32 Pl_barrel_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x20) != 0;
}





s32 Sansai_talk_ck(PLW *pl) {
    EMW *e = em_work;
    s16 i;
    for (i = 0; i < 20; i++, e++) {
        if (e->kind == 0xA && flvecCalcDistance(pl->pos, e->pos) <= 300.0f) {
            return 1;
        }
    }
    return 0;
}

#include "flow.h"
void Pl_vital_calc_item(PLW *, int);
void Pl_max_vital_calc(PLW *, int);
void func_639DF0(PLW *, int);
void set01_set(int, int, int);

#include "flow.h"
void flmatGetTrans(f32 *, u8 *);
void RotMatVec(f32 *, f32 *, int);
f32 flSqrt(f32);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);


void pl_light_ck(PLW *pl) {
    int i;
    EMW *e;
    f32 d;
    f32 dx;
    f32 dz;
    f32 lim;
    f32 hlim;
    if (pl->flag604 != 0 || !(pl->pos[1] - pl->x5AC <= 100.0f)) {
        pl->work613 = 0;
        return;
    }
    e = em_work;
    for (i = 0; i < 20; i++, e++) {
        if (e->be_flag != 0 && e->x01 != 0 && PU8(e, 0x612) >= 2) {
            dx = pl->pos[0] - e->pos[0];
            dz = pl->pos[2] - e->pos[2];
            d = flSqrt(dx * dx + dz * dz);
            if (PU8(e, 0x612) == 2) {
                lim = 300.0f;
                hlim = 300.0f;
            } else {
                lim = 500.0f;
                hlim = 300.0f;
            }
            if (d <= lim && pl->pos[1] - e->pos[2] < hlim) {
                pl->work613 = 1;
                return;
            }
        }
    }
    pl->work613 = 0;
}

#include "flow.h"
void pl_body_make(PLW *pl, f32 *out, f32 radius);
void hit_cap_pk(void *, void *);
int hit_cap_cap3_m(void *, void *, f32 *);


extern f32 *D_63FC50[];
extern f32 *D_6103A0[];
void flvecNormalize(f32 *);
int hit_sphr_sphr3(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb);

/* Pushes two monsters apart (sphere lists from the per-kind push tables). The inner list pointer is
 * never rewound for the 2nd and later spheres of the first monster: faithful to the original. */
void body_hit_sub_em(PLW *pl, PLW *o) {
    f32 pa[3];
    f32 pb[3];
    f32 len[2];
    f32 v[2][3];
    f32 *s4;
    f32 *s3;
    s16 n = 0;
    s16 k;
    int cnt;
    f32 ra;
    f32 *pv;
    f32 *pl_len;
    if (game_w.x1DC == 0) {
        s3 = D_63FC50[o->kind];
        s4 = D_63FC50[pl->kind];
    } else {
        s3 = D_6103A0[o->kind];
        s4 = D_6103A0[pl->kind];
    }
    pv = v[0];
    pl_len = len;
    while (s4[3] != -1.0f) {
        pa[0] = pl->pos[0] + s4[0] * pl->scl[0];
        pa[1] = pl->pos[1] + s4[1] * pl->scl[1];
        pa[2] = pl->pos[2] + s4[2] * pl->scl[2];
        ra = s4[3] * pl->scl[0];
        if (s3[3] != -1.0f) {
            do {
                pb[0] = o->pos[0] + s3[0] * pl->scl[0];
                pb[1] = o->pos[1] + s3[1] * pl->scl[1];
                pb[2] = o->pos[2] + s3[2] * pl->scl[2];
                if (hit_sphr_sphr3(pa, pb, pv, ra, s3[3] * o->scl[0]) != 0) {
                    *pl_len = flvecCalcLength(pv);
                    pv += 3;
                    pl_len++;
                    n++;
                }
                if (n >= 2) {
                    goto done;
                }
                s3 += 4;
            } while (s3[3] != -1.0f);
        }
        s4 += 4;
    }
done:
    cnt = n;
    if (cnt != 0) {
        for (k = 1; k < cnt; k++) {
            v[0][0] += v[k][0];
            v[0][1] += v[k][1];
            v[0][2] += v[k][2];
            if (len[0] < len[k]) {
                len[0] = len[k];
            }
        }
        if (pl->st != 2) {
            v[0][1] = 0;
        }
        flvecNormalize(v[0]);
        v[0][0] *= len[0];
        v[0][1] *= len[0];
        v[0][2] *= len[0];
        if (pl->work612 == o->work612) {
            pl->pos[0] += 0.5f * v[0][0];
            if (pl->st == 2) {
                pl->pos[1] += 0.5f * v[0][1];
            }
            pl->pos[2] += 0.5f * v[0][2];
            o->pos[0] += -0.5f * v[0][0];
            if (o->st == 2) {
                o->pos[1] += -0.5f * v[0][1];
            }
            o->pos[2] += -0.5f * v[0][2];
            pl->work7EC = 1;
            o->work7EC = 1;
        } else if (pl->work612 < o->work612) {
            pl->pos[0] += v[0][0];
            if (pl->st == 2) {
                pl->pos[1] += v[0][1];
            }
            pl->pos[2] += v[0][2];
            pl->work7EC = 1;
        } else {
            o->pos[0] -= v[0][0];
            if (o->st == 2) {
                o->pos[1] -= v[0][1];
            }
            o->pos[2] -= v[0][2];
            o->work7EC = 1;
        }
    }
}

extern s16 *D_63FA10[];
extern s16 *D_610370[];
void body_ptr_ck2(PLW *, s16 **);
int hit_data_expand(PLW *, s16 *, f32 *, f32 *);
int hit_data_expand2(f32 *, s16 *, f32 *, f32 *);
int hit_cap_sphr_m(void *, f32 *, f32 *, f32);

/* Pushes the player against one monster's body list (capsule/sphere hit data, 0x28 bytes per entry). */
void body_hit_sub_new(PLW *pl, PLW *o) {
    s16 *rec;
    f32 len[2];
    u8 pkp[0x40];
    u8 pkb[0x40];
    f32 body[8];
    f32 cap[8];
    f32 sph[4];
    f32 v[2][3];
    f32 *pv;
    f32 *plen;
    f32 *prad;
    s16 n = 0;
    int cnt;
    s16 k;
    int r;
    pl_body_make(pl, body, 40.0f);
    hit_cap_pk(body, pkp);
    if (game_w.x1DC == 0) {
        rec = D_63FA10[o->kind];
    } else {
        rec = D_610370[o->kind];
    }
    pv = v[0];
    plen = len;
    prad = &sph[3];
    goto test;
    for (;;) {
        switch (*rec) {
        case 126:
        case 127:
            r = (s16)hit_data_expand2(o->pos, rec, cap, sph);
            goto got;
        case 125:
            body_ptr_ck2(o, &rec);
            goto test;
        default:
            r = (s16)hit_data_expand(o, rec, cap, sph);
got:
            if (r == 0) {
                r = (u8)hit_cap_sphr_m(pkp, sph, pv, *prad);
            } else {
                hit_cap_pk(cap, pkb);
                r = (u8)hit_cap_cap3_m(pkb, pkp, pv);
            }
            if ((u8)r != 0) {
                *plen = flvecCalcLength(pv);
                if (pl->st != 2) {
                    pv[1] = 0;
                }
                n++;
                pv += 3;
                plen++;
            }
            if (n >= 2) {
                goto done;
            }
            rec += 0x14;
test:
            if (*rec == -1) {
                goto done;
            }
            break;
        }
    }
done:
    cnt = n;
    if (cnt != 0) {
        for (k = 1; k < cnt; k++) {
            v[0][0] += v[k][0];
            v[0][1] += v[k][1];
            v[0][2] += v[k][2];
            if (len[0] < len[k]) {
                len[0] = len[k];
            }
        }
        flvecNormalize(v[0]);
        v[0][0] *= len[0];
        v[0][1] *= len[0];
        v[0][2] *= len[0];
        if (o->work612 <= 0) {
            pl->pos[0] += 0.5f * v[0][0];
            pl->pos[1] += 0.5f * v[0][1];
            pl->pos[2] += 0.5f * v[0][2];
            o->pos[0] += -0.5f * v[0][0];
            o->pos[1] += -0.5f * v[0][1];
            o->pos[2] += -0.5f * v[0][2];
            pl->work7EC = 1;
            o->work7EC = 1;
        } else {
            pl->pos[0] += v[0][0];
            pl->pos[1] += v[0][1];
            pl->pos[2] += v[0][2];
            pl->work7EC = 1;
        }
    }
}

s32 Pl_stg_ck_tw(PLW *, PLW *);

/* Per-frame body pushing: players vs players (when softdip 0xA9), players vs monsters, monsters vs monsters. */
void body_hit(void) {
    f32 d[3];
    int i;
    int j;
    int k;
    PLW *pl;
    PLW *p2;
    PLW *e;
    PLW *f;
    int busy;
    for (i = 0; i < 4; i++) {
        player_work[i].work7EC = 0;
    }
    for (i = 0; i < 20; i++) {
        ((PLW *)&em_work[i])->work7EC = 0;
    }
    for (i = 0; i < 4; i++) {
        pl = &player_work[i];
        if (pl->be_flag != 0 && pl->x01 != 0 && Pl_master_ck(pl) != 0 && pl->work40E == 0 && (u8)Pl_stg_ck(pl)) {
            if (softdip_ck(0xA9) != 0) {
                for (j = 0; j < 4; j++) {
                    p2 = &player_work[j];
                    if (p2->be_flag != 0 && p2->x01 != 0 && p2->work40E == 0 && (u8)Pl_stg_ck_tw(pl, p2)) {
                        body_hit_sub_pl(pl, p2);
                    }
                }
            }
            for (j = 0; j < 20; j++) {
                e = (PLW *)&em_work[j];
                if (e->be_flag != 0 && e->x01 != 0 && e->work40E == 0 && (u8)Pl_stg_ck_tw(pl, e)) {
                    body_hit_sub_new(pl, e);
                    d[0] = pl->pos[0] - e->pos[0];
                    d[1] = pl->pos[1] - e->pos[1];
                    d[2] = pl->pos[2] - e->pos[2];
                    PF32(pl, 0x3AC) = flvecCalcLength(d);
                }
            }
        }
    }
    for (i = 0; i < 19; i++) {
        e = (PLW *)&em_work[i];
        if (e->be_flag != 0 && e->x01 != 0 && e->work40E == 0 && (u8)Pl_stg_ck(e)) {
            busy = PU16(e, 0x7EA) != 0;
            for (k = i + 1; k < 20; k++) {
                f = (PLW *)&em_work[k];
                if (f->be_flag != 0 && f->x01 != 0) {
                    if (!(e->kind != 0x1D && f->kind != 0x1D) || (PU8(e, 0x8C3) == 0 && PU8(f, 0x8C3) == 0)) {
                        if (f->work40E == 0 && (u8)Pl_stg_ck_tw(e, f)) {
                            if ((busy == 0 && PU16(f, 0x7EA) == 0) || e->kind == 0x1D || f->kind == 0x1D) {
                                body_hit_sub_em(e, f);
                            }
                        }
                    }
                }
            }
        }
    }
}


#include "flow.h"
extern u16 Psw[];
void Item_box_get_efct();
void Item_box_get_item(u16, u8);
void net_send_host(int, u8);
s16 Pl_item_num_ck(PLW *, int);

/* Item box (village storage): game_w+0x128 holds {u16 item, s16 count} per slot (32 slots), game_w+0x1A8 a taken-bitmask. */
void box_get(PLW *pl) {
    pl->work8F3 = 0x1E;
    Item_box_get_efct();
    if (Online_ck() == 1) {
        pl->work932 = 0x384;
        pl->work91F = 1;
        net_send_host(1, game_w.master);
        return;
    }
    pl->work932 = 0;
    pl->work91F = 0;
    Pl_item_stack(pl, PU16(&game_w, 0x128 + pl->work8C3 * 4), PS16(&game_w, 0x12A + pl->work8C3 * 4));
    PU32(&game_w, 0x1A8 + (pl->work8C3 >> 5) * 4) |= 1 << (pl->work8C3 % 32);
    Item_box_get_item(PU16(&game_w, 0x128 + pl->work8C3 * 4), pl->work8C3);
}

void Pl_box_select(PLW *pl) {
    s16 trg;
    s16 pad;
    int a;
    u8 idx;
    int bit;
    if (Pl_master_ck(pl) != 0) {
        trg = Psw[2];
        pad = Psw[2] | Psw[12];
        if (pl->work8F3 != 0) {
            pl->work8F3--;
        }
        if (pl->work932 != 0) {
            pl->work932--;
            if (pl->work932 == 0) {
                pl->work91F = 0;
            }
        }
        if (pl->work91F != 0 || pl->work8F3 != 0) {
            return;
        }
        a = pad;
        if (a & 0x40) {
            se_req(7, 0x14, 0);
            pl->work8C2 = 0;
            return;
        }
        if (a & 0x800) {
            se_req(7, 0x16, 0);
            if (pl->work8C3 & 7) {
                pl->work8C3 = pl->work8C3 - 1;
            } else {
                pl->work8C3 = pl->work8C3 + 7;
            }
        }
        if (a & 0x400) {
            se_req(7, 0x16, 0);
            if ((pl->work8C3 & 7) != 7) {
                pl->work8C3 = pl->work8C3 + 1;
            } else {
                pl->work8C3 = pl->work8C3 - 7;
            }
        }
        if (a & 0x2000) {
            se_req(7, 0x16, 0);
            if (pl->work8C3 < 8) {
                pl->work8C3 = pl->work8C3 + 0x18;
            } else {
                pl->work8C3 = pl->work8C3 - 8;
            }
        }
        if (a & 0x1000) {
            se_req(7, 0x16, 0);
            if (pl->work8C3 >= 0x18) {
                pl->work8C3 = pl->work8C3 - 0x18;
            } else {
                pl->work8C3 = pl->work8C3 + 8;
            }
        }
        idx = pl->work8C3;
        bit = 1 << (idx % 32);
        if (!(PU32(&game_w, 0x1A8 + (idx >> 5) * 4) & bit) && (trg & 0x20) && PU16(&game_w, 0x128 + idx * 4) != 0 && pl->work8F3 == 0) {
            if (Pl_item_num_ck(pl, PU16(&game_w, 0x128 + idx * 4)) == 0) {
                if (Pl_item_search_space(pl) != 0) {
                    se_req(7, 0x19, 0);
                    box_get(pl);
                }
            } else if (Pl_item_num_ck2(pl, PU16(&game_w, 0x128 + pl->work8C3 * 4)) >= PS16(&game_w, 0x12A + pl->work8C3 * 4)) {
                se_req(7, 0x19, 0);
                box_get(pl);
            }
        }
    }
}
