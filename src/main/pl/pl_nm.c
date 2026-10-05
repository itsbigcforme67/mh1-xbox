/* Player code (f_pl.s, 0x134950..): working file; matched functions are moved
 * to plX.c, what is left here is near-match (not built). */
#include "pl.h"
#include "plf.h"
#include "game.h"

void Pl_item_charge(PLW *pl) {
    s16 i;
    s16 *p;
    s16 n;
    u8 c = pl->work8BF;
    if (c == 0) return;
    pl->work8BF = c - 1;
    for (i = 0; i < 20; i++) {
        pl->item[i].id = 0;
        pl->item[i].num = 0;
    }
    p = pl_supp_tbl[pl->work8BF + pl->kind * 6];
    n = *p;
    while (n != -1) {
        u16 a = p[1];
        p += 2;
        Pl_item_supply(pl, 0, a, n);
        n = *p;
    }
}

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

void pl_init_sub(PLW *pl) {
    s32 temp_a2;
    u16 temp_a1;
    u16 temp_a2_2;
    u16 var_v0;
    u8 temp_a1_2;
    u8 temp_s1;
    u8 temp_v0;
    u8 temp_v1;

    PS32(pl, 4) = 0;
    pl->work2F4 = 0;
    pl->work2FC = 0;
    pl->scl[0] = 1.0f;
    pl->scl[1] = 1.0f;
    pl->scl[2] = 1.0f;
    temp_v1 = pl->x738;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        pl->pos[0] = stage_start_pos[game_w.stage][0];
        pl->pos[1] = stage_start_pos[game_w.stage][1];
        pl->pos[2] = stage_start_pos[game_w.stage][2];
        pl->ang[1] = stage_start_ang[game_w.stage];
        pl->ang[0] = 0;
        pl->ang[2] = 0;
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        Pl_ofs_set(pl, ((u8 *)pl + 0xAC), (u16) pl->ang[1]);
        pl->work792 = 0x64;
        pl->work882 = 0x12C;
        pl->work6A4 = 0;
        pl->work6A8 = 0;
        pl->work6A5 = 0;
        pl->work6A9 = 0;
        pl->work6A6 = 0;
        pl->work6AA = 0;
        if (Pl_master_ck(pl) == 1) {
            pl->work792 += game_w.x215;
            pl->work792 = pl->work792 + skill_hp_calc_00134FF0(pl);
            if (pl->work792 > 0x96) {
                pl->work792 = 0x96;
            }
            Pl_max_stamina_calc(pl, game_w.x216);
            pl->work6A4 = game_w.x213;
            pl->work6A8 = game_w.x214;
        }
        pl->vital = (s16) pl->work792;
        pl->vital_red = (s16) pl->work792;
        pl->work8C0 = 0x2A30;
        pl->stamina = (s16) pl->work882;
        temp_a1 = pl->wpn_kind;
        pl->work884 = (s16) ((Ken_data[temp_a1][3] * 0x32) + 0x96);
        pl->work87E = (s16) pl->work884;
        pl->work887 = Pl_slash_lv_ck(pl, temp_a1);
        pl->work8CC = 0;
        pl->work918 = 0;
        pl->work91A = 0;
        pl->work930 = 0;
        pl->work91C = 0;
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->x7BA = 0;
        pl->x7B2 = 0;
        pl->x7C4 = 0;
        pl->work934 = 0;
        pl->work88D = 0;
        pl->atk_rate = 1.0f;
        pl->work7DC = 1.0f;
        Pl_item_idx_calc(pl);
        pl->work91E = 0;
        pl->work7ED = 0;
        pl->work8C7 = 0;
        pl->flag12 = 0;
        pl->work01C = 0;
        pl->work01D = 0;
        pl->work56F = 0;
        parts_init(pl);
        if (game_w.pl_ent[pl->id] == 1 || Pl_master_ck(pl) == 1) {
            pl->flag14 = 0;
            pl->flag15 = 0;
        } else {
            Pl_act_set(pl, 4, 5, 2);
        }
        pl->work90E = 0;
        pl->work88A = 0;
        pl->work4D4 = 1;
        pl->work91F = 0;
        temp_v0 = PU8(pl, 2);
        switch (temp_v0) {                          /* switch 2 */
        case 0:                                     /* switch 2 */
        case 4:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0;
            break;
        case 2:                                     /* switch 2 */
            pl->work7D4 = 0;
            pl->work7D5 = 0x64;
            break;
        case 3:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0x64;
            break;
        case 1:                                     /* switch 2 */
        case 5:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0x64;
            break;
        default:                                    /* switch 2 */
            pl->work7D4 = 0x32;
            pl->work7D5 = 0x32;
            break;
        }
        Pl_item_charge(pl);
        pl->work88E = Pl_shell_set(pl, 0, 2);
        pl->work8BC = pl->item[pl->work88E].num;
        Shell_type_set(pl, 0);
        normal_char_set(pl, 0, 0);
        break;
    case 1:                                         /* switch 1 */
        pl->pos[0] = pl->work73C;
        pl->pos[1] = pl->work740;
        pl->pos[2] = pl->work744;
        pl->ang[1] = pl->work570;
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        pl->x738 = 0U;
        pl->work56F = 0;
        if ((game_w.pl_ent[pl->id] == 1) || (Pl_master_ck(pl) == 1)) {
            pl->flag14 = 0;
            pl->flag15 = 0;
        } else {
            Pl_act_set(pl, 4, 5, 2);
        }
        pl->work90E = 0;
        temp_s1 = pl->work56B;
        if (temp_s1 & 0xF) {
            pl_to_normal_clr(pl);
            pl->work56B = (u8)(temp_s1 & 0xF0) | 5;
            Pl_act_set(pl, 5, 0, 0);
        } else {
            pl_to_normal_clr(pl);
            normal_char_set(pl, 0, 0);
        }
        break;
    case 2:                                         /* switch 1 */
        switch (game_w.stage) {   /* switch 3; irregular */
        case 0x4:                                   /* switch 3 */
            pl->pos[0] = 9150.0f;
            pl->pos[1] = 7.0f;
            pl->pos[2] = 6200.0f;
            pl->ang[1] = 0x2AAB;
            break;
        case 0xA:                                   /* switch 3 */
            pl->pos[0] = 6070.0f;
            pl->pos[1] = 17.0f;
            pl->pos[2] = 6990.0f;
            pl->ang[1] = 0xB778;
            break;
        case 0x14:                                  /* switch 3 */
            pl->pos[0] = 5910.0f;
            pl->pos[1] = 11.0f;
            pl->pos[2] = 5330.0f;
            pl->ang[1] = 0xF778;
            break;
        case 0x15:                                  /* switch 3 */
            pl->pos[0] = 11900.0f;
            pl->pos[1] = 40.0f;
            pl->pos[2] = 10100.0f;
            pl->ang[1] = 0xC001;
            break;
        case 0x32:                                  /* switch 3 */
            pl->pos[0] = 10650.0f;
            pl->pos[1] = 500.0f;
            pl->pos[2] = 5220.0f;
            pl->ang[1] = 0xF778;
            break;
        case 0x3D:                                  /* switch 3 */
            pl->pos[0] = 5180.0f;
            pl->pos[1] = 0;
            pl->pos[2] = 4900.0f;
            pl->ang[1] = 0xE38F;
            break;
        case 0x43:                                  /* switch 3 */
            pl->pos[0] = 8137.0f;
            pl->pos[1] = 0;
            pl->pos[2] = 11785.0f;
            pl->ang[1] = 0x6000;
            break;
        default:                                    /* switch 3 */
            pl->pos[0] = pl->work73C;
            pl->pos[1] = pl->work740;
            pl->pos[2] = pl->work744;
            pl->ang[1] = pl->work570;
            break;
        }
        Pl_ofs_set(pl, ((u8 *)pl + 0xAC), (u16) pl->ang[1]);
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        pl->x5AC = pl->pos[1];
        pl->ang_y = (s16) pl->ang[1];
        temp_a2_2 = pl->wpn_kind;
        temp_a1_2 = Ken_data[temp_a2_2][3];
        pl->work884 = (s16) ((temp_a1_2 * 0x32) + 0x96);
        pl->work792 = 0x64;
        if (Pl_master_ck(pl) == 1) {
            pl->work792 = pl->work792 + skill_hp_calc_00134FF0(pl);
            if (pl->work792 > 0x96) {
                pl->work792 = 0x96;
            }
        }
        pl->vital = (s16) pl->work792;
        pl->vital_red = (s16) pl->work792;
        pl->work882 = 0x12C;
        pl->work8C0 = 0x2A30;
        pl->stamina = (s16) pl->work882;
        pl->work8CC = 0;
        pl->work918 = 0;
        pl->work91A = 0;
        pl->work91C = 0;
        pl->work930 = 0;
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->x7BA = 0;
        pl->x7B2 = 0;
        pl->x7C4 = 0;
        pl->work934 = 0;
        pl->x738 = 0U;
        pl->work56F = 0x14;
        PS8(pl, 1) = 0;
        pl->work6A4 = 0;
        pl->work6A8 = 0;
        pl->work6A5 = 0;
        pl->work6A9 = 0;
        pl->work6A6 = 0;
        pl->work6AA = 0;
        pl->work7ED = 0;
        Pl_act_set(pl, 4, 0, 0xC);
        break;
    }
    pl->work81C = 0;
    pl->ang_y = (s16) pl->ang[1];
    pl->work750 = 0;
    pl->work81A = 0;
    pl->work74A = 0;
    (*(void * *)((u8 *)((u8 *)pl + 0x41C))) = (void *) (((u8 *)pl + 0x3D0));
    pl->st = 0;
    pl->work56E = 0;
    pl->work3A0 = 1.0f;
    pl->work4E3 = 0;
    pl->flag604 = 0;
    pl->work608 = -1;
    pl->work60A = -1;
    pl->work3F4 = 0;
    pl->work56A = 0xFF;
    pl->work798 = 1.0f;
    pl->work8C5 = 1;
    pl->work8C2 = 0;
    pl->work8F3 = 0;
    pl->work8C3 = 0;
    pl->work8C4 = 0;
    pl->work908 = 0;
    pl->work917 = 0;
    pl->work8C6 = 0;
    pl->work936 = 0;
    pl->x43E = 0;
    pl->work39C = 0;
    pl->work40C = 0x1E;
    pl->chr_spd0 = 2.0f;
    pl->chr_spd1 = 2.0f;
    pl->work8C8 = 0;
    pl->work8ED = 0;
    pl->work8F0 = 0;
    pl->work8F2 = 0;
    pl->work88C = 0;
    pl->work612 = 0;
    pl->work615 = 0;
    pl->work760 = 0;
    pl->work886 = 0;
    pl->work8BE = 0;
    pl->work7D6 = 0;
    Pl_view_reset(pl, 1, 0xFF, -1);
    pl->work800 = 0;
    pl->work804 = 0;
    pl->work808 = 0;
    flvecCopy((f32 *)((u8 *)pl + 0x80C), (f32 *)((u8 *)pl + 0x800));
    pl->work818 = 0;
    pl->work720[0] = 0;
    pl->work724[0] = 0;
    (*(s16 *)((u8 *)pl + 0x72C)) = 0;
    pl->work720[1] = 0;
    pl->work724[1] = 0;
    (*(s16 *)((u8 *)pl + 0x72E)) = 0;
    pl->work720[2] = 0;
    pl->work724[2] = 0;
    (*(s16 *)((u8 *)pl + 0x730)) = 0;
    pl->work720[3] = 0;
    pl->work724[3] = 0;
    (*(s16 *)((u8 *)pl + 0x732)) = 0;
    pl->work8C9 = 0;
    frame_init(pl, pl->act_tm0, pl->blend0, 0);
    frame_init(pl, (*(u16 *)&pl->act_tm1), pl->blend1, 1);
    Pl_reg_calc(pl);
    Pl_light_init(pl);
    World_calc(pl);
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
    temp_v0_4 = pl->work8C6;
    if (temp_v0_4 != 0) {
        pl->work8C6 = (u8) (temp_v0_4 - 1);
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
    temp_v1_6 = pl->work763;
    if (temp_v1_6 != 0) {
        pl->work763 = (u8) (temp_v1_6 - 1);
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
    temp_v1_9 = pl->work56F;
    if (temp_v1_9 != 0) {
        pl->work56F = (u8) (temp_v1_9 - 1);
    }
    temp_v1_10 = (*(s8 *)&pl->x881);
    if (temp_v1_10 != 0) {
        pl->x881 = (s8) (temp_v1_10 - 1);
    }
}

void timer_calc_sub_em(PLW *pl) {
    s16 temp_v1;
    u8 temp_a1;

    if ((pl->work8C3 == 0) && (temp_a1 = pl->work56A, (temp_a1 != 0xFF)) && (temp_a1 != 0)) {
        temp_v1 = pl->work572;
        if (temp_v1 != 0) {
            pl->work572 = (s16) (temp_v1 - 1);
            return;
        }
        pl->work56A = 0U;
    }
}

void pl_timer_calc(PLW *pl) {
    s16 temp_v0_3;
    s16 temp_v0_5;
    u16 temp_v0;
    u16 temp_v0_4;
    u8 temp_v0_2;
    u8 temp_v0_6;

    if (pl->x40A == 0) {
        if (pl_flag_ck(pl, 0x400001) != 0) {
            pl->work398 = (u16) (pl->work398 + 2);
        } else {
            pl->work398 = 0U;
        }
        pl->work39C = (s32) (pl->work39C + 2);
        if (pl->work39C >= 0x186A0) {
            pl->work39C = 0x2710;
        }
        temp_v0 = pl->work40C;
        if (temp_v0 != 0) {
            pl->work40C = (u16) (temp_v0 - 1);
        }
        temp_v0_2 = pl->work440;
        if (temp_v0_2 != 0) {
            pl->work440 = (u8) (temp_v0_2 - 1);
        }
        temp_v0_3 = pl->x43E;
        if (temp_v0_3 != 0) {
            pl->x43E = (s16) (temp_v0_3 - 1);
        }
        temp_v0_4 = pl->work40E;
        if (temp_v0_4 != 0) {
            pl->work40E = (u16) (temp_v0_4 - 1);
        }
        temp_v0_5 = pl->work4E0;
        if (temp_v0_5 != 0) {
            pl->work4E0 = (s16) (temp_v0_5 - 1);
        }
        temp_v0_6 = pl->work7D6;
        if (temp_v0_6 != 0) {
            pl->work7D6 = (u8) (temp_v0_6 - 1);
        }
        if (pl->x10 == 0) {
            timer_calc_sub_pl(pl);
        } else {
            timer_calc_sub_em(pl);
        }
    }
    pl_flag_clr(pl, 0x800);
    if (pl_flag_ck(pl, 1) != 0) {
        pl_flag_set(pl, 0x2000);
        return;
    }
    pl_flag_clr(pl, 0x2000);
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

u16 item_sel_sub(PLW *pl, u16 sel, int dir) {
    s16 n;
    s16 i;
    s16 found;
    found = 0;
    if (!(dir & 0xFF)) {
        n = 0;
        i = sel;
        for (;;) {
            i++;
            if (i >= 20) i = 0;
            if (pl->item[i].id != 0 && Item_data[pl->item[i].id][1] == 1) {
                sel = i;
                found = 1;
                break;
            }
            n++;
            if (n >= 20) break;
        }
    } else {
        n = 0;
        i = sel;
        for (;;) {
            if (i == 0) i = 19;
            else i--;
            if (pl->item[i].id != 0 && Item_data[pl->item[i].id][1] == 1) {
                sel = i;
                found = 1;
                break;
            }
            n++;
            if (n >= 20) break;
        }
    }
    if (found == 0) sel = 0;
    return sel;
}

int item_blank_ck(PLW *pl) {
    s16 i;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id != 0 && pl->item[i].num > 0 && Item_data[pl->item[i].id][1] == 1) return 1;
    }
    return 0;
}

void pl_item_sel(PLW *pl) {
    u16 t;
    if (Pl_master_ck(pl) == 0) return;
    if (act_ck(pl, 2, 0x13) == 0 && (u32)(pl->flag14 - 3) > 1 && Game_clear_ck(1) != 1) {
        if (pl->sw.pad08[0] & 4) {
            if (pl->work88C == 0) se_req(7, 0x11, 0);
            pl->work88C = 1;
            if (item_blank_ck(pl) != 0) {
                t = pl->sw.pad08[2];
                if (t & 0x20) {
                    se_req(7, 0x12, 0);
                    pl->work888 = item_sel_sub(pl, pl->work888, 0);
                    pl->work8F2 |= 2;
                } else if (t & 0x200) {
                    se_req(7, 0x12, 0);
                    pl->work888 = item_sel_sub(pl, pl->work888, 1);
                    pl->work8F2 |= 4;
                }
            }
        } else if (pl->work88C != 0) {
            se_req(7, 0x14, 0);
            pl->work88C = 0;
        }
        return;
    }
    pl->work88C = 0;
}

int shell_chg_ck(PLW *pl) {
    u8 k = pl->kind;
    if (k != 1 && k != 5 && pl->work88E == 0xFF) return 0;
    if (pl->sw.pad08[0] & 4) return 0;
    if (pl->work56E == 0 && pl->work56D != pl->ammo_type && pl->flag12 != 0) return 1;
    return 0;
}

int pl_shell_sel(PLW *pl) {
    u16 t;
    u8 k;
    if (Pl_master_ck(pl) == 0) return 0;
    if (act_ck(pl, 2, 0x13) != 0) return 0;
    if (pl->flag14 == 3 || pl->flag14 == 4) return 0;
    if (Game_clear_ck(1) == 1) return 0;
    if (pl->work8BE != 0) return 0;
    k = pl->kind;
    if (k != 1 && k != 5) return 0;
    if (pl->work56E == 0) {
        pl->work56E = 1;
        pl->work56D = pl->ammo_type;
        pl->work8CE = pl->work8BC;
        pl->work8D2 = pl->work01D;
        pl->work8D1 = pl->work01C;
    }
    if (pl->sw.pad08[0] & 4) {
        t = pl->sw.pad08[2];
        if (t & 0x40) {
            if (pl->work88E == 0xFF) pl->work88E = Pl_shell_set(pl, 0, 3);
            else pl->work88E = Pl_shell_set(pl, pl->work88E, 1);
            Shell_type_set(pl, 0);
            if (pl->work88E != 0xFF) {
                pl->work8F2 |= 8;
                se_req(7, 0x12, 0);
            }
        } else if (t & 0x100) {
            if (pl->work88E == 0xFF) pl->work88E = Pl_shell_set(pl, 0, 2);
            else pl->work88E = Pl_shell_set(pl, pl->work88E, 0);
            Shell_type_set(pl, 0);
            if (pl->work88E != 0xFF) {
                pl->work8F2 |= 0x10;
                se_req(7, 0x12, 0);
            }
        }
    } else if (pl->work56E != 0) {
        pl->work56E = 0;
        if (pl->work56D != pl->ammo_type) {
            Shell_type_set(pl, 0);
        } else {
            pl->work8BC = pl->work8CE;
            pl->work01D = pl->work8D2;
            pl->work01C = pl->work8D1;
        }
    }
    if (pl->work56D == pl->ammo_type) {
        pl->work8BC = pl->work8CE;
        pl->work01D = pl->work8D2;
        pl->work01C = pl->work8D1;
    }
    return 0;
}

int pl_status_ck(PLW *pl) {
    if (pl->x7B2 > 0) return 1;
    if (pl->x7C4 > 0) return 2;
    return 0;
}

void hit_stop_calc(PLW *pl) {
    u8 a = pl->work409;
    if (a != 0) {
        pl->x40A += a;
        pl->work409 = 0;
        return;
    }
    if (pl->x40A != 0) pl->x40A--;
}

int stick_dir_set(PLW *pl, int no) {
    return (pl->sw.ang[no] + (Get_view_dir() & 0xFFFF)) & 0xFFFF;
}

int stick_pow_get(PLW *pl) {
    u8 r = 0;
    u16 p;
    switch (pl->kind) {
    case 1:
    case 5:
        if (pl->flag12 != 0 && pl->work764 == 1) return 0;
    default:
    case 0:
    case 2:
    case 3:
    case 4:
        pl->work8C8 = 0;
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
                pl->work3A8 = ((calc_vec_ang2(em->pos, pl->pos) & 0xFFFF) + 0x4000) & 0xFFFF,
                k = em->kind, (u32)(k - 0x1B) < 2 || k == 0x1F)) {
            pl->work7EE = 1;
        }
        em++;
    }
}

int em_ninshiki_ck2(PLW *pl) {
    u8 a = pl->work81E;
    if (a != 0) {
        if (a != pl->work81F && pl->work7EE == 0) {
            pl->work7EE = 1;
            return 1;
        }
        return 0xFF;
    }
    return 0;
}

int pl_ride_ck(PLW *pl) {
    f32 d, lim;
    u8 k;
    d = flvecCalcLength((f32 *)((u8 *)pl + 0x618));
    k = pl->flag14;
    if (k == 0 && pl->flag15 == 0xC) lim = 10.0f;
    else if (k == 0 && pl->flag15 == 0x16) lim = 4.0f;
    else lim = 7.0f;
    if (!(d < lim)) {
        if (k == 0 && pl->flag15 == 0x16) return 1;
        Pl_act_set(pl, 0, 0x16, 0);
        return 1;
    }
    return 0;
}

int ex_kabe_ck(PLW *pl) {
    u16 a = pl->sw.an_now;
    if (a & 0x2000) {
        if (act_ck(pl, 0, 0x28) == 0) Pl_act_set(pl, 0, 0x28, 0xC);
        return 1;
    }
    if (a & 0x1000) {
        if (act_ck(pl, 0, 0x29) == 0) Pl_act_set(pl, 0, 0x29, 0xC);
        return 1;
    }
    if (pl->sw.trg & 0x40) {
        Pl_act_set(pl, 0, 0x30, 0);
        return 1;
    }
    return 0;
}
