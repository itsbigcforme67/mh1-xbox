/* Player code (f_pl.s, 0x134950..): working file; matched functions are moved
 * to plX.c, what is left here is near-match (not built). */
#include "pl.h"
#include "plf.h"
#include "game.h"

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
            w = (PL_WALL *)(off + (u8 *)pl_wall_mat[pl->id]);
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
            w = (PL_WALL *)(off + (u8 *)pl_wall_mat[pl->id]);
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
