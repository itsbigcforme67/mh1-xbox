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

int stick_pow_get(PLW *pl) {
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
