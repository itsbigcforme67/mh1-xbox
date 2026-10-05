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

/* near-match (not built): pl_demo000 - 1 instruction differs (`addu s0,v1,a0` vs ours `addu s0,a0,v1`: operand order of base+index for the game_w.x28 slot pointer q). */
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
                q = (u8 *)(i + (int)game_w.x28);
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
                pl->em_demo = e;
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
        e = pl->em_demo;
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
    em_ninshiki_ck();
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
