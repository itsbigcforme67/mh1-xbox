/* em_cmd_nm - game.bin 0x0055B060-0x0056653C (f_em_55B060): the monster command interpreter.
 * Each monster runs a byte-code program (em_cmdN_tbl[kind]); em_cmd_ck steps it and
 * every opcode has an em_cmd_* handler taking (em, program pointer after the opcode)
 * and returning the pointer to continue at. Condition commands (mode 0) skip to the
 * matching else (mode 1) / end (mode 2) marker with CMD_SKIP / else_ck.
 * Whole file; matching runs are built as em_cmd*.c. Meanings of fields are guesses. */
#include "em_cmd.h"

void em_cmd_init(EMW *em) {
    em->cmd_idx = 0;
    em->x822 = 0;
    em->x824 = 0;
    EM_FIELD(em, s8 *, 0x826) = 0;
    em->x84E = 0;
    em->x844 = -1;
    switch (*(u8 *)0x3F341E) {
    case 0:
    case 1:
        em->cmd_tbl = em_cmd0_tbl[em->kind];
        break;
    case 2:
        em->cmd_tbl = em_cmd1_tbl[em->kind];
        break;
    case 3:
        em->cmd_tbl = em_cmd2_tbl[em->kind];
        break;
    case 4:
        em->cmd_tbl = em_cmd3_tbl[em->kind];
        break;
    case 5:
        em->cmd_tbl = em_cmd4_tbl[em->kind];
        break;
    case 6:
        em->cmd_tbl = em_cmd5_tbl[em->kind];
        break;
    case 7:
        em->cmd_tbl = em_cmd6_tbl[em->kind];
        break;
    }
    em->cmd_pc = em_cmd_top(em);
    em->x928 = -1;
    em->x929 = -1;
    em->x92D = -1;
    em->x92C = -1;
    em->x92F = 0xFF;
    em->x92E = 0xFF;
    em->x854 = 0;
    em_atk_bit = 0;
    em->cmd_top = em->cmd_pc;
    em->x827 = 0;
    em->x828 = 0;
    em->x829 = 0;
    em->x881 = 0;
    em->x882 = 0;
    em->x883 = 0;
}

void em_cmd_ck(EMW *em) {
    u16 run = 1;
    u8 *p;
    int n = 0;
    s32 a;
    u16 b;
    u8 *q;

    if (em->mode != 5) {
        em->x8BD = 0;
        reset_flag_ck(em);
        if ((p = cancel_prog_ck(em)) == NULL) {
            p = set_cmd(em);
        }
        while (run) {
            n++;
            if (n >= 0x3E8) {
                em->x9DA = 2;
            }
            if (em->x9DA != 0) {
                em_cmd_reset(em);
                break;
            }
            if (p == NULL) {
                em->x9DA = 0;
                em_cmd_reset(em);
                break;
            }
            switch (*p) {                  /* irregular */
                case 0x1:
                    p = em_cmd_kehai_ck(em, p + 1);
                    break;
                case 0x2:
                    p = em_cmd_ninshiki_ck(em, p + 1);
                    break;
                case 0x3:
                    p = em_cmd_area_move_ck(em, p + 1);
                    break;
                case 0x4:
                    em->cmd_idx = 0;
                    p = em_cmd_top(em);
                    break;
                case 0x5:
                    p++;
                    a = *p++;
                    b = *p++;
                    em->x82B = *p++;
                    if (em->x8C3 == 0) {
                        em_type_act_set(em, a, b, 1);
                    }
                    ret_cmd(em, p);
                    run = 0;
                    break;
                case 0x6:
                    p = em_cmd_target_set(em, p + 1);
                    break;
                case 0x7:
                    p = em_cmd_main_jump(em, p + 1);
                    break;
                case 0x8:
                    p = em_cmd_stand_ck(em, p + 1);
                    break;
                case 0x9:
                    p = em_cmd_fly_ck(em, p + 1);
                    break;
                case 0xA:
                    p = em_cmd_body_status_set(em, p + 1);
                    break;
                case 0xB:
                    p = em_cmd_mode_ck(em, p + 1);
                    break;
                case 0xC:
                    p = em_cmd_flag_set(em, p + 1);
                    break;
                case 0xD:
                    p = em_cmd_flag_clear(em, p + 1);
                    break;
                case 0xE:
                    p = em_cmd_stage_no_ck(em, p + 1);
                    break;
                case 0xF:
                    p = em_cmd_route_set(em, p + 1);
                    break;
                case 0x10:
                    p = em_cmd_route_ck(em, p + 1);
                    break;
                case 0x11:
                    p = em_cmd_kehai_pl_set(em, p + 1);
                    break;
                case 0x12:
                    p = em_cmd_find_ck(em, p + 1);
                    break;
                case 0x13:
                    p = em_cmd_pl_target_set(em, p + 1);
                    break;
                case 0x14:
                    p = em_cmd_angle_ck(em, p + 1);
                    break;
                case 0x15:
                    p = em_cmd_stage_no_sel(em, p + 1);
                    break;
                case 0x16:
                    p = em_cmd_action_set(em, p + 1);
                    break;
                case 0x17:
                    p = em_cmd_area_route_set(em, p + 1);
                    break;
                case 0x18:
                    p = em_cmd_area_route_move(em, p + 1);
                    break;
                case 0x19:
                    p = em_cmd_area_route_ck(em, p + 1);
                    break;
                case 0x1A:
                    p = em_cmd_escape_area_set(em, p + 1);
                    break;
                case 0x1B:
                    p = em_cmd_mind_ck(em, p + 1);
                    break;
                case 0x1C:
                    p = em_cmd_mind_no_ck(em, p + 1);
                    break;
                case 0x1D:
                    p = em_cmd_mind_sel(em, p + 1);
                    break;
                case 0x1E:
                    p = em_cmd_mind_move_end(em, p + 1);
                    break;
                case 0x20:
                    p = em_cmd_pl_ang_sel(em, p + 1);
                    break;
                case 0x21:
                    p = em_cmd_thirst_ck(em, p + 1);
                    break;
                case 0x22:
                    p = em_cmd_near_pos_ck(em, p + 1);
                    break;
                case 0x23:
                    p = em_cmd_emtype_ck(em, p + 1);
                    break;
                case 0x24:
                    p = em_cmd_repeat_cnt_set(em, p + 1);
                    break;
                case 0x25:
                    p = em_cmd_repeat_cnt_clr(em, p + 1);
                    break;
                case 0x26:
                    p = em_cmd_demo_flag_set(em, p + 1);
                    break;
                case 0x27:
                    p = em_cmd_type_sel(em, p + 1);
                    break;
                case 0x28:
                    p = em_cmd_all_pl_same_stage_ck(em, p + 1);
                    break;
                case 0x29:
                    p = em_cmd_stay_timer_ck(em, p + 1);
                    break;
                case 0x2A:
                    p = em_cmd_runaway_timer_ck(em, p + 1);
                    break;
                case 0x2B:
                    p = em_cmd_flag_ck(em, p + 1);
                    break;
                case 0x2C:
                    p = em_cmd_myemtype_sel(em, p + 1);
                    break;
                case 0x2D:
                    p = em_cmd_smell_set(em, p + 1);
                    break;
                case 0x2E:
                    p = em_cmd_search_data_set(em, p + 1);
                    break;
                case 0x2F:
                    p = em_cmd_egg_ck(em, p + 1);
                    break;
                case 0x30:
                    p = em_cmd_egg_cancel_ck(em, p + 1);
                    break;
                case 0x31:
                    p = em_cmd_yobi_pos_set(em, p + 1);
                    break;
                case 0x32:
                    p = em_cmd_body_status_ck(em, p + 1);
                    break;
                case 0x33:
                    p = em_cmd_body_status_sel(em, p + 1);
                    break;
                case 0x34:
                    p = em_cmd_act_st_ck(em, p + 1);
                    break;
                case 0x35:
                    p = em_cmd_ikari_ck(em, p + 1);
                    break;
                case 0x38:
                    p = em_cmd_water_ck(em, p + 1);
                    break;
                case 0x39:
                    p = em_cmd_eye_dmg_ck(em, p + 1);
                    break;
                case 0x3A:
                    p = em_cmd_sensor_ck(em, p + 1);
                    break;
                case 0x3B:
                    p = em_cmd_boss_work_ck(em, p + 1);
                    break;
                case 0x3C:
                    p = em_cmd_boss_atk_ck(em, p + 1);
                    break;
                case 0x3D:
                    p = em_cmd_before_stage_ck(em, p + 1);
                    break;
                case 0x3E:
                    p = em_cmd_before_stage_sel(em, p + 1);
                    break;
                case 0x3F:
                    p = em_cmd_ground_area_move(em, p + 1);
                    break;
                case 0x40:
                    p = em_cmd_em_mode_change(em, p + 1);
                    break;
                case 0x41:
                    p = em_cmd_em_hp_vital_add(em, p + 1);
                    break;
                case 0x42:
                    p = em_cmd_horm_pos_ang_ck(em, p + 1);
                    break;
                case 0x44:
                    p = em_cmd_boss_same_stage_ck(em, p + 1);
                    break;
                case 0x45:
                    p = em_cmd_pl_fishing_ck(em, p + 1);
                    break;
                case 0x46:
                    p = em_cmd_target_pl_act_ck(em, p + 1);
                    break;
                case 0x47:
                    p = em_cmd_fish_ok_ck(em, p + 1);
                    break;
                case 0x48:
                    p = em_cmd_timer_set(em, p + 1);
                    break;
                case 0x49:
                    p = em_cmd_pl_land_target(em, p + 1);
                    break;
                case 0x4A:
                    p = em_cmd_pl_look_ck(em, p + 1);
                    break;
                case 0x4B:
                    p = em_cmd_kehai_clear(em, p + 1);
                    break;
                case 0x4C:
                    p = em_cmd_hate_clear(em, p + 1);
                    break;
                case 0x4D:
                    p = em_cmd_horm_pos_set(em, p + 1);
                    break;
                case 0x4E:
                    p = em_cmd_thirst_add(em, p + 1);
                    break;
                case 0x4F:
                    p = em_cmd_hungry_add(em, p + 1);
                    break;
                case 0x50:
                    p = em_cmd_suimin_add(em, p + 1);
                    break;
                case 0x51:
                    p = em_cmd_swim_ck(em, p + 1);
                    break;
                case 0x52:
                    p = em_cmd_all_pl_target_sel(em, p + 1);
                    break;
                case 0x53:
                    p = em_cmd_samestage_pl_target_sel(em, p + 1);
                    break;
                case 0x54:
                    p = em_cmd_target_pl_samestage_ck(em, p + 1);
                    break;
                case 0x55:
                    p = em_cmd_target_pl_hate_high_ck(em, p + 1);
                    break;
                case 0x56:
                    p = em_cmd_quest_no_ck(em, p + 1);
                    break;
                case 0x57:
                    p = em_cmd_quest_no_sel(em, p + 1);
                    break;
                case 0x58:
                    p = em_cmd_boss_pl_target_set(em, p + 1);
                    break;
                case 0x59:
                    p = em_cmd_tenjo_ck(em, p + 1);
                    break;
                case 0x5A:
                    p = em_cmd_target_land_no_ck(em, p + 1);
                    break;
                case 0x5B:
                    p = em_cmd_ninshiki_timer_sub(em, p + 1);
                    break;
                case 0x5C:
                    p = em_cmd_tenjostage_ck(em, p + 1);
                    break;
                case 0x5D:
                    p = em_cmd_smell_set_ck(em, p + 1);
                    break;
                case 0x5E:
                    p = em_cmd_my_floor_ck(em, p + 1);
                    break;
                case 0x5F:
                    p = em_cmd_st25_pl_target_sel(em, p + 1);
                    break;
                case 0x60:
                    p = em_cmd_st25_gate_ck(em, p + 1);
                    break;
                case 0x61:
                    p = em_cmd_runaway_timer_set(em, p + 1);
                    break;
                case 0x62:
                    p = em_cmd_dansa_sel(em, p + 1);
                    break;
                case 0x63:
                    p = em_cmd_male_ck(em, p + 1);
                    break;
                case 0x64:
                    p = em_cmd_target_pl_hate_ck(em, p + 1);
                    break;
                case 0x65:
                    p = em_cmd_all_pl_hate_clear(em, p + 1);
                    break;
                case 0x66:
                    p = em_cmd_pl_ride_ck(em, p + 1);
                    break;
                case 0x67:
                    p = em_cmd_em_master_ck(em, p + 1);
                    break;
                case 0x68:
                    p = em_cmd_em_cmd_reset(em, p + 1);
                    break;
                case 0x80:
                    p = em_cmd_rnd32(em, p + 1);
                    break;
                case 0x81:
                    p = em_cmd_contents(em, p + 1);
                    break;
                case 0x82:
                    p = em_cmd_sub_contents(em, p + 1);
                    break;
                case 0x83:
                    p = em_cmd_range_ck(em, p + 1);
                    break;
                case 0x84:
                    p += 1;
                    break;
                case 0xFF:
                    p = em_cmd_end_command(em, p + 1);
                    break;
                case 0x90:
                    p = em_cmd_position_set(em, p + 1);
                    break;
                case 0x91:
                    p = em_cmd_vec_set(em, p + 1);
                    break;
                case 0x92:
                    p = em_cmd_demo_start(em, p + 1);
                    break;
                case 0x93:
                    p = em_cmd_wait_set(em, p + 1);
                    break;
                case 0x94:
                    p = em_cmd_em_atk_bit(em, p + 1);
                    break;
                default:
                    em_cmd_reset(em);
                    run = 0;
                    break;
                }
        }
        em->cmd_ofs = em->cmd_pc - em->cmd_top;
    }
}

u8 *em_cmd_kehai_ck(EMW *em, u8 *p) {
    u8 *q;
    int i;
    int cnt;
    u8 n;

    q = p;
    switch (*q) {
    case 0:
        n = *(u8 *)0x3F34C3;
        q += 1;
        i = 0;
        cnt = 0;
        for (; i < n; i++) {
            if (em->x914 & (1 << i)) {
                cnt += 1;
            }
        }
        if (cnt == 0) {
            CMD_SKIP(em, q, 1);
        }
        break;
    case 1:
        q = else_ck(em, q + 1, 1);
        break;
    case 2:
        q += 1;
        break;
    }
    return q;
}

u8 *em_cmd_ninshiki_ck(EMW *em, u8 *p) {
    u8 *q;

    q = p;
    switch (*q) {
    case 0:
        q += 1;
        if (em->x88F == 0) {
            em->x844 = -1;
            CMD_SKIP(em, q, 2);
        }
        break;
    case 1:
        q = else_ck(em, q + 1, 2);
        break;
    case 2:
        q += 1;
        break;
    }
    return q;
}

u8 *em_cmd_area_move_ck(EMW *em, u8 *p) {
    u16 stg;
    u8 *q;
    u8 v;
    u8 *tbl;

    q = p;
    switch (*q) {
    case 0:
        tbl = em_area_mv_tbl[em->kind];
        stg = em->x73A;
        q += 1;
        if (em->stg != stg && stg != 0xFF && (v = tbl[stg]) != 0) {
            if (v != 2 && v != 1) {
            }
        } else {
            em->x827 = 0;
            em->x828 = 0;
            em->x829 = 0;
            em->x881 = 0;
            em->x882 = 0;
            em->x883 = 0;
            CMD_SKIP(em, q, 3);
        }
        break;
    case 1:
        q = else_ck(em, q + 1, 3);
        break;
    case 2:
        q += 1;
        break;
    }
    return q;
}

u8 *em_cmd_main_jump(EMW *em, u8 *p) {
    em->cmd_idx = *p;
    return em_cmd_top(em);
}

u8 *em_cmd_stand_ck(EMW *em, u8 *p) {
    u8 *q;

    q = p;
    switch (*q++) {
    case 0:
        if (em->x388 != 0) {
            CMD_SKIP(em, q, 8);
        }
        break;
    case 1:
        q = else_ck(em, q, 8);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_fly_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a2;
    u8 temp_v1;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x388 != 2) {
            CMD_SKIP(em, var_a1, 9);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 9);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_body_status_set(EMW *em, u8 *p) {
    EM_FIELD(em, u8 *, 0x762) = (u8) *p;
    return p + 1;
}

u8 *em_cmd_mode_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    u8 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        em->x838 = (u8) em->x888;
        if (em->x888 != v) {
            CMD_SKIP(em, var_a1, 0xB);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0xB);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_flag_set(EMW *em, u8 *p) {
    s32 n;
    s32 i;
    u8 *ex;
    u8 kind;
    u8 val;

    kind = p[0];
    val = p[1];
    p += 2;
    if (kind == 0) {
        kind = 1;
        n = 7;
    } else {
        n = 1;
    }
    ex = em->ex;
    {
        for (i = 0; i < n; i++) {
            switch (kind) {
            case 1:
                em->x88B = val;
                break;
            case 2:
                EM_FIELD(em, u8 *, 0x8C0) = val;
                break;
            case 3:
                em->x9E1 = (s8)val;
                break;
            case 4:
                em->x83A = val;
                break;
            case 5:
                if (em->kind == 7 && em->x8C3 == 0) {
                    EM_FIELD(em, u8 *, 0x45C) = val;
                }
                break;
            case 6:
                if (em->kind == 0xF && em->x8C3 == 0) {
                    ex[0x44] = val;
                }
                break;
            case 7:
                if (em->kind == 0xF && em->x8C3 == 0) {
                    ex[0x45] = val;
                }
                break;
            }
            kind = kind + 1;
        }
    }
    return p;
}

u8 *em_cmd_flag_clear(EMW *em, u8 *p) {
    s32 n;
    s32 i;
    u8 *ex;
    u8 kind;

    kind = p[0];
    p += 1;
    if (kind == 0) {
        kind = 1;
        n = 7;
    } else {
        n = 1;
    }
    ex = em->ex;
    for (i = 0; i < n; i++) {
        switch (kind) {
        case 1:
            em->x88B = 0;
            break;
        case 2:
            em->x8C0 = 0;
            break;
        case 3:
            em->x9E1 = 0;
            break;
        case 4:
            em->x83A = 0;
            break;
        case 5:
            if (em->kind == 7 && em->x8C3 == 0) {
                EM_FIELD(em, u8 *, 0x45C) = 0;
            }
            break;
        case 6:
            if (em->kind == 0xF && em->x8C3 == 0) {
                ex[0x44] = 0;
            }
            break;
        case 7:
            if (em->kind == 0xF && em->x8C3 == 0) {
                ex[0x45] = 0;
            }
            break;
        }
        kind = kind + 1;
    }
    return p;
}

u8 *em_cmd_stage_no_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    u8 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (em->stg != v) {
            CMD_SKIP(em, var_a1, 0xE);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0xE);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_route_set(EMW *em, u8 *p) {
    EM_STG_POS *s;
    u8 max;
    u8 kind;

    em->x92A = 0;
    em->x830 = *p++;
    max = *p++;
    em->x831 = *p++;
    em->x832 = *p++;
    em->x833 = *p++;
    ret_cmd(em, p);
    em->x83B |= 1;
    kind = em->x830;
    switch (kind) {
    case 0:
    case 1:
    case 2:
        s = gp_ck(em, em->area->x4, em->stg);
        if (s == NULL) {
            em->x83B &= 0xFE;
            return p;
        }
        if (s->num < max) {
            max = s->num;
        }
        em->x929 = max;
        em->x92B = s->num;
        em->cmd_route = route_ptr_set(em, em->x832);
        break;
    default:
        break;
    }

    return em->cmd_route;
}

u8 *em_cmd_route_ck(EMW *em, u8 *p) {
    u8 list[16];
    s8 n;
    s8 i;

    switch (em->x830) {
    case 0:
        if (em->x928 == -1 || em->x928 >= em->x92B) {
            i = 0;
            n = 0;
            for (; i < em->x92B; i++) {
                list[n] = i;
                n++;
            }
            if (n == 0) {
                em->x928 = 0;
                em_cmd_reset(em);
            } else {
                em->x928 = list[(s8)(em->x39A % n)];
            }
        }
        break;
    case 1:
        if (em->x928 == -1 || em->x928 >= em->x92B) {
            em->x928 = *option_route_ptr_set(em, em->x831);
        }
        break;
    case 2:
        if (em->x928 == -1 || em->x928 >= em->x92B) {
            em->x928 = 0;
        }
        break;
    }
    em->x827 = 2;
    em->x828 = 1;
    em->x829 = em->x928;
    return p;
}

u8 *em_cmd_kehai_pl_set(EMW *em, u8 *p) {
    int i;

    for (i = 0; i < *(u8 *)0x3F34C3; i++) {
        if (em->x915 & (1 << i)) {
            em->x829 = i;
        }
    }
    if (em->x8C3 == 0) {
        em->x884 = 2;
    }
    em->x827 = 1;
    em->x828 = 0;
    return p;
}

u8 *em_cmd_find_ck(EMW *em, u8 *p) {
    int i;

    for (i = 0; i < *(u8 *)0x3F34C3; i++) {
        if (em->x88F & (1 << i)) {
            em->x829 = i;
            em->x844 = em->x829;
        }
    }
    em->x827 = 1;
    em->x828 = 0;
    if (em->x8C3 == 0) {
        em->x885 = 2;
    }
    em->x83B &= 0xEE;
    return p;
}

u8 *em_cmd_pl_target_set(EMW *em, u8 *p) {
    em->x827 = 1;
    em->x828 = 0;
    if (em->x844 == -1) {
        em->x829 = 0xFF;
    } else {
        em->x829 = em->x844 & 0xF;
    }
    return p;
}

u8 *em_cmd_angle_ck(EMW *em, u8 *p) {
    f32 sp70[3];
    f32 sp60[3];
    s32 temp_s0;
    s32 var_a0;
    s32 var_s1;
    u8 *temp_v0;
    u8 *temp_v0_2;
    u8 *var_s2;
    u8 *var_v0;
    u8 temp_a0;
    u8 temp_s4;
    u8 temp_v1;
    u8 temp_v1_2;

    var_s2 = p;
    switch (*var_s2++) {
    case 0:
        var_s1 = 0;
        temp_s0 = (s32)(0.5f + ((65536.0f * (f32) *(s8 *)var_s2) / 360.0f)) & 0xFFFF;
        var_s2 += 1;
        if ((em->x827 == 1) && (em->x828 == 0) && (temp_s4 = em->x829, (temp_s4 != -1U))) {
            em_pl_pos_set(em, temp_s4, sp70);
            World_calc2(player_work[temp_s4].stg, sp70, sp60);
            var_a0 = ((Em_Calc_angY(em->x754, sp60) & 0xFFFF) - em->ang[1]) & 0xFFFF;
            if (var_a0 >= 0x8001) {
                var_a0 = (0x10000 - var_a0) & 0xFFFF;
            }
            if ((var_a0 & 0xFFFF) < (temp_s0 & 0xFFFF)) {
                var_s1 = 1;
            }
        } else {
            var_s1 = 1;
        }
        if (var_s1 == 0) {
            if ((EM_FIELD(var_s2, u8 *, 0) == 0x14) && (EM_FIELD(var_s2, u8 *, 1) == 1)) {
                temp_v0 = next_cmd_search(em, var_s2);
                if ((EM_FIELD(temp_v0, u8 *, 0) == 0x14) && (EM_FIELD(temp_v0, u8 *, 1) == 2)) {
                    var_v0 = next_cmd_search(em, temp_v0);
                } else {
                    var_v0 = next_cmd_search(em, cmd_end_search(em, temp_v0, 0x14, 2));
                }
                goto block_33;
            }
        } else {
loop_22:
            temp_a0 = EM_FIELD(var_s2, u8 *, 0);
            if ((temp_a0 != 0x14) || (EM_FIELD(var_s2, u8 *, 1) != 1)) {
                if (temp_a0 == 0x14) {
                    if (EM_FIELD(var_s2, u8 *, 1) != 2) {
                        goto block_27;
                    }
                } else {
block_27:
                    var_s2 = cmd_end_search(em, var_s2, 0x14, 2);
                    goto loop_22;
                }
            }
            temp_v0_2 = next_cmd_search(em, var_s2);
            temp_v1_2 = EM_FIELD(temp_v0_2, u8 *, 0);
            var_s2 = temp_v0_2;
            if ((temp_v1_2 == 0x14) && (EM_FIELD(var_s2, u8 *, 1) == 2)) {
                var_v0 = next_cmd_search(em, var_s2);
block_33:
                var_s2 = var_v0;
            }
        }
        break;
    case 1:
        var_v0 = else_ck(em, var_s2, 0x14);
        goto block_33;
    case 2:
        break;
    }
    return var_s2;
}

CMD_SEL_FUNC(em_cmd_stage_no_sel, 0x15, u8, u8, em->stg)

u8 *em_cmd_action_set(EMW *em, u8 *p) {
    em->x845 = *p;
    em->cmd_p840 = p + 1;
    p = action_ptr_set(em, em->x845);
    em->cmd_top = p;
    return p;
}

u8 *em_cmd_area_route_set(EMW *em, u8 *p) {
    if (em->x92C != -1) {
        return p + 4;
    }
    em->x846 = *p++;
    em->x92C = *p++;
    em->x847 = *p++;
    em->x84C = *p++;
    em->x92D = 0;
    if (em->x92C == 0) {
        em->x92C = -1;
        return p;
    }
    return p;
}

u8 *em_cmd_area_route_move(EMW *em, u8 *p) {
    s32 temp_v0;
    s32 var_s0;
    u8 temp_v1;

    var_s0 = 0;
    if (em->x92C == -1) {
        return p;
    }
    EM_FIELD(em, s8 *, 0x84D) = 1;
    EM_FIELD(em, s8 *, 0x92E) = 0xFF;
loop_4:
    temp_v1 = *(area_route_ptr_set(em, em->x846) + em->x92D);
    if ((s32) temp_v1 >= 0x80) {
        em->x92F = (u8) *area_route_rnd32(em, *(EM_FIELD(em->cmd_tbl, s32 *, 0x1C) + ((temp_v1 & 0x7F) * 4)));
    } else {
        em->x92F = temp_v1;
    }
    if (em->x92F == em->stg) {
        em->x92D = (s8) (em->x92D + 1);
        if (em->x92D >= em->x92C) {
            if (em->x84C == 0) {
                var_s0 = 1;
                em->x92D = -1;
                em->x92C = -1;
            } else {
                em->x92D = 0;
            }
        }
        if ((var_s0 & 0xFF) == 1) {
            return p;
        }
        goto loop_4;
    }
    EM_FIELD(em, s32 *, 0x848) = p;
    temp_v0 = area_move_ptr_set(em, em->x847);
    EM_FIELD(em, s32 *, 0x870) = temp_v0;
    return temp_v0;
}

u8 *em_cmd_area_route_ck(EMW *em, u8 *p) {
    u8 temp_v1;

    EM_FIELD(em, s8 *, 0x827) = 3;
    temp_v1 = em->x92F;
    em->x829 = temp_v1;
    em->x828 = temp_v1;
    return p;
}

u8 *em_cmd_escape_area_set(EMW *em, u8 *p) {
    f32 sp164;
    f32 sp15C;
    f32 sp158;
    f32 sp150;
    f32 spF0;
    u16 spE0;
    u16 spD0;
    u16 spC0;
    u8 *spBC;
    u8 spA0;
    f32 *var_s1;
    f32 *var_s4;
    s32 temp_s6;
    s32 temp_v0;
    s32 var_a1;
    s32 var_s0;
    int temp_s1;
    int var_s0_2;
    int var_s2;
    int var_s5;
    int var_s5_2;
    s8 temp_a0;
    s8 temp_v1_4;
    u16 *var_s2_2;
    u16 *var_s3;
    u16 temp_v1_2;
    u16 temp_v1_3;
    u16 var_s7;
    u8 temp_v1;
    u8 temp_v1_5;
    void *temp_v0_2;
    void *temp_v0_3;
    void *temp_v0_4;

    var_s7 = saved_reg_s7;
    spBC = p;
    spA0 = *p;
    EM_FIELD(em, s8 *, 0x827) = 3;
    temp_v0 = st_mv_ptr_ck();
    if (temp_v0 == 0) {

    } else {
        var_a1 = 0;
loop_4:
        if (spA0 == *(temp_v0 + var_a1)) {

        } else {
            var_a1 += 1;
            if (var_a1 >= 8) {
                temp_v0_2 = Stage_data_get(em->stg, var_a1);
                sp150 = EM_FIELD(temp_v0_2, f32 *, 0) + (EM_FIELD(temp_v0_2, f32 *, 0x10) / 2.0f);
                sp158 = EM_FIELD(temp_v0_2, f32 *, 4) + (EM_FIELD(temp_v0_2, f32 *, 0x14) / 2.0f);
                temp_v0_3 = Stage_data_get(spA0);
                sp15C = EM_FIELD(temp_v0_3, f32 *, 0) + (EM_FIELD(temp_v0_3, f32 *, 0x10) / 2.0f);
                sp164 = EM_FIELD(temp_v0_3, f32 *, 4) + (EM_FIELD(temp_v0_3, f32 *, 0x14) / 2.0f);
                var_s4 = &spF0;
                temp_s6 = st_mv_ptr_ck(em);
                var_s0 = 0;
                var_s2 = 0;
                var_s1 = var_s4;
loop_9:
                temp_a0 = *(temp_s6 + var_s0);
                if (temp_a0 != -1) {
                    temp_v0_4 = Stage_data_get((u8) temp_a0);
                    var_s0 += 1;
                    var_s2 = (s8)(var_s2 + 1);
                    EM_FIELD(var_s1, f32 *, 0) = EM_FIELD(temp_v0_4, f32 *, 0) + (EM_FIELD(temp_v0_4, f32 *, 0x10) / 2.0f);
                    EM_FIELD(var_s1, f32 *, 8) = (f32) (EM_FIELD(temp_v0_4, f32 *, 4) + (EM_FIELD(temp_v0_4, f32 *, 0x14) / 2.0f));
                    var_s1 += 0xC;
                    if (var_s0 < 8) {
                        goto loop_9;
                    }
                }
                temp_s1 = (s8)(var_s2);
                if (temp_s1 == 1) {

                } else {
                    var_s5 = -1;
                    spE0 = Em_Calc_angY(&sp150, &sp15C);
                    var_s0_2 = 0;
                    if (temp_s1 > 0) {
                        var_s3 = &spD0;
                        var_s2_2 = &spC0;
                        do {
                            temp_v1 = em->x92E;
                            if (temp_v1 != 0xFF) {
                                if (temp_v1 != *(temp_s6 + var_s0_2)) {
                                    goto block_18;
                                }
                            } else {
block_18:
                                *var_s3 = Em_Calc_angY(&sp150, var_s4);
                                *var_s2_2 = *var_s3 - spE0;
                                temp_v1_2 = *var_s2_2;
                                if ((s32) temp_v1_2 >= 0x8001) {
                                    *var_s2_2 = 0x10000 - temp_v1_2;
                                }
                                if (((s8)(var_s5)) == -1) {
                                    var_s7 = *var_s2_2;
                                    var_s5_2 = var_s0_2 << 0x38;
                                    goto block_25;
                                }
                                temp_v1_3 = *var_s2_2;
                                if ((s32) temp_v1_3 < (var_s7 & 0xFFFF)) {
                                    var_s5_2 = var_s0_2 << 0x38;
                                    var_s7 = temp_v1_3;
block_25:
                                    var_s5 = var_s5_2 >> 0x38;
                                }
                            }
                            var_s0_2 += 1;
                            var_s4 += 0xC;
                            var_s3 += 2;
                            var_s2_2 += 2;
                        } while (var_s0_2 < temp_s1);
                    }
                    temp_v1_4 = *(temp_s6 + ((s8)(var_s5)));
                    if (temp_v1_4 != -1) {
                        spA0 = temp_v1_4 & 0xFF;
                    }
                }
            } else {
                goto loop_4;
            }
        }
    }
    em->x92F = spA0;
    temp_v1_5 = em->x92F;
    em->x829 = temp_v1_5;
    em->x828 = temp_v1_5;
    return spBC + 1;
}

u8 *em_cmd_mind_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    u8 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (v != em->x889) {
            CMD_SKIP(em, var_a1, 0x1B);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x1B);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_mind_no_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    u8 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (v != em->x88A) {
            CMD_SKIP(em, var_a1, 0x1C);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x1C);
        break;
    case 2:
        break;
    }
    return var_a1;
}

CMD_SEL_FUNC(em_cmd_mind_sel, 0x1D, u8, u8, em->x88A)

u8 *em_cmd_mind_move_end(EMW *em, u8 *p) {
    EM_FIELD(em, s8 *, 0x889) = 0;
    EM_FIELD(em, s8 *, 0x88A) = 0;
    EM_FIELD(em, s8 *, 0x9E3) = 0;
    EM_FIELD(em, s8 *, 0x9E4) = 0;
    EM_FIELD(em, s8 *, 0x9E5) = 0;
    EM_FIELD(em, s8 *, 0x9E6) = 0;
    EM_FIELD(em, s8 *, 0x9E7) = 0;
    EM_FIELD(em, s8 *, 0x9E8) = 0;
    em->x8C1 = 0;
    em->x8C0 = 0;
    EM_FIELD(em, s8 *, 0x8BF) = 0;
    return p;
}

u8 *em_cmd_pl_ang_sel(EMW *em, u8 *p) {
    f32 var_f2;
    s32 var_s0;
    s32 var_s0_2;
    s8 temp_v1_2;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v0;
    u8 temp_v1;
    void *temp_v0_2;
    void *temp_v0_3;

    var_a1 = p + 1;
    temp_v1 = EM_FIELD(p, u8 *, 0);
    switch (temp_v1) {                              /* irregular */
    case 3:
        break;
    case 0:
        var_a1 += 3;
        if ((EM_FIELD(p, u8 *, 1) & 0xFF) <= 0) {

        } else {
            temp_v0 = *var_a1;
            if ((s32) temp_v0 >= 0) {
                var_f2 = (f32) temp_v0;
            } else {
                var_f2 = 2.0f * (f32) ((temp_v0 >> 1) | (temp_v0 & 1));
            }
            temp_v1_2 = em->x844;
            var_a1 += 1;
            if ((M2C_BITWISE(s32, (0.5f + ((65536.0f * var_f2) / 360.0f))) & 0xFFFF) >= (s32) EM_FIELD(((temp_v1_2 * 2) + em), u16 *, 0x904)) {
                if (temp_v1_2 == -1) {
                    goto block_14;
                }
            } else {
block_14:
                var_s0 = 1;
                do {
                    temp_v0_2 = cmd_end_search(em, var_a1, 0x20, 3);
                    temp_a0 = EM_FIELD(temp_v0_2, u8 *, 0);
                    if (temp_a0 == 0x20) {
                        if (EM_FIELD(temp_v0_2, u8 *, 1) != 2) {
                            goto block_18;
                        }
                        goto block_21;
                    }
block_18:
                    if ((temp_a0 == 0x20) && (EM_FIELD(temp_v0_2, u8 *, 1) == 3)) {
block_21:
                        var_s0 = 0;
                    }
                    var_a1 = next_cmd_search(em, temp_v0_2);
                } while (var_s0 != 0);
            }
        }
        break;
    case 1:
        var_a1 += 1;
        /* fallthrough */
    case 2:
        var_s0_2 = 1;
        do {
            temp_v0_3 = cmd_end_search(em, var_a1, 0x20, 3);
            if ((EM_FIELD(temp_v0_3, u8 *, 0) == 0x20) && (EM_FIELD(temp_v0_3, u8 *, 1) == 3)) {
                var_s0_2 = 0;
            }
            var_a1 = next_cmd_search(em, temp_v0_3);
        } while (var_s0_2 != 0);
        break;
    }
    return var_a1;
}

u8 *em_cmd_thirst_ck(EMW *em, u8 *p) {
    u8 *q;

    q = p;
    switch (*q++) {
    case 0:
        if (em->thirst > (s32)(0.3f * (f32)em->thirst_max)) {
            CMD_SKIP(em, q, 0x21);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x21);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_near_pos_ck(EMW *em, u8 *p) {
    u8 *q;
    u32 v;

    q = p;
    switch (*q++) {
    case 0:
        v = *q;
        q += 1;
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 100.0f * v)) {
            CMD_SKIP(em, q, 0x22);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x22);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_emtype_ck(EMW *em, u8 *p) {
    u8 *q;
    s8 found;
    s8 i;
    EMW *w;
    u8 kind;
    u8 type;

    w = em_work;
    q = p;
    switch (*q++) {
    case 0:
        kind = q[0];
        type = q[1];
        i = 0;
        found = 0;
        q += 2;
        for (; i < 20; i++, w++) {
            if (w->be_flag != 0 && w->kind == kind && (Pl_stg_ck_tw(em, (PLW *)w) & 0xFF) &&
                (w->type == type || type == 0xFF)) {
                found = 1;
                break;
            }
        }
        if (found == 0) {
            em->x944 = NULL;
            CMD_SKIP(em, q, 0x23);
        } else {
            em->x944 = w;
        }
        break;
    case 1:
        q = else_ck(em, q, 0x23);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_repeat_cnt_set(EMW *em, u8 *p) {
    u8 *q;
    s8 t;

    q = p;
    switch (*q++) {
    case 0:
        em->x84F = *q;
        q += 1;
        em->cmd_p858 = q;
        if (em->x84F <= 0) {
            for (;;) {
                if (q[0] == 0x24 && q[1] == 2) {
                    break;
                }
                q = cmd_end_search(em, q, 0x24, 1);
            }
            q = next_cmd_search(em, q);
            if (q[0] == 0x24 && q[1] == 1) {
                q = next_cmd_search(em, q);
            }
        }
        break;
    case 1:
        t = em->x84F - 1;
        em->x84F = t;
        if (t > 0) {
            q = em->cmd_p858;
        }
        break;
    }
    return q;
}

u8 *em_cmd_repeat_cnt_clr(EMW *em, u8 *p) {
    EM_FIELD(em, s8 *, 0x84F) = 0;
    return p;
}

u8 *em_cmd_demo_flag_set(EMW *em, u8 *p) {
    em->x8C2 = (u8) *p;
    return p + 1;
}

CMD_SEL_FUNC(em_cmd_type_sel, 0x27, u8, u8, em->type)

u8 *em_cmd_all_pl_same_stage_ck(EMW *em, u8 *p) {
    u8 *q;
    PLW *pl;
    s32 i;
    u8 ok;

    q = p;
    pl = player_work;
    switch (*q++) {
    case 0:
        ok = 1;
        i = 0;
        for (; i < *(u8 *)0x3F34C3; i++, pl++) {
            if (Pl_stg_ck_tw(em, pl) != 0 && pl->be_flag != 0) {
                ok = 0;
                break;
            }
        }
        if (ok) {
            if (ok) {
                for (;;) {
                    if (q[0] == 0x28 && q[1] == 1) {
                        break;
                    }
                    if (q[0] == 0x28 && q[1] == 2) {
                        break;
                    }
                    q = cmd_end_search(em, q, 0x28, 2);
                    if (!ok) {
                        break;
                    }
                }
            }
            q = next_cmd_search(em, q);
            if (q[0] == 0x28 && q[1] == 2) {
                q = next_cmd_search(em, q);
            }
        }
        break;
    case 1:
        q = else_ck(em, q, 0x28);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_stay_timer_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->stay_tm > 0) {
            CMD_SKIP(em, var_a1, 0x29);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x29);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_runaway_timer_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->runaway_tm > 0) {
            CMD_SKIP(em, var_a1, 0x2A);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x2A);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_flag_ck(EMW *em, u8 *p) {
    s32 var_s0;
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_a2;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;

    var_s0 = saved_reg_s0;
    var_a1 = p + 1;
    temp_v1 = EM_FIELD(p, u8 *, 0);
    switch (temp_v1) {                              /* switch 1; irregular */
    case 2:                                         /* switch 1 */
        break;
    case 0:                                         /* switch 1 */
        temp_v1_2 = EM_FIELD(p, u8 *, 1);
        temp_a2 = EM_FIELD(var_a1, u8 *, 1);
        var_a1 += 2;
        switch (temp_v1_2) {                        /* switch 2 */
        case 1:                                     /* switch 2 */
            var_s0 = 0;
            if (em->x88B == (temp_a2 & 0xFF)) {

            } else {
                var_s0 = 1 & 0xFF;
            }
            break;
        case 2:                                     /* switch 2 */
            var_s0 = 0;
            if (EM_FIELD(em, u8 *, 0x8C0) == (temp_a2 & 0xFF)) {

            } else {
                var_s0 = 1 & 0xFF;
            }
            break;
        case 3:                                     /* switch 2 */
            var_s0 = 0;
            if (em->x9E1 == (temp_a2 & 0xFF)) {

            } else {
                var_s0 = 1 & 0xFF;
            }
            break;
        case 4:                                     /* switch 2 */
            var_s0 = 0;
            if (em->x83A == (temp_a2 & 0xFF)) {

            } else {
                var_s0 = 1 & 0xFF;
            }
            break;
        case 5:                                     /* switch 2 */
            if (EM_FIELD(em, u8 *, 2) == 7) {
                var_s0 = 0;
                if (EM_FIELD(em, u8 *, 0x45C) == (temp_a2 & 0xFF)) {

                } else {
                    var_s0 = 1 & 0xFF;
                }
            }
            break;
        case 6:                                     /* switch 2 */
            if (EM_FIELD(em, u8 *, 2) == 0xF) {
                var_s0 = 0;
                if (EM_FIELD(em, u8 *, 0x488) == (temp_a2 & 0xFF)) {

                } else {
                    var_s0 = 1 & 0xFF;
                }
            }
            break;
        case 7:                                     /* switch 2 */
            if (EM_FIELD(em, u8 *, 2) == 0xF) {
                var_s0 = 0;
                if (EM_FIELD(em, u8 *, 0x489) == (temp_a2 & 0xFF)) {

                } else {
                    var_s0 = 1 & 0xFF;
                }
            }
            break;
        }
        if (var_s0 == 0) {

        } else {
loop_39:
            if (var_s0 != 0) {
                temp_a0 = EM_FIELD(var_a1, u8 *, 0);
                if ((temp_a0 != 0x2B) || (EM_FIELD(var_a1, u8 *, 1) != 1)) {
                    if (temp_a0 == 0x2B) {
                        if (EM_FIELD(var_a1, u8 *, 1) != 2) {
                            goto block_38;
                        }
                    } else {
block_38:
                        var_a1 = cmd_end_search(em, var_a1, 0x2B, 2);
                        goto loop_39;
                    }
                }
            }
            temp_v0 = next_cmd_search(em, var_a1);
            temp_v1_3 = EM_FIELD(temp_v0, u8 *, 0);
            var_a1 = temp_v0;
            if ((temp_v1_3 == 0x2B) && (EM_FIELD(var_a1, u8 *, 1) == 2)) {
                var_a1 = next_cmd_search(em, var_a1);
            }
        }
        break;
    case 1:                                         /* switch 1 */
        var_a1 = else_ck(em, var_a1, 0x2B);
        break;
    }
    return var_a1;
}

CMD_SEL_FUNC(em_cmd_myemtype_sel, 0x2C, u8, u8, em->kind)

u8 *em_cmd_smell_set(EMW *em, u8 *p) {
    EM_FIELD(em, s8 *, 0x827) = 7;
    em->x828 = (u8) em->x951;
    em->x829 = (u8) em->x952;
    return p;
}

u8 *em_cmd_search_data_set(EMW *em, u8 *p) {
    em_search_data_set(em, *p);
    return p + 1;
}

u8 *em_cmd_egg_ck(EMW *em, u8 *p) {
    s8 i;
    u16 found;
    u8 *q;
    u8 n;

    q = p;
    switch (*q++) {
    case 0:
        n = *(u8 *)0x3F34C3;
        found = 0;
        for (i = 0; i < n; i++) {
            if (player_work[i].work56B & 0xF) {
                found = 1;
                break;
            }
        }
        if (!found) {
            CMD_SKIP(em, q, 0x2F);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x2F);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_egg_cancel_ck(EMW *em, u8 *p) {
    s8 i;
    s32 found;
    u8 n;

    found = 0;
    n = *(u8 *)0x3F34C3;
    for (i = 0; i < n; i++) {
        if (player_work[i].work56B & 0xF) {
            found = 1;
            break;
        }
    }
    if (found != 0) {
        em->x9E5 = 1;
    }
    return p;
}

u8 *em_cmd_yobi_pos_set(EMW *em, u8 *p) {
    EM_FIELD(em, s8 *, 0x827) = 8;
    return p;
}

u8 *em_cmd_body_status_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    u8 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (EM_FIELD(em, u8 *, 0x762) != v) {
            CMD_SKIP(em, var_a1, 0x32);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x32);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_body_status_sel(EMW *em, u8 *p) {
    u8 *q;
    u8 n;
    s32 i;
    u16 more;

    q = p;
    switch (*q++) {
    case 0:
        n = *q;
        q += 3;
        for (i = 0; i < n; i++) {
            q += 1;
            if (*q != (u8)em->x762) {
                q = cmd_end_search(em, q, 0x33, 3);
                if (q[1] == 2 || q[1] == 3) {
                    q = q + 2;
                    break;
                }
                q = q + 2;
            } else {
                break;
            }
        }
        break;
    case 1:
        q += 1;
    case 2:
        more = 1;
        do {
            q = cmd_end_search(em, q, 0x33, 3);
            if (q[0] == 0x33 && q[1] == 3) {
                more = 0;
            }
            q = next_cmd_search(em, q);
        } while (more);
        break;
    case 3:
        break;
    }
    return q;
}

u8 *em_cmd_act_st_ck(EMW *em, u8 *p) {
    u8 *q;
    u8 a;
    u8 b;

    q = p;
    switch (*q++) {
    case 0:
        a = q[0];
        b = q[1];
        q += 2;
        if (em->mode == a && em->x15 == (b & 0xFF)) {
        } else {
            CMD_SKIP(em, q, 0x34);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x34);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_ikari_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x8B6 == 0) {
            CMD_SKIP(em, var_a1, 0x35);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x35);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_water_ck(EMW *em, u8 *p) {
    u8 *q;

    q = p;
    switch (*q++) {
    case 0:
        if (GetWaterData() == 0) {
            CMD_SKIP(em, q, 0x38);
        }
        break;
    case 1:
        q = else_ck(em, q, 0x38);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_eye_dmg_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x94E == 0) {
            CMD_SKIP(em, var_a1, 0x39);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x39);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_sensor_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x9E1 != 0) {
            CMD_SKIP(em, var_a1, 0x3A);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x3A);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_boss_work_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (EM_FIELD(em, s32 *, 0x9D4) == 0) {
            CMD_SKIP(em, var_a1, 0x3B);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x3B);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_boss_atk_ck(EMW *em, u8 *p) {
    u8 *q;
    EMW *b;
    u8 ok;

    q = p;
    switch (*q++) {
    case 0:
        b = em->boss;
        ok = 1;
        if (b == NULL || b->x888 != 1 || b->stg != em->stg) {
            ok = 1;
            if (ok) {
                for (;;) {
                    if (q[0] == 0x3C && q[1] == 1) {
                        break;
                    }
                    if (q[0] == 0x3C && q[1] == 2) {
                        break;
                    }
                    q = cmd_end_search(em, q, 0x3C, 2);
                    if (!ok) {
                        break;
                    }
                }
            }
            q = next_cmd_search(em, q);
            if (q[0] == 0x3C && q[1] == 2) {
                q = next_cmd_search(em, q);
            }
        }
        break;
    case 1:
        q = else_ck(em, q, 0x3C);
        break;
    case 2:
        break;
    }
    return q;
}

u8 *em_cmd_before_stage_ck(EMW *em, u8 *p) {
    u8 temp_a0;
    s32 var_s0;
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        temp_v1_2 = *var_a1;
        var_a1 += 1;
        if (temp_v1_2 == 0xFF) {
            var_s0 = 1 & 0xFF;
            goto loop_16;
        }
        var_s0 = 1 & 0xFF;
        if (em->x92E != temp_v1_2) {
loop_16:
            if (var_s0 != 0) {
                temp_a0 = EM_FIELD(var_a1, u8 *, 0);
                if ((temp_a0 != 0x3D) || (EM_FIELD(var_a1, u8 *, 1) != 1)) {
                    if (temp_a0 == 0x3D) {
                        if (EM_FIELD(var_a1, u8 *, 1) != 2) {
                            goto block_15;
                        }
                    } else {
block_15:
                        var_a1 = cmd_end_search(em, var_a1, 0x3D, 2);
                        goto loop_16;
                    }
                }
            }
            temp_v0 = next_cmd_search(em, var_a1);
            temp_v1_3 = EM_FIELD(temp_v0, u8 *, 0);
            var_a1 = temp_v0;
            if ((temp_v1_3 == 0x3D) && (EM_FIELD(var_a1, u8 *, 1) == 2)) {
                var_a1 = next_cmd_search(em, var_a1);
            }
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x3D);
        break;
    case 2:
        break;
    }
    return var_a1;
}

CMD_SEL_FUNC_W(em_cmd_before_stage_sel, 0x3E, u8, u8, em->x92E)

u8 *em_cmd_ground_area_move(EMW *em, u8 *p) {
    int spC0;
    int spAC;
    int spA0;
    f32 temp_f0;
    f32 var_f20;
    s16 *temp_s6_2;
    s16 *var_s0;
    s16 temp_a0;
    s32 temp_s7;
    s32 var_a0;
    s32 var_s1_3;
    s32 var_s2;
    int var_s0_2;
    int var_s1;
    int var_s1_2;
    int var_s2_2;
    int var_v1;
    u16 temp_a1;
    u8 temp_a2;
    u8 temp_s6;
    void *temp_v0;
    void *temp_v0_2;
    void *var_s3;

    var_s1 = saved_reg_s1;
    var_s3 = saved_reg_s3;
    temp_s7 = p + 2;
    em->x8BE = 0;
    temp_s6 = EM_FIELD(p, u8 *, 0);
    EM_FIELD(em, u8 *, 0x9F0) = (u8) EM_FIELD(p, u8 *, 1);
    var_s2 = 0;
    if ((s32) EM_FIELD(em, u8 *, 0x9F0) >= 0) {

    }
    temp_a2 = em->stg;
    if (temp_a2 == (temp_s6 & 0xFF)) {
        return temp_s7;
    }
    temp_v0 = gp_ck(em, EM_FIELD(em->area, s32 *, 0x18), temp_a2);
    var_s0 = EM_FIELD(temp_v0, s16 **, 4);
    if (var_s0 == NULL) {
        return temp_s7;
    }
    EM_FIELD(em, u16 *, 0x73A) = (u16) (temp_s6 & 0xFF);
    temp_a1 = EM_FIELD(em, u16 *, 0x73A);
    if (temp_a1 == 0xFF) {
        var_f20 = -1.0f;
        var_s2_2 = 0;
        if (EM_FIELD(temp_v0, s16 *, 2) > 0) {
loop_11:
            var_s3 = EM_FIELD(var_s0, void **, 4);
            if (var_s3 == NULL) {
                var_s0_2 = 0;
                em->x9DB = 7U;
            } else {
                temp_f0 = CalcDistanceXZ(em + 0xAC, var_s3);
                if (var_f20 == -1.0f) {
                    var_s1_2 = var_s2_2 << 0x30;
                    var_f20 = temp_f0;
                    goto block_18;
                }
                if (!(var_f20 <= temp_f0)) {
                    var_s1_2 = var_s2_2 << 0x30;
                    var_f20 = temp_f0;
block_18:
                    var_s1 = var_s1_2 >> 0x30;
                }
                var_s2_2 = (s16)(var_s2_2 + 1);
                var_s0 += 0x18;
                if (var_s2_2 >= EM_FIELD(temp_v0, s16 *, 2)) {
                    goto block_21;
                }
                goto loop_11;
            }
        } else {
block_21:
            var_v1 = (s16)(var_s1);
            goto block_31;
        }
    } else {
        temp_a0 = EM_FIELD(temp_v0, s16 *, 2);
        var_v1 = 0;
        if (temp_a0 > 0) {
loop_24:
            if (*var_s0 == temp_a1) {
                var_s2 = 1;
            } else {
                var_v1 = (s16)(var_v1 + 1);
                var_s0 += 0x18;
                if (var_v1 < temp_a0) {
                    goto loop_24;
                }
            }
        }
        if (var_s2 == 0) {
            var_s0_2 = 0;
            em->x9DB = 1U;
        } else {
block_31:
            em->x86D = (u8) var_v1;
            var_s1_3 = 0;
            temp_s6_2 = EM_FIELD(temp_v0, s16 **, 4);
            var_a0 = em->x86D * 0x18;
            temp_v0_2 = var_a0 + temp_s6_2;
            var_s3 = EM_FIELD(temp_v0_2, void **, 4);
            var_s0_2 = 0;
            if (EM_FIELD(temp_v0_2, s16 *, 2) > 0) {
loop_33:
                if (EM_FIELD(var_s3, u16 *, 0x10) != 0) {
                    if (*(u8 *)0x3F3404 != em->stg) {
                        var_s1_3 = 1;
                        var_s0_2 = (s16)(EM_FIELD((var_a0 + temp_s6_2), s16 *, 2) - 1);
                    } else {
                        SetVector(EM_FIELD(em, f32 *, 0xAC), EM_FIELD(em, f32 *, 0xB4), &spA0);
                        SetVector(EM_FIELD(var_s3, f32 *, 0), EM_FIELD(var_s3, f32 *, 8), &spAC);
                        if (GetWallHitLine(&spA0, &spAC, &spC0, em->x95E) == 0) {
                            var_s1_3 = 1;
                        } else {
                            goto block_40;
                        }
                    }
                } else {
block_40:
                    var_s0_2 = (s16)(var_s0_2 + 1);
                    var_a0 = em->x86D * 0x18;
                    var_s3 += 0x14;
                    if (var_s0_2 < EM_FIELD((temp_s6_2 + var_a0), s16 *, 2)) {
                        goto loop_33;
                    }
                }
            }
            if (var_s1_3 == 0) {
                var_s3 -= 0x14;
                em->x9DB = 2U;
                var_s0_2 = (s16)(EM_FIELD(((em->x86D * 0x18) + temp_s6_2), s16 *, 2) - 1);
            }
        }
    }
    if ((em->x9DB != 0) && (em->x8C3 != 0)) {
        em->x9DB = 0U;
        var_s0_2 = 0;
        em->x86D = 0U;
        em->x86E = 0U;
        var_s3 = EM_FIELD(((em->x86D * 0x18) + EM_FIELD(temp_v0, s16 **, 4)), void **, 4);
    }
    if (em->x9DB != 0) {
        em->x9DB = 0U;
    }
    em->x86E = (u8) var_s0_2;
    em->x827 = 9U;
    em->x828 = (u8) em->x86D;
    em->x829 = (u8) em->x86E;
    em->x881 = (u8) em->x827;
    em->x882 = (u8) em->x828;
    em->x883 = (u8) em->x829;
    EM_FIELD(em, s32 *, 0x868) = temp_s7;
    em->x86C = (u8) EM_FIELD(var_s3, u16 *, 0x12);
    return ground_area_move_ptr_set(em, em->x86C);
}

u8 *em_cmd_em_mode_change(EMW *em, u8 *p) {
    if (*p == 0) {
        Em_Mode_Chg(em, 0, 0);
    } else {
        Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
    }
    return p + 1;
}

u8 *em_cmd_em_hp_vital_add(EMW *em, u8 *p) {
    u8 v = *p;

    switch (em->kind) {
    case 0x1B:
    case 0x1C:
    case 0x1F:
        if (v == 0) {
            em_hp_add(em, (s32)(0.15f * (f32)em->x792));
        }
        break;
    }
    return p + 1;
}

u8 *em_cmd_horm_pos_ang_ck(EMW *em, u8 *p) {
    s32 var_a0;
    s32 var_s1;
    s8 temp_a2;
    u8 *temp_v0;
    u8 *temp_v0_2;
    u8 *var_s2;
    u8 *var_v0;
    u8 temp_a0;
    u8 temp_v1;
    u8 temp_v1_2;

    var_s2 = p;
    switch (*var_s2++) {
    case 0:
        temp_a2 = *(s8 *)var_s2;
        var_s1 = 0;
        var_s2 += 1;
        var_a0 = ((Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF) - em->ang[1]) & 0xFFFF;
        if (var_a0 >= 0x8001) {
            var_a0 = (0x10000 - var_a0) & 0xFFFF;
        }
        if ((var_a0 & 0xFFFF) < ((s32)(0.5f + ((65536.0f * (f32) temp_a2) / 360.0f)) & 0xFFFF & 0xFFFF)) {
            var_s1 = 1;
        }
        if (var_s1 == 0) {
            if ((EM_FIELD(var_s2, u8 *, 0) == 0x42) && (EM_FIELD(var_s2, u8 *, 1) == 1)) {
                temp_v0 = next_cmd_search(em, var_s2);
                if ((EM_FIELD(temp_v0, u8 *, 0) == 0x42) && (EM_FIELD(temp_v0, u8 *, 1) == 2)) {
                    var_v0 = next_cmd_search(em, temp_v0);
                } else {
                    var_v0 = next_cmd_search(em, cmd_end_search(em, temp_v0, 0x42, 2));
                }
                goto block_27;
            }
        } else {
loop_16:
            temp_a0 = EM_FIELD(var_s2, u8 *, 0);
            if ((temp_a0 != 0x42) || (EM_FIELD(var_s2, u8 *, 1) != 1)) {
                if (temp_a0 == 0x42) {
                    if (EM_FIELD(var_s2, u8 *, 1) != 2) {
                        goto block_21;
                    }
                } else {
block_21:
                    var_s2 = cmd_end_search(em, var_s2, 0x42, 2);
                    goto loop_16;
                }
            }
            temp_v0_2 = next_cmd_search(em, var_s2);
            temp_v1_2 = EM_FIELD(temp_v0_2, u8 *, 0);
            var_s2 = temp_v0_2;
            if ((temp_v1_2 == 0x42) && (EM_FIELD(var_s2, u8 *, 1) == 2)) {
                var_v0 = next_cmd_search(em, var_s2);
block_27:
                var_s2 = var_v0;
            }
        }
        break;
    case 1:
        var_v0 = else_ck(em, var_s2, 0x42);
        goto block_27;
    case 2:
        break;
    }
    return var_s2;
}

u8 *em_cmd_boss_same_stage_ck(EMW *em, u8 *p) {
    s32 var_s0;
    u8 *temp_v0_2;
    u8 *var_a1;
    u8 temp_v1;
    u8 temp_v1_2;
    void *temp_v0;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        temp_v0 = em->boss;
        var_s0 = 1 & 0xFF;
        if (temp_v0 == NULL) {
            goto loop_16;
        }
        var_s0 = 1 & 0xFF;
        if (em->stg != EM_FIELD(temp_v0, u8 *, 0x736)) {
loop_16:
            if (var_s0 != 0) {
                temp_a0 = EM_FIELD(var_a1, u8 *, 0);
                if ((temp_a0 != 0x44) || (EM_FIELD(var_a1, u8 *, 1) != 1)) {
                    if (temp_a0 == 0x44) {
                        if (EM_FIELD(var_a1, u8 *, 1) != 2) {
                            goto block_15;
                        }
                    } else {
block_15:
                        var_a1 = cmd_end_search(em, var_a1, 0x44, 2);
                        goto loop_16;
                    }
                }
            }
            temp_v0_2 = next_cmd_search(em, var_a1);
            temp_v1_2 = EM_FIELD(temp_v0_2, u8 *, 0);
            var_a1 = temp_v0_2;
            if ((temp_v1_2 == 0x44) && (EM_FIELD(var_a1, u8 *, 1) == 2)) {
                var_a1 = next_cmd_search(em, var_a1);
            }
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x44);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_pl_fishing_ck(EMW *em, u8 *p) {
    s8 sp8C;
    s32 var_s6;
    int temp_v0;
    int var_s1;
    s8 *var_s0;
    s8 var_s2;
    u8 *temp_v0_2;
    u8 *var_s3;
    u8 *var_s4;
    u8 *var_v0;
    u8 temp_a0;
    u8 temp_v1;
    u8 temp_v1_2;

    var_s4 = p + 1;
    temp_v1 = *p;
    var_s3 = &player_work;
    switch (temp_v1) {                              /* irregular */
    case 2:
        break;
    case 0:
        var_s2 = 0;
        var_s1 = 0;
        var_s6 = 0;
        var_s0 = &sp8C;
        do {
            if ((*var_s3 != 0) && (Pl_stg_ck_tw(em, var_s3) != 0) && (pl_flag_ck(var_s3, 0x80000) != 0)) {
                *var_s0 = var_s2;
                var_s0 += 1;
                var_s1 = (s8)(var_s1 + 1);
                var_s6 = 1;
            }
            var_s2 = (s8) ((s8)(var_s2 + 1));
            var_s3 += 0xA00;
        } while (var_s2 < 4);
        if (var_s6 != 0) {
            temp_v0 = (s8)(var_s1);
            if (temp_v0 == 0) {
                M2C_BREAK(0);
            }
            EM_FIELD(em, u8 *, 0x844) = (u8) EM_FIELD((((s32) em->x39A % temp_v0) + sp), u8 *, 0x8C);
        } else {
loop_16:
            temp_a0 = EM_FIELD(var_s4, u8 *, 0);
            if ((temp_a0 != 0x45) || (EM_FIELD(var_s4, u8 *, 1) != 1)) {
                if (temp_a0 == 0x45) {
                    if (EM_FIELD(var_s4, u8 *, 1) != 2) {
                        goto block_21;
                    }
                } else {
block_21:
                    var_s4 = cmd_end_search(em, var_s4, 0x45, 2);
                    goto loop_16;
                }
            }
            temp_v0_2 = next_cmd_search(em, var_s4);
            temp_v1_2 = EM_FIELD(temp_v0_2, u8 *, 0);
            var_s4 = temp_v0_2;
            if ((temp_v1_2 == 0x45) && (EM_FIELD(var_s4, u8 *, 1) == 2)) {
                var_v0 = next_cmd_search(em, var_s4);
block_27:
                var_s4 = var_v0;
            }
        }
        break;
    case 1:
        var_v0 = else_ck(var_s4, 0x45);
        goto block_27;
    }
    return var_s4;
}

u8 *em_cmd_target_pl_act_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_s0;
    u8 *var_v0;
    u8 temp_a0;
    u8 temp_a2;
    u8 temp_v1;
    u8 temp_v1_2;

    temp_v1 = EM_FIELD(p, u8 *, 0);
    var_s0 = p + 1;
    switch (temp_v1) {                              /* irregular */
    case 2:
        break;
    case 0:
        temp_a2 = EM_FIELD(var_s0, u8 *, 1);
        var_s0 += 2;
        if (((s16)(act_ck(&player_work + ((em->x844 & 0xF) * 0xA00), EM_FIELD(p, u8 *, 1), temp_a2))) == 0) {
loop_5:
            temp_a0 = EM_FIELD(var_s0, u8 *, 0);
            if ((temp_a0 != 0x46) || (EM_FIELD(var_s0, u8 *, 1) != 1)) {
                if (temp_a0 == 0x46) {
                    if (EM_FIELD(var_s0, u8 *, 1) != 2) {
                        goto block_10;
                    }
                } else {
block_10:
                    var_s0 = cmd_end_search(em, var_s0, 0x46, 2);
                    goto loop_5;
                }
            }
            temp_v0 = next_cmd_search(em, var_s0);
            temp_v1_2 = EM_FIELD(temp_v0, u8 *, 0);
            var_s0 = temp_v0;
            if ((temp_v1_2 == 0x46) && (EM_FIELD(var_s0, u8 *, 1) == 2)) {
                var_v0 = next_cmd_search(em, var_s0);
block_16:
                var_s0 = var_v0;
            }
        }
        break;
    case 1:
        var_v0 = else_ck(var_s0, 0x46);
        goto block_16;
    }
    return var_s0;
}

u8 *em_cmd_fish_ok_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x8B9 != 0) {
            em->x8B9 = 0U;
        } else {
            CMD_SKIP(em, var_a1, 0x47);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x47);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_timer_set(EMW *em, u8 *p) {
    EM_FIELD(em, s32 *, 8) = (s32) (*p * 0x1E);
    return p + 1;
}

u8 *em_cmd_pl_land_target(EMW *em, u8 *p) {
    u8 v = *p;

    if (em->x844 == -1) {
        em->x827 = 1;
        em->x828 = 0;
        em->x829 = 0xFF;
    } else {
        em->x827 = 0xB;
        em->x828 = (u8) (em->x844 & 0xF);
        em->x829 = v;
    }
    em->x881 = (u8) em->x827;
    em->x882 = (u8) em->x828;
    em->x883 = (u8) em->x829;
    return p + 1;
}

u8 *em_cmd_pl_look_ck(EMW *em, u8 *p) {
    s8 temp_a0;
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_v1;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        temp_a0 = em->x844;
        if (em->x88C & (1 << (temp_a0 & 0xF))) {
            if (temp_a0 == -1) {
                goto loop_7;
            }
        } else {
            CMD_SKIP(em, var_a1, 0x4A);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x4A);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_kehai_clear(EMW *em, u8 *p) {
    if (em->x844 != -1) {
        em->x8F4[em->x844 & 0xF] = 0;
    }
    return p;
}

u8 *em_cmd_hate_clear(EMW *em, u8 *p) {
    if (em->x844 != -1) {
        em->x918[em->x844 & 0xF] = 0;
    }
    return p;
}

u8 *em_cmd_horm_pos_set(EMW *em, u8 *p) {
    cmd_target_kind_set(em + 0x934);
    return p;
}

u8 *em_cmd_thirst_add(EMW *em, u8 *p) {
    s32 temp_v0;
    s32 var_a1;

    if (*p != 0) {

    } else {
        temp_v0 = em->thirst_max;
        var_a1 = temp_v0 >> 1;
        if (temp_v0 < 0) {
            var_a1 = (s32) (temp_v0 + 1) >> 1;
        }
        em_thirst_add(var_a1);
    }
    return p + 1;
}

u8 *em_cmd_hungry_add(EMW *em, u8 *p) {
    s32 temp_v0;
    s32 var_a1;

    if (*p != 0) {

    } else {
        temp_v0 = em->hungry_max;
        var_a1 = temp_v0 >> 1;
        if (temp_v0 < 0) {
            var_a1 = (s32) (temp_v0 + 1) >> 1;
        }
        em_hungry_add(var_a1);
    }
    return p + 1;
}

u8 *em_cmd_suimin_add(EMW *em, u8 *p) {
    s32 temp_v0;
    s32 var_a1;

    if (*p != 0) {

    } else {
        temp_v0 = em->x8AC;
        var_a1 = temp_v0 >> 1;
        if (temp_v0 < 0) {
            var_a1 = (s32) (temp_v0 + 1) >> 1;
        }
        em_suimin_add(var_a1);
    }
    return p + 1;
}

u8 *em_cmd_swim_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x388 != 4) {
            CMD_SKIP(em, var_a1, 0x51);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x51);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_all_pl_target_sel(EMW *em, u8 *p) {
    u8 spD;
    u8 spC;
    s32 var_a2;
    s32 var_a3_2;
    s32 var_t0;
    s32 var_t0_2;
    s32 var_t0_3;
    s32 var_t1_4;
    s32 var_t3_4;
    s32 var_v1;
    u8 *var_t1;
    u8 *var_t1_2;
    u8 *var_t2;
    u8 *var_t2_2;
    u8 *var_t2_3;
    u8 *var_t2_4;
    u8 *var_t3_3;
    u8 temp_a2;
    u8 temp_a3;
    u8 temp_a3_2;
    u8 temp_t0;
    u8 temp_t1;
    u8 temp_t1_2;
    u8 temp_t2;
    u8 var_a3;
    u8 var_a3_3;
    u8 var_t1_3;
    u8 var_t4;
    u8 var_v0;
    void *var_t3;
    void *var_t3_2;

    if (em->x88F != 0) {
        var_t4 = 0;
        temp_t1 = *(u8 *)0x3F34C3;
        var_v1 = 0;
        if ((s32) temp_t1 > 0) {
            var_t3 = em;
            var_t2 = &spC;
            do {
                if ((EM_FIELD(var_t3, s32 *, 0x918) >= 0xC350) && (em->x88F & (1 << var_t4))) {
                    *var_t2 = var_t4;
                    var_v1 += 1;
                    var_t2 += 1;
                }
                var_t4 += 1;
                var_t3 += 4;
            } while ((s32) var_t4 < (s32) temp_t1);
        }
        if (var_v1 >= 2) {
            var_v0 = spC;
            var_t0 = 1;
            if (var_v1 >= 2) {
                var_t1 = &spD;
                do {
                    temp_a3 = *var_t1;
                    if (EM_FIELD((em + ((var_v0 & 0xFF) * 4)), f32 *, 0x8C4) < EM_FIELD((em + (temp_a3 * 4)), f32 *, 0x8C4)) {
                        var_v0 = temp_a3;
                    }
                    var_t0 += 1;
                    var_t1 += 1;
                } while (var_t0 < var_v1);
            }
        } else if (var_v1 == 1) {
            var_v0 = spC;
        } else {
            temp_t1_2 = *(void *)0x3F34C3;
            var_a3 = 0;
            var_t0_2 = 0;
            if ((s32) temp_t1_2 > 0) {
                var_t3_2 = em;
                var_t2_2 = &spC;
                do {
                    if ((EM_FIELD(var_t3_2, s32 *, 0x918) >= 0x7530) && (em->x88F & (1 << var_a3))) {
                        *var_t2_2 = var_a3;
                        var_t0_2 += 1;
                        var_t2_2 += 1;
                    }
                    var_a3 += 1;
                    var_t3_2 += 4;
                } while ((s32) var_a3 < (s32) temp_t1_2);
            }
            var_v0 = 0xFF;
            if (var_t0_2 >= 2) {
                var_v0 = spC;
                var_a3_2 = 1;
                if (var_t0_2 >= 2) {
                    var_t1_2 = &spD;
                    do {
                        temp_a2 = *var_t1_2;
                        if (EM_FIELD((em + ((var_v0 & 0xFF) * 4)), f32 *, 0x8C4) < EM_FIELD((em + (temp_a2 * 4)), f32 *, 0x8C4)) {
                            var_v0 = temp_a2;
                        }
                        var_a3_2 += 1;
                        var_t1_2 += 1;
                    } while (var_a3_2 < var_t0_2);
                }
            } else if (var_t0_2 == 1) {
                var_v0 = spC;
            } else {
                temp_t2 = *(void *)0x3F34C3;
                var_t1_3 = 0;
                var_t0_3 = 0;
                if ((s32) temp_t2 > 0) {
                    var_t3_3 = &spC;
                    do {
                        if (em->x88F & (1 << var_t1_3)) {
                            *var_t3_3 = var_t1_3;
                            var_t0_3 += 1;
                            var_t3_3 += 1;
                        }
                        var_t1_3 += 1;
                    } while ((s32) var_t1_3 < (s32) temp_t2);
                }
                if (var_t0_3 == 1) {
                    var_v0 = spC;
                } else if (var_t0_3 != 0) {
                    var_v0 = spC;
                    var_t3_4 = 0;
                    if (EM_FIELD(((var_v0 * 4) + em), s32 *, 0x918) == 0) {
                        var_t3_4 = 1 & 0xFF;
                    }
                    var_t1_4 = 1;
                    if (var_t0_3 >= 2) {
                        var_t2_3 = &spD;
                        do {
                            temp_a3_2 = *var_t2_3;
                            if (EM_FIELD((em + ((var_v0 & 0xFF) * 4)), s32 *, 0x918) < EM_FIELD((em + (temp_a3_2 * 4)), s32 *, 0x918)) {
                                var_v0 = temp_a3_2;
                                var_t3_4 = 0;
                            }
                            var_t1_4 += 1;
                            var_t2_3 += 1;
                        } while (var_t1_4 < var_t0_3);
                    }
                    if ((var_t3_4 & 0xFF) == 1) {
                        var_v0 = 0xFF;
                    }
                }
            }
        }
        if ((var_v0 & 0xFF) == 0xFF) {
            var_a3_3 = 0;
            temp_t0 = *(void *)0x3F34C3;
            var_a2 = 0;
            if ((s32) temp_t0 > 0) {
                var_t2_4 = &spC;
                do {
                    if (em->x88F & (1 << var_a3_3)) {
                        *var_t2_4 = var_a3_3;
                        var_a2 += 1;
                        var_t2_4 += 1;
                    }
                    var_a3_3 += 1;
                } while ((s32) var_a3_3 < (s32) temp_t0);
            }
            if (var_a2 != 0) {
                if (var_a2 == 0) {
                    M2C_BREAK(0);
                }
                var_v0 = EM_FIELD((((s32) em->x39A % var_a2) + sp), u8 *, 0xC);
            } else {
                var_v0 = 0;
            }
        }
    } else {
        var_v0 = -1U;
    }
    EM_FIELD(em, u8 *, 0x844) = var_v0;
    return p;
}

u8 *em_cmd_samestage_pl_target_sel(EMW *em, u8 *p) {
    u8 sp7D;
    u8 sp7C;
    s32 temp_s2;
    s32 temp_v0;
    s32 var_a0;
    s32 var_a1;
    s32 var_a1_2;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a3_4;
    s32 var_s4;
    s32 var_s5;
    s32 var_t0_2;
    s32 var_t2_2;
    u8 *var_a3_2;
    u8 *var_a3_3;
    u8 *var_s3;
    u8 *var_t0;
    u8 *var_t0_3;
    u8 *var_t1;
    u8 *var_t1_3;
    u8 *var_t1_4;
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_a2;
    u8 temp_a3;
    u8 temp_a3_2;
    u8 temp_t0;
    u8 var_a0_2;
    u8 var_a2_3;
    u8 var_a2_4;
    u8 var_a3;
    u8 var_v1;
    void *var_t1_2;
    void *var_t2;

    if (em->x88F != 0) {
        var_s4 = 0;
        var_s5 = 0;
        if ((s32) *(u8 *)0x3F34C3 > 0) {
            var_s3 = &player_work;
            do {
                temp_s2 = 1 << var_s5;
                if ((em->x88F & temp_s2) && (Pl_stg_ck_tw(em, var_s3) != 0) && (*var_s3 != 0)) {
                    var_s4 = (var_s4 | (temp_s2 & 0xFF)) & 0xFF;
                }
                var_s5 += 1;
                var_s3 += 0xA00;
            } while (var_s5 < (s32) *(void *)0x3F34C3);
        }
        temp_v0 = var_s4 & 0xFF;
        if (temp_v0 == 0) {
            EM_FIELD(em, u8 *, 0x844) = -1U;
            return p;
        }
        var_a3 = 0;
        var_a0 = 0;
        if ((s32) *(void *)0x3F34C3 > 0) {
            var_t2 = em;
            var_t1 = &sp7C;
            do {
                if ((EM_FIELD(var_t2, s32 *, 0x918) >= 0xC350) && (temp_v0 & (1 << var_a3))) {
                    *var_t1 = var_a3;
                    var_a0 += 1;
                    var_t1 += 1;
                }
                var_a3 += 1;
                var_t2 += 4;
            } while ((s32) var_a3 < (s32) *(void *)0x3F34C3);
        }
        if (var_a0 >= 2) {
            var_v1 = sp7C;
            var_a2 = 1;
            if (var_a0 >= 2) {
                var_a3_2 = &sp7D;
                do {
                    temp_a1 = *var_a3_2;
                    if (EM_FIELD((em + ((var_v1 & 0xFF) * 4)), f32 *, 0x8C4) < EM_FIELD((em + (temp_a1 * 4)), f32 *, 0x8C4)) {
                        var_v1 = temp_a1;
                    }
                    var_a2 += 1;
                    var_a3_2 += 1;
                } while (var_a2 < var_a0);
            }
        } else if (var_a0 == 1) {
            var_v1 = sp7C;
        } else {
            temp_a3 = *(void *)0x3F34C3;
            var_a0_2 = 0;
            var_a2_2 = 0;
            if ((s32) temp_a3 > 0) {
                var_t1_2 = em;
                var_t0 = &sp7C;
                do {
                    if ((EM_FIELD(var_t1_2, s32 *, 0x918) >= 0x7530) && (temp_v0 & (1 << var_a0_2))) {
                        *var_t0 = var_a0_2;
                        var_a2_2 += 1;
                        var_t0 += 1;
                    }
                    var_a0_2 += 1;
                    var_t1_2 += 4;
                } while ((s32) var_a0_2 < (s32) temp_a3);
            }
            var_v1 = 0xFF;
            if (var_a2_2 >= 2) {
                var_v1 = sp7C;
                var_a1 = 1;
                if (var_a2_2 >= 2) {
                    var_a3_3 = &sp7D;
                    do {
                        temp_a0 = *var_a3_3;
                        if (EM_FIELD((em + ((var_v1 & 0xFF) * 4)), f32 *, 0x8C4) < EM_FIELD((em + (temp_a0 * 4)), f32 *, 0x8C4)) {
                            var_v1 = temp_a0;
                        }
                        var_a1 += 1;
                        var_a3_3 += 1;
                    } while (var_a1 < var_a2_2);
                }
            } else if (var_a2_2 == 1) {
                var_v1 = sp7C;
            } else {
                temp_t0 = *(void *)0x3F34C3;
                var_a2_3 = 0;
                var_a3_4 = 0;
                if ((s32) temp_t0 > 0) {
                    var_t1_3 = &sp7C;
                    do {
                        if (temp_v0 & (1 << var_a2_3)) {
                            *var_t1_3 = var_a2_3;
                            var_a3_4 += 1;
                            var_t1_3 += 1;
                        }
                        var_a2_3 += 1;
                    } while ((s32) var_a2_3 < (s32) temp_t0);
                }
                if (var_a3_4 == 1) {
                    var_v1 = sp7C;
                } else if (var_a3_4 != 0) {
                    var_v1 = sp7C;
                    var_t2_2 = 0;
                    if (EM_FIELD(((var_v1 * 4) + em), s32 *, 0x918) == 0) {
                        var_t2_2 = 1 & 0xFF;
                    }
                    var_t0_2 = 1;
                    if (var_a3_4 >= 2) {
                        var_t1_4 = &sp7D;
                        do {
                            temp_a2 = *var_t1_4;
                            if (EM_FIELD((em + ((var_v1 & 0xFF) * 4)), s32 *, 0x918) < EM_FIELD((em + (temp_a2 * 4)), s32 *, 0x918)) {
                                var_v1 = temp_a2;
                                var_t2_2 = 0;
                            }
                            var_t0_2 += 1;
                            var_t1_4 += 1;
                        } while (var_t0_2 < var_a3_4);
                    }
                    if ((var_t2_2 & 0xFF) == 1) {
                        var_a2_4 = 0;
                        temp_a3_2 = *(void *)0x3F34C3;
                        var_a1_2 = 0;
                        if ((s32) temp_a3_2 > 0) {
                            var_t0_3 = &sp7C;
                            do {
                                if (temp_v0 & (1 << var_a2_4)) {
                                    *var_t0_3 = var_a2_4;
                                    var_a1_2 += 1;
                                    var_t0_3 += 1;
                                }
                                var_a2_4 += 1;
                            } while ((s32) var_a2_4 < (s32) temp_a3_2);
                        }
                        if (var_a1_2 != 0) {
                            if (var_a1_2 == 0) {
                                M2C_BREAK(0);
                            }
                            var_v1 = EM_FIELD((((s32) em->x39A % var_a1_2) + sp), u8 *, 0x7C);
                        } else {
                            var_v1 = 0xFF;
                        }
                    }
                }
            }
        }
        EM_FIELD(em, u8 *, 0x844) = var_v1;
        goto block_71;
    }
    EM_FIELD(em, u8 *, 0x844) = -1U;
block_71:
    return p;
}

u8 *em_cmd_target_pl_samestage_ck(EMW *em, u8 *p) {
    s8 temp_a1;
    u8 *temp_s2;
    u8 *temp_v0;
    u8 *var_s0;
    u8 *var_v0;
    u8 temp_a0;
    u8 temp_v1;
    u8 temp_v1_2;

    temp_v1 = *p;
    var_s0 = p + 1;
    switch (temp_v1) {                              /* irregular */
    case 2:
        break;
    case 0:
        temp_a1 = em->x844;
        if ((temp_a1 == -1) || (temp_s2 = &player_work + (temp_a1 * 0xA00), (Pl_stg_ck_tw(temp_s2) == 0)) || (*temp_s2 == 0)) {
loop_8:
            temp_a0 = EM_FIELD(var_s0, u8 *, 0);
            if ((temp_a0 != 0x54) || (EM_FIELD(var_s0, u8 *, 1) != 1)) {
                if (temp_a0 == 0x54) {
                    if (EM_FIELD(var_s0, u8 *, 1) != 2) {
                        goto block_13;
                    }
                } else {
block_13:
                    var_s0 = cmd_end_search(em, var_s0, 0x54, 2);
                    goto loop_8;
                }
            }
            temp_v0 = next_cmd_search(em, var_s0);
            temp_v1_2 = EM_FIELD(temp_v0, u8 *, 0);
            var_s0 = temp_v0;
            if ((temp_v1_2 == 0x54) && (EM_FIELD(var_s0, u8 *, 1) == 2)) {
                var_v0 = next_cmd_search(em, var_s0);
block_19:
                var_s0 = var_v0;
            }
        }
        break;
    case 1:
        var_v0 = else_ck(var_s0, 0x54);
        goto block_19;
    }
    return var_s0;
}

u8 *em_cmd_target_pl_hate_high_ck(EMW *em, u8 *p) {
    s8 temp_v1_2;
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_v1;
    u8 temp_v1_3;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        temp_v1_2 = em->x844;
        if (temp_v1_2 != -1) {
            if (EM_FIELD(((temp_v1_2 * 4) + em), s32 *, 0x918) < 0x7530) {
                goto loop_7;
            }
        } else {
            CMD_SKIP(em, var_a1, 0x55);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x55);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_quest_no_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    s16 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (*(s16 *)0x3C7448 != v) {
            CMD_SKIP(em, var_a1, 0x56);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x56);
        break;
    case 2:
        break;
    }
    return var_a1;
}

CMD_SEL_FUNC(em_cmd_quest_no_sel, 0x57, s16, s16, *(s16 *)0x3C7448)

u8 *em_cmd_boss_pl_target_set(EMW *em, u8 *p) {
    EMW *b = em->boss;
    em->x827 = 1;
    em->x828 = 0;
    if (b == NULL) {
        em->x829 = 0xFF;
    } else {
        em->x829 = b->x617;
    }
    em->x844 = em->x829;
    return p;
}

u8 *em_cmd_tenjo_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (EM_FIELD(em, s8 *, 0x9EF) == 0) {
            CMD_SKIP(em, var_a1, 0x59);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x59);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_target_land_no_ck(EMW *em, u8 *p) {
    s8 temp_v1_2;
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_v1;
    u8 temp_v1_3;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        var_a1 += 1;
        if ((em->x881 == 0xB) && (temp_v1_2 = em->x844, (temp_v1_2 != -1))) {
            if ((EM_FIELD(p, u8 *, 1) & 0xFF) != EM_FIELD((&player_work + ((temp_v1_2 & 0xF) * 0xA00)), u8 *, 0x70E)) {
                goto loop_8;
            }
        } else {
            CMD_SKIP(em, var_a1, 0x5A);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x5A);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_ninshiki_timer_sub(EMW *em, u8 *p) {
    int var_t1;
    void *var_t0;

    var_t1 = 0;
    if ((s32) *(u8 *)0x3F34C3 > 0) {
        var_t0 = em;
        do {
            if (*p & 0xFF) {

            } else if (!(em->x88C & (1 << ((s8)(var_t1))))) {
                EM_FIELD(var_t0, s16 *, 0x890) = 0;
            }
            var_t1 = (s8)(var_t1 + 1);
            var_t0 += 2;
        } while (var_t1 < (s32) *(void *)0x3F34C3);
    }
    return p + 1;
}

u8 *em_cmd_tenjostage_ck(EMW *em, u8 *p) {
    int sp3C;
    int sp38;
    u8 *temp_v0;
    u8 *var_s0;
    u8 *var_v0;
    u8 temp_a0;
    u8 temp_v1;
    u8 temp_v1_2;

    temp_v1 = *p;
    var_s0 = p + 1;
    switch (temp_v1) {                              /* irregular */
    case 2:
        break;
    case 0:
        if ((em->stg != *(u8 *)0x3F3404) || (GetTenjoHit(em + 0xAC, &sp3C, &sp38) == 0)) {
loop_6:
            temp_a0 = EM_FIELD(var_s0, u8 *, 0);
            if ((temp_a0 != 0x5C) || (EM_FIELD(var_s0, u8 *, 1) != 1)) {
                if (temp_a0 == 0x5C) {
                    if (EM_FIELD(var_s0, u8 *, 1) != 2) {
                        goto block_11;
                    }
                } else {
block_11:
                    var_s0 = cmd_end_search(em, var_s0, 0x5C, 2);
                    goto loop_6;
                }
            }
            temp_v0 = next_cmd_search(em, var_s0);
            temp_v1_2 = EM_FIELD(temp_v0, u8 *, 0);
            var_s0 = temp_v0;
            if ((temp_v1_2 == 0x5C) && (EM_FIELD(var_s0, u8 *, 1) == 2)) {
                var_v0 = next_cmd_search(em, var_s0);
block_17:
                var_s0 = var_v0;
            }
        }
        break;
    case 1:
        var_v0 = else_ck(var_s0, 0x5C);
        goto block_17;
    }
    return var_s0;
}

u8 *em_cmd_smell_set_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_s0;
    u8 *var_v0;
    u8 temp_a0;
    u8 temp_v1;
    u8 temp_v1_2;

    temp_v1 = *p;
    var_s0 = p + 1;
    switch (temp_v1) {                              /* irregular */
    case 2:
        break;
    case 0:
        cmd_target_kind_set(em + 0x934);
        if ((EM_FIELD(em, f32 *, 0x934) == 0.0f) || (EM_FIELD(em, f32 *, 0x938) == 0.0f) || (EM_FIELD(em, f32 *, 0x93C) == 0.0f)) {
loop_7:
            temp_a0 = EM_FIELD(var_s0, u8 *, 0);
            if ((temp_a0 != 0x5D) || (EM_FIELD(var_s0, u8 *, 1) != 1)) {
                if (temp_a0 == 0x5D) {
                    if (EM_FIELD(var_s0, u8 *, 1) != 2) {
                        goto block_12;
                    }
                } else {
block_12:
                    var_s0 = cmd_end_search(em, var_s0, 0x5D, 2);
                    goto loop_7;
                }
            }
            temp_v0 = next_cmd_search(em, var_s0);
            temp_v1_2 = EM_FIELD(temp_v0, u8 *, 0);
            var_s0 = temp_v0;
            if ((temp_v1_2 == 0x5D) && (EM_FIELD(var_s0, u8 *, 1) == 2)) {
                var_v0 = next_cmd_search(em, var_s0);
block_18:
                var_s0 = var_v0;
            }
        }
        break;
    case 1:
        var_v0 = else_ck(var_s0, 0x5D);
        goto block_18;
    }
    return var_s0;
}

u8 *em_cmd_my_floor_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    u8 v;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        v = *var_a1;
        var_a1 += 1;
        if (em->x70E != v) {
            CMD_SKIP(em, var_a1, 0x5E);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x5E);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_st25_pl_target_sel(EMW *em, u8 *p) {
    u8 sp7D;
    u8 sp7C;
    s32 temp_a2;
    s32 temp_a3;
    s32 temp_s4;
    s32 var_a1;
    s32 var_a1_2;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a2_3;
    s32 var_a2_4;
    s32 var_a3_5;
    s32 var_s0;
    s32 var_s1;
    s32 var_v1;
    u16 temp_a0;
    u16 temp_v1;
    u16 temp_v1_2;
    u8 *var_a3;
    u8 *var_a3_3;
    u8 *var_a3_4;
    u8 *var_s5;
    u8 *var_t0;
    u8 *var_t0_2;
    u8 *var_t0_3;
    u8 *var_t1;
    u8 *var_t1_3;
    u8 temp_a0_2;
    u8 temp_a1;
    u8 temp_a1_2;
    u8 temp_a2_2;
    u8 temp_a3_2;
    u8 var_a1_3;
    u8 var_a3_2;
    u8 var_v0;
    u8 var_v1_2;
    void *var_t1_2;
    void *var_t2;

    if (em->x88F != 0) {
        var_s0 = 0;
        var_s1 = 0;
        if ((s32) *(u8 *)0x3F34C3 > 0) {
            var_s5 = &player_work;
            do {
                temp_s4 = 1 << var_s1;
                if ((em->x88F & temp_s4) && (Pl_stg_ck_tw(em, var_s5) != 0) && (*var_s5 != 0)) {
                    var_s0 = (var_s0 | (temp_s4 & 0xFF)) & 0xFF;
                }
                var_s1 += 1;
                var_s5 += 0xA00;
            } while (var_s1 < (s32) *(void *)0x3F34C3);
        }
        if (!(var_s0 & 0xFF) || (temp_v1 = em->x70E, (temp_v1 == 0))) {
            EM_FIELD(em, u8 *, 0x844) = -1U;
            return p;
        }
        if (((s32) temp_v1 > 0) && ((s32) temp_v1 < 4)) {
            var_a1 = 0;
            if ((s32) *(void *)0x3F34C3 > 0) {
                var_a3 = &player_work;
                do {
                    temp_a2 = 1 << var_a1;
                    if (!(var_s0 & 0xFF & temp_a2) || (temp_v1_2 = EM_FIELD(var_a3, u16 *, 0x70E), ((s32) temp_v1_2 <= 0)) || ((s32) temp_v1_2 >= 4)) {
                        var_s0 = var_s0 & (~temp_a2 & 0xFF) & 0xFF;
                    }
                    var_a1 += 1;
                    var_a3 += 0xA00;
                } while (var_a1 < (s32) *(void *)0x3F34C3);
            }
        } else {
            var_a2 = 0;
            if ((s32) *(void *)0x3F34C3 > 0) {
                var_t0 = &player_work;
                do {
                    temp_a3 = 1 << var_a2;
                    if ((var_s0 & 0xFF & temp_a3) && (temp_a0 = EM_FIELD(var_t0, u16 *, 0x70E), (((s32) temp_a0 < 4) == 0))) {
                        if ((s32) temp_a0 >= 0xB) {
                            goto block_27;
                        }
                    } else {
block_27:
                        var_s0 = var_s0 & (~temp_a3 & 0xFF) & 0xFF;
                    }
                    var_a2 += 1;
                    var_t0 += 0xA00;
                } while (var_a2 < (s32) *(void *)0x3F34C3);
            }
        }
        var_a3_2 = 0;
        var_v1 = 0;
        if ((s32) *(void *)0x3F34C3 > 0) {
            var_t2 = em;
            var_t1 = &sp7C;
            do {
                if ((EM_FIELD(var_t2, s32 *, 0x918) == 0xC350) && (var_s0 & 0xFF & (1 << var_a3_2))) {
                    *var_t1 = var_a3_2;
                    var_v1 += 1;
                    var_t1 += 1;
                }
                var_a3_2 += 1;
                var_t2 += 4;
            } while ((s32) var_a3_2 < (s32) *(void *)0x3F34C3);
        }
        if (var_v1 >= 2) {
            var_v0 = sp7C;
            var_a2_2 = 1;
            if (var_v1 >= 2) {
                var_a3_3 = &sp7D;
                do {
                    temp_a1 = *var_a3_3;
                    if (EM_FIELD((em + ((var_v0 & 0xFF) * 4)), f32 *, 0x8C4) < EM_FIELD((em + (temp_a1 * 4)), f32 *, 0x8C4)) {
                        var_v0 = temp_a1;
                    }
                    var_a2_2 += 1;
                    var_a3_3 += 1;
                } while (var_a2_2 < var_v1);
            }
        } else if (var_v1 == 1) {
            var_v0 = sp7C;
        } else {
            temp_a2_2 = *(void *)0x3F34C3;
            var_v1_2 = 0;
            var_a1_2 = 0;
            if ((s32) temp_a2_2 > 0) {
                var_t1_2 = em;
                var_t0_2 = &sp7C;
                do {
                    if ((EM_FIELD(var_t1_2, s32 *, 0x918) >= 0x7530) && (var_s0 & 0xFF & (1 << var_v1_2))) {
                        *var_t0_2 = var_v1_2;
                        var_a1_2 += 1;
                        var_t0_2 += 1;
                    }
                    var_v1_2 += 1;
                    var_t1_2 += 4;
                } while ((s32) var_v1_2 < (s32) temp_a2_2);
            }
            var_v0 = 0xFF;
            if (var_a1_2 >= 2) {
                var_v0 = sp7C;
                var_a2_3 = 1;
                if (var_a1_2 >= 2) {
                    var_a3_4 = &sp7D;
                    do {
                        temp_a0_2 = *var_a3_4;
                        if (EM_FIELD((em + ((var_v0 & 0xFF) * 4)), f32 *, 0x8C4) < EM_FIELD((em + (temp_a0_2 * 4)), f32 *, 0x8C4)) {
                            var_v0 = temp_a0_2;
                        }
                        var_a2_3 += 1;
                        var_a3_4 += 1;
                    } while (var_a2_3 < var_a1_2);
                }
            } else if (var_a1_2 == 1) {
                var_v0 = sp7C;
            } else {
                temp_a3_2 = *(void *)0x3F34C3;
                var_a1_3 = 0;
                var_a2_4 = 0;
                if ((s32) temp_a3_2 > 0) {
                    var_t1_3 = &sp7C;
                    do {
                        if (var_s0 & 0xFF & (1 << var_a1_3)) {
                            *var_t1_3 = var_a1_3;
                            var_a2_4 += 1;
                            var_t1_3 += 1;
                        }
                        var_a1_3 += 1;
                    } while ((s32) var_a1_3 < (s32) temp_a3_2);
                }
                var_a3_5 = 1;
                if (var_a2_4 == 1) {
                    var_v0 = sp7C;
                } else if (var_a2_4 != 0) {
                    var_v0 = sp7C;
                    if (var_a2_4 >= 2) {
                        var_t0_3 = &sp7D;
                        do {
                            temp_a1_2 = *var_t0_3;
                            if (EM_FIELD((em + ((var_v0 & 0xFF) * 4)), s32 *, 0x918) < EM_FIELD((em + (temp_a1_2 * 4)), s32 *, 0x918)) {
                                var_v0 = temp_a1_2;
                            }
                            var_a3_5 += 1;
                            var_t0_3 += 1;
                        } while (var_a3_5 < var_a2_4);
                    }
                }
            }
        }
        goto block_77;
    }
    var_v0 = -1U;
block_77:
    EM_FIELD(em, u8 *, 0x844) = var_v0;
    return p;
}

u8 *em_cmd_st25_gate_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (*(u8 *)0x3F35D6 == 0) {
            CMD_SKIP(em, var_a1, 0x60);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x60);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_runaway_timer_set(EMW *em, u8 *p) {
    em->runaway_tm = (s16) *(&em02_runaway_timer_tbl + (em->stg * 2));
    return p;
}

u8 *em_cmd_male_ck(EMW *em, u8 *p) {
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        if (em->x11 != 0) {
            CMD_SKIP(em, var_a1, 0x63);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x63);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_target_pl_hate_ck(EMW *em, u8 *p) {
    s8 temp_v1_2;
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_v1;
    u8 temp_v1_3;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        temp_v1_2 = em->x844;
        var_a1 += 1;
        if (temp_v1_2 != -1) {
            if (EM_FIELD(((temp_v1_2 * 4) + em), s32 *, 0x918) < *(&check_hate_tbl + ((EM_FIELD(p, u8 *, 1) & 0xFF) * 4))) {
                goto loop_7;
            }
        } else {
            CMD_SKIP(em, var_a1, 0x64);
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x64);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_all_pl_hate_clear(EMW *em, u8 *p) {
    EM_FIELD(em, s32 *, 0x918) = 0;
    EM_FIELD(em, s32 *, 0x91C) = 0;
    EM_FIELD(em, s32 *, 0x920) = 0;
    EM_FIELD(em, s32 *, 0x924) = 0;
    return p;
}

u8 *em_cmd_pl_ride_ck(EMW *em, u8 *p) {
    int *var_a2;
    s32 var_s0;
    int var_a0;
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;

    var_a1 = p + 1;
    temp_v1 = *p;
    var_s0 = 1;
    switch (temp_v1) {                              /* irregular */
    case 2:
        break;
    case 0:
        temp_v1_2 = *(u8 *)0x3F34C3;
        var_a2 = &player_work;
        var_a0 = 0;
        if ((s32) temp_v1_2 > 0) {
            do {
                if (EM_FIELD(var_a2, u8 *, 0x604) != 0) {
                    var_s0 = 0;
                }
                var_a0 = (s8)(var_a0 + 1);
                var_a2 += 0xA00;
            } while (var_a0 < (s32) temp_v1_2);
        }
        if (var_s0 != 0) {
            if (var_s0 != 0) {
loop_11:
                temp_a0 = EM_FIELD(var_a1, u8 *, 0);
                if ((temp_a0 != 0x66) || (EM_FIELD(var_a1, u8 *, 1) != 1)) {
                    if (temp_a0 == 0x66) {
                        if (EM_FIELD(var_a1, u8 *, 1) != 2) {
                            goto block_16;
                        }
                    } else {
block_16:
                        var_a1 = cmd_end_search(em, var_a1, 0x66, 2);
                        if (var_s0 != 0) {
                            goto loop_11;
                        }
                    }
                }
            }
            temp_v0 = next_cmd_search(em, var_a1);
            temp_v1_3 = EM_FIELD(temp_v0, u8 *, 0);
            var_a1 = temp_v0;
            if ((temp_v1_3 == 0x66) && (EM_FIELD(var_a1, u8 *, 1) == 2)) {
                var_a1 = next_cmd_search(em, var_a1);
            }
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x66);
        break;
    }
    return var_a1;
}

u8 *em_cmd_em_master_ck(EMW *em, u8 *p) {
    s32 temp_s0;
    u8 *temp_v0;
    u8 *var_a1;
    u8 temp_v1;
    u8 temp_v1_2;

    var_a1 = p;
    switch (*var_a1++) {
    case 0:
        temp_s0 = em->x8C3 != 0;
        if (temp_s0 != 0) {
            if (temp_s0 != 0) {
loop_7:
                temp_a0 = EM_FIELD(var_a1, u8 *, 0);
                if ((temp_a0 != 0x67) || (EM_FIELD(var_a1, u8 *, 1) != 1)) {
                    if (temp_a0 == 0x67) {
                        if (EM_FIELD(var_a1, u8 *, 1) != 2) {
                            goto block_12;
                        }
                    } else {
block_12:
                        var_a1 = cmd_end_search(em, var_a1, 0x67, 2);
                        if (temp_s0 != 0) {
                            goto loop_7;
                        }
                    }
                }
            }
            temp_v0 = next_cmd_search(em, var_a1);
            temp_v1_2 = EM_FIELD(temp_v0, u8 *, 0);
            var_a1 = temp_v0;
            if ((temp_v1_2 == 0x67) && (EM_FIELD(var_a1, u8 *, 1) == 2)) {
                var_a1 = next_cmd_search(em, var_a1);
            }
        }
        break;
    case 1:
        var_a1 = else_ck(em, var_a1, 0x67);
        break;
    case 2:
        break;
    }
    return var_a1;
}

u8 *em_cmd_em_cmd_reset(EMW *em, u8 *p) {
    em_cmd_reset();
    return p;
}

u8 *em_cmd_rnd32(EMW *em, u8 *p) {
    s32 temp_at;
    s32 temp_s1;
    s32 var_s2;
    s32 var_s3;
    u8 temp_a0;
    u8 temp_s0;
    u8 temp_v1;
    void *temp_v0;
    void *temp_v0_2;
    void *var_a1;
    void *var_a1_2;

    var_a1 = p;
    temp_v1 = EM_FIELD(var_a1, u8 *, 0);
    switch (temp_v1) {                              /* irregular */
    case 0x0:
        temp_s0 = EM_FIELD(var_a1, u8 *, 1);
        var_s2 = 0;
        var_s3 = 0;
        temp_at = (s32) temp_s0 > 0;
        var_a1 += 2;
        if (temp_at != 0) {
            temp_s1 = em->x39A & 0x1F & 0xFFFF;
loop_15:
            temp_v0 = cmd_end_search(em, var_a1, 0x80, 0xFF);
            temp_a0 = EM_FIELD(temp_v0, u8 *, 2);
            var_a1 = temp_v0 + 3;
            if ((temp_a0 == 0) || (((temp_a0 & 0xFF) != 0xFF) && (var_s2 = (var_s2 + temp_a0) & 0xFF, ((temp_s1 < var_s2) == 0)))) {
                var_s3 = (var_s3 + 1) & 0xFFFF;
                if (var_s3 >= (s32) temp_s0) {

                } else {
                    goto loop_15;
                }
            }
        }
        break;
    case 0xA:
    case 0x9:
    case 0x8:
    case 0x7:
    case 0x6:
    case 0x5:
    case 0x4:
    case 0x3:
    case 0x2:
    case 0x1:
        var_a1_2 = var_a1 + 2;
loop_22:
        temp_v0_2 = cmd_end_search(em, var_a1_2, 0x80, 0xFF);
        if (EM_FIELD(temp_v0_2, u8 *, 1) != 0xFF) {
            var_a1_2 = temp_v0_2 + 3;
            goto loop_22;
        }
        var_a1 = temp_v0_2 + 2;
        break;
    case 0xFF:
        var_a1 += 1;
        break;
    }
    return var_a1;
}

u8 *em_cmd_contents(EMW *em, u8 *p) {
    s32 temp_a1;

    EM_FIELD(em, u16 *, 0x822) = (u16) *p;
    if (em->x826 == 0) {
        em->cmd_p808 = (void *) (p + 1);
    }
    em->x826 = 1U;
    temp_a1 = *(EM_FIELD(em->cmd_tbl, s32 *, 4) + (EM_FIELD(em, u16 *, 0x822) * 4));
    EM_FIELD(em, s32 *, 0x870) = temp_a1;
    return temp_a1;
}

u8 *em_cmd_sub_contents(EMW *em, u8 *p) {
    s32 temp_a1;

    EM_FIELD(em, u16 *, 0x824) = (u16) EM_FIELD(p, u8 *, 1);
    if (em->x826 == 1) {
        em->cmd_p80C = (void *) (p + 2);
    }
    em->x826 = 2U;
    temp_a1 = *(EM_FIELD((((EM_FIELD(p, u8 *, 0) & 0xFF) * 4) + EM_FIELD(em, s32 *, 0x800)), s32 *, 0x3C) + (EM_FIELD(em, u16 *, 0x824) * 4));
    EM_FIELD(em, s32 *, 0x870) = temp_a1;
    return temp_a1;
}

u8 *em_cmd_range_ck(EMW *em, u8 *p) {
    f32 temp_f1;
    f32 var_f1;
    s32 temp_v1_4;
    s32 var_s1;
    s8 temp_v1_2;
    s8 var_a2;
    u8 temp_v1;
    u8 temp_v1_3;
    void *temp_v0;
    void *var_a0;
    void *var_a1;
    void *var_a1_2;

    var_a1 = p + 1;
    temp_v1 = EM_FIELD(p, u8 *, 0);
    switch (temp_v1) {                              /* irregular */
    case 0x0:
        temp_v1_2 = em->x844;
        if (temp_v1_2 == -1) {
            var_f1 = -1.0f;
        } else {
            var_f1 = EM_FIELD((((temp_v1_2 & 0xF) * 4) + em), f32 *, 0x8C4);
        }
        temp_v1_3 = EM_FIELD(p, u8 *, 1);
        var_a1_2 = var_a1 + 1;
        if (var_f1 == -1.0f) {
            var_a2 = temp_v1_3 & 0xFF;
        } else {
            temp_v1_4 = temp_v1_3 & 0xFF;
            var_a2 = 0;
            EM_FIELD(em, f32 *, 0x3AC) = var_f1;
            if (temp_v1_4 > 0) {
                var_a0 = em;
loop_16:
                temp_f1 = EM_FIELD(var_a0, f32 *, 0x810);
                if (EM_FIELD(em, f32 *, 0x3AC) <= temp_f1) {
                    if (temp_f1 < 0.0f) {
                        goto block_19;
                    }
                } else {
block_19:
                    var_a2 += 1;
                    var_a0 += 4;
                    if (var_a2 < temp_v1_4) {
                        goto loop_16;
                    }
                }
            }
        }
        em->x82A = var_a2;
        var_s1 = 0;
        if ((s32) (u8) em->x82A > 0) {
            do {
                var_a1_2 = cmd_end_search(em, next_cmd_search(em, var_a1_2), 0x83, 0xFF);
                var_s1 += 1;
            } while (var_s1 < (s32) (u8) em->x82A);
        }
        var_a1 = next_cmd_search(em, var_a1_2);
        break;
    case 0x5:
    case 0x4:
    case 0x3:
    case 0x2:
    case 0x1:
loop_25:
        temp_v0 = cmd_end_search(em, var_a1, 0x83, 0xFF);
        if (EM_FIELD(temp_v0, u8 *, 1) != 0xFF) {
            var_a1 = temp_v0 + 2;
            goto loop_25;
        }
        var_a1 = temp_v0 + 2;
        break;
    }
    return var_a1;
}

u8 *em_cmd_end_command(EMW *em, u8 *p) {
    s8 sp60;
    int sp50;
    f32 temp_f0;
    f32 var_f1;
    s16 temp_v1_2;
    s16 temp_v1_3;
    s32 temp_a2;
    s32 temp_s0;
    int temp_v1_8;
    int var_a1;
    int var_a2;
    s8 *var_a0;
    s8 temp_v0_2;
    s8 temp_v1_5;
    u8 *var_s2;
    u8 temp_a1;
    u8 temp_v1;
    u8 temp_v1_4;
    u8 temp_v1_6;
    u8 temp_v1_7;
    void *temp_s1;
    void *temp_v0;
    void *temp_v0_3;

    temp_v1 = *p;
    var_s2 = p;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        EM_FIELD(em, s16 *, 0x820) = 0;
        var_s2 = em_cmd_top(em);
        em->x83B = 0U;
        em_g_init_flag_set(em, 2);
        break;
    case 0x1:                                       /* switch 1 */
        EM_FIELD(em, s8 *, 0x826) = 0;
        var_s2 = em->cmd_p808;
        break;
    case 0x2:                                       /* switch 1 */
        EM_FIELD(em, s8 *, 0x826) = 1;
        var_s2 = em->cmd_p80C;
        break;
    case 0x3:                                       /* switch 1 */
        var_s2 = em->cmd_p840;
        break;
    case 0x4:                                       /* switch 1 */
        temp_v0 = gp_ck(EM_FIELD(em->area, s32 *, 0x18), em->stg);
        temp_s1 = EM_FIELD(temp_v0, void **, 4);
        if (temp_s1 == NULL) {
            var_s2 = em->cmd_p868;
            em->cmd_p868 = NULL;
            em->x827 = 0U;
            em->x881 = 0U;
        } else {
            temp_v1_2 = EM_FIELD(temp_v0, s16 *, 2);
            if ((s32) em->x86D >= temp_v1_2) {
                em->x86D = (u8) (temp_v1_2 - 1);
            }
            temp_a1 = em->x86D;
            temp_v1_3 = EM_FIELD(temp_s1, s16 *, 2);
            temp_s0 = EM_FIELD(((temp_a1 * 0x18) + temp_s1), s32 *, 4);
            if ((s32) em->x86E >= temp_v1_3) {
                em->x86E = (u8) (temp_v1_3 - 1);
            }
            if (em->x8C3 != 0) {
                if (em->x881 == 9) {
                    em->x86C = (u8) EM_FIELD(((em->x86E * 0x14) + temp_s0), u16 *, 0x12);
                    var_s2 = ground_area_move_ptr_set(em, em->x86C);
                } else {
                    EM_FIELD(em, s16 *, 0x820) = 0;
                    var_s2 = em_cmd_top(em, temp_a1);
                }
            } else {
                temp_v1_4 = em->x86E;
                if (!(CalcDistanceXZ(em + 0xAC, temp_s0 + (em->x86E * 0x14)) <= EM_FIELD(((temp_v1_4 * 0x14) + temp_s0), f32 *, 0xC))) {
                    temp_v0_2 = em->x8BE + 1;
                    em->x8BE = temp_v0_2;
                    if (((s8)(temp_v0_2)) >= 0xA) {
                        em->x86E = (u8) (EM_FIELD(temp_s1, s16 *, 2) - 1);
                    } else {
                        em->x827 = 9U;
                        em->x828 = (u8) em->x86D;
                        em->x829 = (u8) em->x86E;
                        em->x881 = (u8) em->x827;
                        em->x882 = (u8) em->x828;
                        em->x883 = (u8) em->x829;
                        em->x86C = (u8) EM_FIELD(((em->x86E * 0x14) + temp_s0), u16 *, 0x12);
                        var_s2 = ground_area_move_ptr_set(em, em->x86C);
                        em->cmd_top = var_s2;
                    }
                } else if (temp_v1_4 == 0) {
                    var_s2 = em->cmd_p868;
                    em->cmd_p868 = NULL;
                    if ((*(temp_s1 + (em->x882 * 0x18)) != -1) && (em->x9E2 != 0)) {
                        EM_FIELD(em, f32 *, 0xAC) = (f32) EM_FIELD(((em->x86D * 0x18) + temp_s1), f32 *, 8);
                        EM_FIELD(em, f32 *, 0xB0) = (f32) EM_FIELD(((em->x86D * 0x18) + temp_s1), f32 *, 0xC);
                        EM_FIELD(em, f32 *, 0xB4) = (f32) EM_FIELD(((em->x86D * 0x18) + temp_s1), f32 *, 0x10);
                        EM_FIELD(em, s32 *, 0xA4) = (s32) ((EM_FIELD(((em->x882 * 0x18) + temp_s1), u16 *, 0x14) + 0x4000) & 0xFFFF);
                        em->x92E = (u8) em->stg;
                        em->stg = (u8) *(temp_s1 + (em->x882 * 0x18));
                        em_area_move_init(em);
                        em_g_init_flag_set(em, 1);
                    }
                    em->x827 = 0U;
                    em->x881 = 0U;
                } else {
                    em->x8BE = 0;
                    em->x86E = (u8) (em->x86E - 1);
                    em->x827 = 9U;
                    em->x828 = (u8) em->x86D;
                    em->x829 = (u8) em->x86E;
                    em->x881 = (u8) em->x827;
                    em->x882 = (u8) em->x828;
                    em->x883 = (u8) em->x829;
                    em->x86C = (u8) EM_FIELD(((em->x86E * 0x14) + temp_s0), u16 *, 0x12);
                    var_s2 = ground_area_move_ptr_set(em, em->x86C);
                }
            }
        }
        break;
    case 0xF5:                                      /* switch 1 */
        EM_FIELD(em, s16 *, 0x820) = 0;
        var_s2 = em_cmd_top(em);
        em->x83B = 0U;
        em_g_init_flag_set(em, 2);
        break;
    case 0xF6:                                      /* switch 1 */
        EM_FIELD(em, s16 *, 0x820) = 0;
        var_s2 = em_cmd_top(em);
        em->x83B = 0U;
        em_g_init_flag_set(em, 2);
        break;
    case 0xF7:                                      /* switch 1 */
        EM_FIELD(em, s16 *, 0x820) = 0;
        var_s2 = em_cmd_top(em);
        em->x83B = 0U;
        em_g_init_flag_set(em, 2);
        temp_v1_5 = em->x844;
        if (temp_v1_5 != -1) {
            temp_a2 = temp_v1_5 & 0xF;
            em->x88F = (u8) (em->x88F & (~(1 << temp_a2) & 0xFF));
            temp_v0_3 = (temp_a2 * 4) + em;
            EM_FIELD(((temp_a2 * 2) + em), s16 *, 0x890) = 0;
            EM_FIELD(temp_v0_3, s32 *, 0x8F4) = 0;
            EM_FIELD(temp_v0_3, s32 *, 0x918) = 0;
        }
        break;
    case 0xF8:                                      /* switch 1 */
        em->x83B = (u8) (em->x83B & 0xF7);
        if (em->x83B & 1) {
            var_s2 = route_ptr_set(em->x833);
        } else {
            EM_FIELD(em, s16 *, 0x820) = 0;
            var_s2 = em_cmd_top(em);
            em_g_init_flag_set(em, 2);
        }
        break;
    case 0xF9:                                      /* switch 1 */
        em->x83B = (u8) (em->x83B & 0xFB);
        if (em->x83B & 1) {
            var_s2 = route_ptr_set(em->x833);
        } else {
            EM_FIELD(em, s16 *, 0x820) = 0;
            var_s2 = em_cmd_top(em);
            em_g_init_flag_set(em, 2);
        }
        break;
    case 0xFA:                                      /* switch 1 */
        em_cmd_top(em);
        em->x83B = (u8) (em->x83B & 0xFD);
        em_search_data_set(em, 0);
        if (em->x83B & 1) {
            var_s2 = route_ptr_set((u8) em, em->x833);
        } else {
            EM_FIELD(em, s16 *, 0x820) = 0;
            var_s2 = em_cmd_top(em);
            em_g_init_flag_set(em, 2);
        }
        break;
    case 0xFB:                                      /* switch 1 */
        EM_FIELD(em, s8 *, 0x84D) = 0;
        var_s2 = em->cmd_p848;
        em->x92D = (s8) (em->x92D + 1);
        if (em->x92D >= em->x92C) {
            if (em->x84C == 0) {
                em->x92D = -1;
                em->x92C = -1;
            } else {
                em->x92D = 0;
            }
        }
        break;
    case 0xFC:                                      /* switch 1 */
        if (em->x8C3 == 0) {
            Em_Mode_Chg(1, *(&em_atk_mode_timer_tbl + (EM_FIELD(em, u8 *, 2) * 2)));
            em->x838 = (u8) em->x888;
        }
        EM_FIELD(em, s16 *, 0x820) = 0;
        var_s2 = em_cmd_top(em);
        em->x83B = 0U;
        em_g_init_flag_set(em, 2);
        break;
    case 0xFD:                                      /* switch 1 */
        em->x83B = (u8) (em->x83B & 0xEF);
        if (em->x83B & 1) {
            var_s2 = route_ptr_set(em->x833);
        } else {
            EM_FIELD(em, s16 *, 0x820) = 0;
            var_s2 = em_cmd_top(em);
            EM_FIELD(em, s8 *, 0x86F) = 2;
        }
        break;
    case 0xFE:                                      /* switch 1 */
        cmd_target_kind_set(&sp50);
        temp_f0 = CalcDistanceXZ(&sp50, em + 0xAC);
        temp_v1_6 = EM_FIELD(em, u8 *, 2);
        if (temp_v1_6 != 2) {
            if (temp_v1_6 == 7) {
                goto block_81;
            }
            var_f1 = 500.0f;
        } else {
block_81:
            var_f1 = 1000.0f;
        }
        if (temp_f0 <= var_f1) {
            em->x92A = (s8) (em->x92A + 1);
            temp_v1_7 = em->x830;
            switch (temp_v1_7) {                    /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                var_a1 = 0;
                var_a2 = 0;
                if (em->x92B > 0) {
                    var_a0 = &sp60;
                    do {
                        if (em->x928 != ((s8)(var_a1))) {
                            *var_a0 = (s8) var_a1;
                            var_a0 += 1;
                            var_a2 = (s8)(var_a2 + 1);
                        }
                        var_a1 = (s8)(var_a1 + 1);
                    } while (var_a1 < em->x92B);
                }
                temp_v1_8 = (s8)(var_a2);
                if (temp_v1_8 == 0) {
                    em->x928 = 0;
                } else {
                    if (temp_v1_8 == 0) {
                        M2C_BREAK(0);
                    }
                    em->x928 = (s8) EM_FIELD((((s8)(((s32) em->x39A % temp_v1_8))) + sp), u8 *, 0x60);
                }
                break;
            case 1:                                 /* switch 2 */
                em->x928 = (s8) *(option_route_ptr_set(em, em->x831) + em->x92A);
                break;
            case 2:                                 /* switch 2 */
                em->x928 = (s8) (em->x928 + 1);
                break;
            }
        }
        var_s2 = route_ptr_set((u8) em, em->x833);
        break;
    case 0xFF:                                      /* switch 1 */
        if (em->x92A < em->x929) {
            var_s2 = route_ptr_set(em->x832);
        } else {
            em->x83B = (u8) (em->x83B & 0xFE);
            var_s2 = set_cmd();
        }
        break;
    }
    em->cmd_top = var_s2;
    return var_s2;
}

u8 *em_cmd_position_set(EMW *em, u8 *p) {
    void *temp_v1;

    temp_v1 = *(&em_cmd_pos_tbl + (EM_FIELD(em, u8 *, 2) * 4)) + (*p * 0xC);
    EM_FIELD(em, f32 *, 0xAC) = (f32) EM_FIELD(temp_v1, f32 *, 0);
    EM_FIELD(em, f32 *, 0xB0) = (f32) EM_FIELD(temp_v1, f32 *, 4);
    EM_FIELD(em, f32 *, 0xB4) = (f32) EM_FIELD(temp_v1, f32 *, 8);
    EM_FIELD(em, f32 *, 0x5A0) = (f32) EM_FIELD(em, f32 *, 0xAC);
    EM_FIELD(em, f32 *, 0x5A4) = (f32) EM_FIELD(em, f32 *, 0xB0);
    EM_FIELD(em, f32 *, 0x5A8) = (f32) EM_FIELD(em, f32 *, 0xB4);
    return p + 1;
}

u8 *em_cmd_vec_set(EMW *em, u8 *p) {
    EM_FIELD(em, s32 *, 0xA4) = (s32) (Em_Calc_angY(em + 0xAC, *(&em_cmd_pos_tbl + (EM_FIELD(em, u8 *, 2) * 4)) + (*p * 0xC)) & 0xFFFF);
    return p + 1;
}

u8 *em_cmd_demo_start(EMW *em, u8 *p) {
    return p;
}

u8 *em_cmd_wait_set(EMW *em, u8 *p) {
    return p;
}

u8 *em_cmd_em_atk_bit(EMW *em, u8 *p) {
    s32 temp_v1_2;
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s1;
    u8 *var_a1;
    u8 temp_a0;
    u8 temp_s0;
    u8 temp_v0;
    u8 temp_v1;
    u8 temp_v1_3;
    void *temp_v0_2;
    void *temp_v0_3;
    void *temp_v0_4;

    var_a1 = p + 1;
    temp_v1 = EM_FIELD(p, u8 *, 0);
    switch (temp_v1) {                              /* irregular */
    case 3:
        break;
    case 0:
        temp_s0 = EM_FIELD(p, u8 *, 1);
        var_s1 = 0;
        var_a1 += 3;
        if ((s32) temp_s0 > 0) {
loop_7:
            temp_v0 = *var_a1;
            temp_v1_2 = EM_FIELD(saved_reg_gp, s32 *, -0x4440);
            var_a1 += 1;
            if (temp_v1_2 != temp_v0) {
                if ((s32) temp_v0 < temp_v1_2) {
                    temp_v0_2 = cmd_end_search(em, var_a1, 0x94, 3);
                    temp_v1_3 = EM_FIELD(temp_v0_2, u8 *, 1);
                    if ((temp_v1_3 == 2) || (temp_v1_3 == 3)) {
                        var_a1 = temp_v0_2 + 2;
                    } else {
                        var_a1 = temp_v0_2 + 2;
                        var_s1 += 1;
                        if (var_s1 >= (s32) temp_s0) {

                        } else {
                            goto loop_7;
                        }
                    }
                } else {
                    var_s0 = 1;
                    do {
                        temp_v0_3 = cmd_end_search(em, var_a1, 0x94, 3);
                        temp_a0 = EM_FIELD(temp_v0_3, u8 *, 0);
                        if (temp_a0 == 0x94) {
                            if (EM_FIELD(temp_v0_3, u8 *, 1) != 2) {
                                goto block_17;
                            }
                            goto block_20;
                        }
block_17:
                        if ((temp_a0 == 0x94) && (EM_FIELD(temp_v0_3, u8 *, 1) == 3)) {
block_20:
                            var_s0 = 0;
                        }
                        var_a1 = next_cmd_search(em, temp_v0_3);
                    } while (var_s0 != 0);
                }
            }
        }
        break;
    case 1:
        var_a1 += 1;
        /* fallthrough */
    case 2:
        var_s0_2 = 1;
        do {
            temp_v0_4 = cmd_end_search(em, var_a1, 0x94, 3);
            if ((EM_FIELD(temp_v0_4, u8 *, 0) == 0x94) && (EM_FIELD(temp_v0_4, u8 *, 1) == 3)) {
                var_s0_2 = 0;
            }
            var_a1 = next_cmd_search(em, temp_v0_4);
        } while (var_s0_2 != 0);
        break;
    }
    return var_a1;
}

u8 *em_cmd_top(EMW *em) {
    EM_FIELD(em, s8 *, 0x826) = 0;
    return em->cmd_top = (*em->cmd_tbl)[em->cmd_idx];
}

u8 *cmd_end_search(EMW *em, u8 *p, int a2, int a3) {
    s32 temp_s1;
    u8 temp_v0;
    u8 temp_v1;
    void *var_a1;
    void *var_a1_2;

    var_a1 = p;
    temp_s1 = a2 & 0xFF;
loop_3:
    temp_v0 = EM_FIELD(var_a1, u8 *, 0);
    if (temp_v0 != temp_s1) {
        var_a1 = next_cmd_search(em, var_a1);
        goto loop_3;
    }
    if ((temp_v0 == temp_s1) && (EM_FIELD(var_a1, u8 *, 1) == 0)) {
        var_a1_2 = next_cmd_search(em, var_a1);
loop_7:
        temp_v1 = EM_FIELD(var_a1_2, u8 *, 0);
        if (temp_v1 != temp_s1) {
block_10:
            if (temp_v1 == temp_s1) {
                var_a1_2 = next_cmd_search(em, var_a1_2);
            }
            var_a1_2 = cmd_end_search(em, var_a1_2, a2, a3);
            goto loop_7;
        }
        if (EM_FIELD(var_a1_2, u8 *, 1) != (a3 & 0xFF)) {
            goto block_10;
        }
        var_a1 = next_cmd_search(em, var_a1_2);
        goto loop_3;
    }
    return var_a1;
}

u8 *next_cmd_search(EMW *em, u8 *p) {
    u8 *temp_v1_10;
    u8 *temp_v1_16;
    u8 *temp_v1_19;
    u8 *temp_v1_22;
    u8 *temp_v1_36;
    u8 *temp_v1_4;
    u8 *temp_v1_8;
    u8 *var_a1;
    u8 temp_v0;
    u8 temp_v1;
    u8 temp_v1_11;
    u8 temp_v1_12;
    u8 temp_v1_13;
    u8 temp_v1_14;
    u8 temp_v1_15;
    u8 temp_v1_17;
    u8 temp_v1_18;
    u8 temp_v1_20;
    u8 temp_v1_21;
    u8 temp_v1_23;
    u8 temp_v1_24;
    u8 temp_v1_25;
    u8 temp_v1_26;
    u8 temp_v1_27;
    u8 temp_v1_28;
    u8 temp_v1_29;
    u8 temp_v1_2;
    u8 temp_v1_30;
    u8 temp_v1_31;
    u8 temp_v1_32;
    u8 temp_v1_33;
    u8 temp_v1_34;
    u8 temp_v1_35;
    u8 temp_v1_37;
    u8 temp_v1_3;
    u8 temp_v1_5;
    u8 temp_v1_6;
    u8 temp_v1_7;
    u8 temp_v1_9;

    var_a1 = p;
    temp_v0 = *var_a1;
    switch (temp_v0) {                              /* switch 1; irregular */
    case 0x1:                                       /* switch 1 */
        var_a1 += 2;
        break;
    case 0x2:                                       /* switch 1 */
        var_a1 += 2;
        break;
    case 0x3:                                       /* switch 1 */
        var_a1 += 2;
        break;
    case 0x4:                                       /* switch 1 */
        var_a1 += 1;
        break;
    case 0x5:                                       /* switch 1 */
        var_a1 += 4;
        break;
    case 0x6:                                       /* switch 1 */
        var_a1 += 4;
        break;
    case 0x7:                                       /* switch 1 */
        var_a1 += 2;
        break;
    case 0x8:                                       /* switch 1 */
        var_a1 += 2;
        break;
    case 0x9:                                       /* switch 1 */
        var_a1 += 2;
        break;
    case 0xA:                                       /* switch 1 */
        var_a1 += 2;
        break;
    case 0xB:                                       /* switch 1 */
        var_a1 += 1;
        temp_v1 = *var_a1;
        switch (temp_v1) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 2 */
        case 1:                                     /* switch 2 */
            var_a1 += 1;
            break;
        }
        break;
    case 0xC:                                       /* switch 1 */
        var_a1 += 3;
        break;
    case 0xD:                                       /* switch 1 */
        var_a1 += 2;
        break;
    case 0xE:                                       /* switch 1 */
        var_a1 += 1;
        temp_v1_2 = *var_a1;
        switch (temp_v1_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 3 */
        case 1:                                     /* switch 3 */
            var_a1 += 1;
            break;
        }
        break;
    case 0xF:                                       /* switch 1 */
        var_a1 += 6;
        break;
    case 0x10:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x11:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x12:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x13:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x14:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_3 = *var_a1;
        switch (temp_v1_3) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 4 */
        case 1:                                     /* switch 4 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x15:                                      /* switch 1 */
        temp_v1_4 = var_a1 + 1;
        var_a1 = temp_v1_4 + 1;
        temp_v1_5 = *temp_v1_4;
        switch (temp_v1_5) {                        /* switch 5; irregular */
        case 3:                                     /* switch 5 */
        case 2:                                     /* switch 5 */
            break;
        case 0:                                     /* switch 5 */
            var_a1 += 3;
            /* fallthrough */
        case 1:                                     /* switch 5 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x16:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x17:                                      /* switch 1 */
        var_a1 += 5;
        break;
    case 0x18:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x19:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x1A:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x1B:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_6 = *var_a1;
        switch (temp_v1_6) {                        /* switch 6; irregular */
        case 0:                                     /* switch 6 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 6 */
        case 1:                                     /* switch 6 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x1C:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_7 = *var_a1;
        switch (temp_v1_7) {                        /* switch 7; irregular */
        case 0:                                     /* switch 7 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 7 */
        case 1:                                     /* switch 7 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x1D:                                      /* switch 1 */
        temp_v1_8 = var_a1 + 1;
        var_a1 = temp_v1_8 + 1;
        temp_v1_9 = *temp_v1_8;
        switch (temp_v1_9) {                        /* switch 8; irregular */
        case 3:                                     /* switch 8 */
        case 2:                                     /* switch 8 */
            break;
        case 0:                                     /* switch 8 */
            var_a1 += 3;
            /* fallthrough */
        case 1:                                     /* switch 8 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x1E:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x20:                                      /* switch 1 */
        temp_v1_10 = var_a1 + 1;
        var_a1 = temp_v1_10 + 1;
        temp_v1_11 = *temp_v1_10;
        switch (temp_v1_11) {                       /* switch 9; irregular */
        case 3:                                     /* switch 9 */
        case 2:                                     /* switch 9 */
            break;
        case 0:                                     /* switch 9 */
            var_a1 += 3;
            /* fallthrough */
        case 1:                                     /* switch 9 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x21:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_12 = *var_a1;
        switch (temp_v1_12) {                       /* switch 10; irregular */
        case 0:                                     /* switch 10 */
            var_a1 += 1;
            break;
        case 2:                                     /* switch 10 */
        case 1:                                     /* switch 10 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x22:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_13 = *var_a1;
        switch (temp_v1_13) {                       /* switch 11; irregular */
        case 0:                                     /* switch 11 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 11 */
        case 1:                                     /* switch 11 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x23:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_14 = *var_a1;
        switch (temp_v1_14) {                       /* switch 12; irregular */
        case 0:                                     /* switch 12 */
            var_a1 += 3;
            break;
        case 2:                                     /* switch 12 */
        case 1:                                     /* switch 12 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x24:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_15 = *var_a1;
        switch (temp_v1_15) {                       /* switch 13; irregular */
        case 0:                                     /* switch 13 */
            var_a1 += 2;
            break;
        case 1:                                     /* switch 13 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x25:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x26:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x27:                                      /* switch 1 */
        temp_v1_16 = var_a1 + 1;
        var_a1 = temp_v1_16 + 1;
        temp_v1_17 = *temp_v1_16;
        switch (temp_v1_17) {                       /* switch 14; irregular */
        case 3:                                     /* switch 14 */
        case 2:                                     /* switch 14 */
            break;
        case 0:                                     /* switch 14 */
            var_a1 += 3;
            /* fallthrough */
        case 1:                                     /* switch 14 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x28:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x29:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x2A:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x2B:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_18 = *var_a1;
        switch (temp_v1_18) {                       /* switch 15; irregular */
        case 0:                                     /* switch 15 */
            var_a1 += 3;
            break;
        case 2:                                     /* switch 15 */
        case 1:                                     /* switch 15 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x2C:                                      /* switch 1 */
        temp_v1_19 = var_a1 + 1;
        var_a1 = temp_v1_19 + 1;
        temp_v1_20 = *temp_v1_19;
        switch (temp_v1_20) {                       /* switch 16; irregular */
        case 3:                                     /* switch 16 */
        case 2:                                     /* switch 16 */
            break;
        case 0:                                     /* switch 16 */
            var_a1 += 3;
            /* fallthrough */
        case 1:                                     /* switch 16 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x2D:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x2E:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x2F:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x30:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x31:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x32:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_21 = *var_a1;
        switch (temp_v1_21) {                       /* switch 17; irregular */
        case 0:                                     /* switch 17 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 17 */
        case 1:                                     /* switch 17 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x33:                                      /* switch 1 */
        temp_v1_22 = var_a1 + 1;
        var_a1 = temp_v1_22 + 1;
        temp_v1_23 = *temp_v1_22;
        switch (temp_v1_23) {                       /* switch 18; irregular */
        case 3:                                     /* switch 18 */
        case 2:                                     /* switch 18 */
            break;
        case 0:                                     /* switch 18 */
            var_a1 += 3;
            /* fallthrough */
        case 1:                                     /* switch 18 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x34:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_24 = *var_a1;
        switch (temp_v1_24) {                       /* switch 19; irregular */
        case 0:                                     /* switch 19 */
            var_a1 += 3;
            break;
        case 2:                                     /* switch 19 */
        case 1:                                     /* switch 19 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x35:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x38:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x39:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x3A:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x3B:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x3C:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x3D:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_25 = *var_a1;
        switch (temp_v1_25) {                       /* switch 20; irregular */
        case 0:                                     /* switch 20 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 20 */
        case 1:                                     /* switch 20 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x3E:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_26 = *var_a1;
        switch (temp_v1_26) {                       /* switch 21; irregular */
        case 0:                                     /* switch 21 */
            var_a1 += 3;
            /* fallthrough */
        case 1:                                     /* switch 21 */
            var_a1 += 2;
            break;
        case 3:                                     /* switch 21 */
        case 2:                                     /* switch 21 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x3F:                                      /* switch 1 */
        var_a1 += 3;
        break;
    case 0x40:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x41:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x42:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_27 = *var_a1;
        switch (temp_v1_27) {                       /* switch 22; irregular */
        case 0:                                     /* switch 22 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 22 */
        case 1:                                     /* switch 22 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x44:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x45:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x46:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_28 = *var_a1;
        switch (temp_v1_28) {                       /* switch 23; irregular */
        case 0:                                     /* switch 23 */
            var_a1 += 3;
            break;
        case 2:                                     /* switch 23 */
        case 1:                                     /* switch 23 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x47:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x48:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x49:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x4A:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x4B:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x4C:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x4D:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x50:                                      /* switch 1 */
    case 0x4F:                                      /* switch 1 */
    case 0x4E:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x51:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x52:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x53:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x54:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x55:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x56:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_29 = *var_a1;
        switch (temp_v1_29) {                       /* switch 24; irregular */
        case 0:                                     /* switch 24 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 24 */
        case 1:                                     /* switch 24 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x57:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_30 = *var_a1;
        switch (temp_v1_30) {                       /* switch 25; irregular */
        case 0:                                     /* switch 25 */
            var_a1 += 3;
            /* fallthrough */
        case 1:                                     /* switch 25 */
            var_a1 += 2;
            break;
        case 3:                                     /* switch 25 */
        case 2:                                     /* switch 25 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x58:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x59:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x5A:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_31 = *var_a1;
        switch (temp_v1_31) {                       /* switch 26; irregular */
        case 0:                                     /* switch 26 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 26 */
        case 1:                                     /* switch 26 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x5B:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x5C:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x5D:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x5E:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_32 = *var_a1;
        switch (temp_v1_32) {                       /* switch 27; irregular */
        case 0:                                     /* switch 27 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 27 */
        case 1:                                     /* switch 27 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x5F:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x60:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x61:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x62:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x63:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x64:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_33 = *var_a1;
        switch (temp_v1_33) {                       /* switch 28; irregular */
        case 0:                                     /* switch 28 */
            var_a1 += 2;
            break;
        case 2:                                     /* switch 28 */
        case 1:                                     /* switch 28 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x65:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x66:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x67:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x68:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x80:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_34 = *var_a1;
        switch (temp_v1_34) {                       /* switch 29; irregular */
        case 0x0:                                   /* switch 29 */
            var_a1 += 3;
            /* fallthrough */
        case 0xA:                                   /* switch 29 */
        case 0x9:                                   /* switch 29 */
        case 0x8:                                   /* switch 29 */
        case 0x7:                                   /* switch 29 */
        case 0x6:                                   /* switch 29 */
        case 0x5:                                   /* switch 29 */
        case 0x4:                                   /* switch 29 */
        case 0x3:                                   /* switch 29 */
        case 0x2:                                   /* switch 29 */
        case 0x1:                                   /* switch 29 */
            var_a1 += 2;
            break;
        case 0xFF:                                  /* switch 29 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x81:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x82:                                      /* switch 1 */
        var_a1 += 3;
        break;
    case 0x83:                                      /* switch 1 */
        var_a1 += 1;
        temp_v1_35 = *var_a1;
        switch (temp_v1_35) {                       /* switch 30; irregular */
        case 0x0:                                   /* switch 30 */
            var_a1 += 4;
            break;
        case 0xFF:                                  /* switch 30 */
        case 0x5:                                   /* switch 30 */
        case 0x4:                                   /* switch 30 */
        case 0x3:                                   /* switch 30 */
        case 0x2:                                   /* switch 30 */
        case 0x1:                                   /* switch 30 */
            var_a1 += 1;
            break;
        }
        break;
    case 0x84:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0xFF:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x90:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x91:                                      /* switch 1 */
        var_a1 += 2;
        break;
    case 0x93:                                      /* switch 1 */
    case 0x92:                                      /* switch 1 */
        var_a1 += 1;
        break;
    case 0x94:                                      /* switch 1 */
        temp_v1_36 = var_a1 + 1;
        var_a1 = temp_v1_36 + 1;
        temp_v1_37 = *temp_v1_36;
        switch (temp_v1_37) {                       /* switch 31; irregular */
        case 3:                                     /* switch 31 */
        case 2:                                     /* switch 31 */
            break;
        case 0:                                     /* switch 31 */
            var_a1 += 2;
            /* fallthrough */
        case 1:                                     /* switch 31 */
            var_a1 += 1;
            break;
        }
        break;
    default:                                        /* switch 1 */
        EM_FIELD(em, s8 *, 0x9DA) = 4;
        break;
    }
    return var_a1;
}

u8 *else_ck(EMW *em, u8 *p, int a2) {
    s32 temp_s0;
    s32 var_a1;
    void *var_a1_2;

    var_a1_2 = p;
    temp_s0 = a2 & 0xFF;
    if ((EM_FIELD(var_a1_2, u8 *, 0) == temp_s0) && (EM_FIELD(var_a1_2, u8 *, 1) == 2)) {
        var_a1 = next_cmd_search();
    } else {
loop_3:
        if (EM_FIELD(var_a1_2, u8 *, 0) != temp_s0) {
block_7:
            var_a1_2 = cmd_end_search(em, var_a1_2, a2, 2);
            goto loop_3;
        }
        if (EM_FIELD(var_a1_2, u8 *, 1) != 2) {
            goto block_7;
        }
        var_a1 = next_cmd_search(em, var_a1_2);
    }
    return var_a1;
}

void em_cmd_reset(EMW *em) {
    if (em->x8C3 == 0 && em->x86F == 0) {
        em_g_init_flag_set(em, 2);
    }
    em->x84E = 1;
    reset_flag_ck(em);
}

void NextStage_No_Set(void *arg0) {
    s8 sp148;
    f32 sp144;
    f32 sp13C;
    f32 sp138;
    f32 sp130;
    f32 spD0;
    u16 spC0;
    u16 spB0;
    u16 spA0;
    f32 *var_s1;
    f32 *var_s4;
    s32 temp_s6;
    s32 temp_v0;
    s32 var_a2;
    int temp_s1;
    int var_a3;
    int var_s0;
    int var_s0_2;
    int var_s3;
    int var_s5;
    int var_s5_2;
    s8 *temp_s5;
    s8 *var_s2;
    s8 temp_a0;
    s8 temp_a0_5;
    s8 temp_v1;
    u16 *var_s2_2;
    u16 *var_s3_2;
    u16 temp_a0_3;
    u16 temp_a0_4;
    u16 var_s7;
    u8 temp_a0_2;
    void *temp_v0_2;
    void *temp_v0_3;
    void *temp_v0_4;

    var_s7 = saved_reg_s7;
    if (EM_FIELD(arg0, u8 *, 0x92F) == 0xFF) {
        EM_FIELD(arg0, u8 *, 0x92F) = (u8) EM_FIELD(arg0, u16 *, 0x73A);
    }
    temp_v0 = st_mv_ptr_ck(arg0);
    var_a2 = 0;
    if (temp_v0 != 0) {
        var_a3 = 0;
loop_4:
        temp_v1 = *(temp_v0 + ((s8)(var_a3)));
        if (EM_FIELD(arg0, u16 *, 0x73A) == temp_v1) {
            var_a2 = 1;
        } else if (temp_v1 != -1) {
            var_a3 = (s8)(var_a3 + 1);
            if (var_a3 >= 8) {

            } else {
                goto loop_4;
            }
        }
    } else {
        var_a2 = 1;
    }
    if (var_a2 == 0) {
        temp_v0_2 = Stage_data_get(EM_FIELD(arg0, u8 *, 0x736));
        sp130 = EM_FIELD(temp_v0_2, f32 *, 0) + (EM_FIELD(temp_v0_2, f32 *, 0x10) / 2.0f);
        sp138 = EM_FIELD(temp_v0_2, f32 *, 4) + (EM_FIELD(temp_v0_2, f32 *, 0x14) / 2.0f);
        temp_v0_3 = Stage_data_get((u8) EM_FIELD(arg0, u16 *, 0x73A));
        sp13C = EM_FIELD(temp_v0_3, f32 *, 0) + (EM_FIELD(temp_v0_3, f32 *, 0x10) / 2.0f);
        sp144 = EM_FIELD(temp_v0_3, f32 *, 4) + (EM_FIELD(temp_v0_3, f32 *, 0x14) / 2.0f);
        var_s4 = &spD0;
        temp_s6 = st_mv_ptr_ck(arg0);
        var_s0 = 0;
        var_s3 = 0;
        var_s2 = &sp148;
        var_s1 = var_s4;
loop_12:
        temp_s5 = temp_s6 + ((s8)(var_s0));
        temp_a0 = *temp_s5;
        if (temp_a0 != -1) {
            temp_v0_4 = Stage_data_get((u8) temp_a0);
            var_s3 = (s8)(var_s3 + 1);
            *var_s2 = *temp_s5;
            var_s0 = (s8)(var_s0 + 1);
            var_s2 += 1;
            EM_FIELD(var_s1, f32 *, 0) = EM_FIELD(temp_v0_4, f32 *, 0) + (EM_FIELD(temp_v0_4, f32 *, 0x10) / 2.0f);
            EM_FIELD(var_s1, f32 *, 8) = (f32) (EM_FIELD(temp_v0_4, f32 *, 4) + (EM_FIELD(temp_v0_4, f32 *, 0x14) / 2.0f));
            var_s1 += 0xC;
            if (var_s0 >= 8) {

            } else {
                goto loop_12;
            }
        }
        temp_s1 = (s8)(var_s3);
        if (temp_s1 == 1) {
            EM_FIELD(arg0, u16 *, 0x73A) = (u16) sp148;
            return;
        }
        var_s5 = -1;
        spC0 = Em_Calc_angY(&sp130, &sp13C);
        var_s0_2 = 0;
        if (temp_s1 > 0) {
            var_s3_2 = &spB0;
            var_s2_2 = &spA0;
            do {
                temp_a0_2 = EM_FIELD(arg0, u8 *, 0x92E);
                if (temp_a0_2 != 0xFF) {
                    if (temp_a0_2 != *(temp_s6 + ((s8)(var_s0_2)))) {
                        goto block_22;
                    }
                } else {
block_22:
                    *var_s3_2 = Em_Calc_angY(&sp130, var_s4);
                    *var_s2_2 = *var_s3_2 - spC0;
                    temp_a0_3 = *var_s2_2;
                    if ((s32) temp_a0_3 >= 0x8001) {
                        *var_s2_2 = 0x10000 - temp_a0_3;
                    }
                    if (((s8)(var_s5)) == -1) {
                        var_s7 = *var_s2_2;
                        var_s5_2 = var_s0_2 << 0x38;
                        goto block_29;
                    }
                    temp_a0_4 = *var_s2_2;
                    if ((s32) temp_a0_4 < (var_s7 & 0xFFFF)) {
                        var_s5_2 = var_s0_2 << 0x38;
                        var_s7 = temp_a0_4;
block_29:
                        var_s5 = var_s5_2 >> 0x38;
                    }
                }
                var_s4 += 0xC;
                var_s0_2 = (s8)(var_s0_2 + 1);
                var_s3_2 += 2;
                var_s2_2 += 2;
            } while (var_s0_2 < temp_s1);
        }
        temp_a0_5 = *(temp_s6 + ((s8)(var_s5)));
        if (temp_a0_5 != -1) {
            EM_FIELD(arg0, u16 *, 0x73A) = (u16) temp_a0_5;
        }
    }
}

void NextStage_Dir_Set(void *arg0, void *arg1) {
    f32 temp_f10;
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f3;
    f32 temp_f4;
    f32 temp_f5;
    f32 temp_f9;
    int var_a0;
    void *temp_s2;
    void *temp_v0;

    temp_s2 = Stage_data_get(EM_FIELD(arg0, u16 *, 0x73A));
    temp_v0 = Stage_data_get((u16) EM_FIELD(arg0, u8 *, 0x736));
    var_a0 = 0;
    temp_f10 = EM_FIELD(temp_v0, f32 *, 0x10);
    temp_f9 = temp_f10 / 2.0f;
    temp_f4 = (EM_FIELD(temp_s2, f32 *, 4) + (EM_FIELD(temp_s2, f32 *, 0x14) / 2.0f)) - (EM_FIELD(temp_v0, f32 *, 4) + (EM_FIELD(temp_v0, f32 *, 0x14) / 2.0f));
    temp_f5 = (EM_FIELD(temp_s2, f32 *, 0) + (EM_FIELD(temp_s2, f32 *, 0x10) / 2.0f)) - (EM_FIELD(temp_v0, f32 *, 0) + temp_f9);
    if (temp_f5 <= (temp_f10 / 3.0f)) {
        EM_FIELD(arg1, f32 *, 0) = 0.0f;
    } else if (temp_f5 <= ((2.0f * temp_f10) / 3.0f)) {
        EM_FIELD(arg1, f32 *, 0) = temp_f9;
        var_a0 = 1;
    } else {
        EM_FIELD(arg1, f32 *, 0) = temp_f10;
    }
    temp_f3 = EM_FIELD(temp_v0, f32 *, 0x14);
    if (temp_f4 <= (temp_f3 / 3.0f)) {
        EM_FIELD(arg1, f32 *, 8) = 0.0f;
    } else if (temp_f4 <= ((2.0f * temp_f3) / 3.0f)) {
        var_a0 = (s8)(var_a0 | 2);
        EM_FIELD(arg1, f32 *, 8) = (f32) (temp_f3 / 2.0f);
    } else {
        EM_FIELD(arg1, f32 *, 8) = temp_f3;
    }
    if (((s8)(var_a0)) == 3) {
        temp_f1 = EM_FIELD(temp_v0, f32 *, 0x10);
        if (temp_f5 <= (temp_f1 / 2.0f)) {
            EM_FIELD(arg1, f32 *, 0) = 0.0f;
        } else {
            EM_FIELD(arg1, f32 *, 0) = temp_f1;
        }
        temp_f1_2 = EM_FIELD(temp_v0, f32 *, 0x14);
        if (temp_f4 <= (temp_f1_2 / 2.0f)) {
            EM_FIELD(arg1, f32 *, 8) = 0.0f;
        } else {
            EM_FIELD(arg1, f32 *, 8) = temp_f1_2;
        }
    }
    EM_FIELD(arg1, f32 *, 4) = (f32) *(&area_move_high_y_tbl + (EM_FIELD(arg0, u8 *, 0x736) * 4));
}

void em_cdm_act_flag_ck(void *arg0) {
    s32 var_t0;
    s32 var_t1;
    s8 var_a2;
    u8 temp_a2;
    u8 temp_a3;
    u8 temp_v1;

    temp_v1 = EM_FIELD(arg0, u8 *, 0x82B);
    switch (temp_v1) {                              /* irregular */
    case 0:
        EM_FIELD(arg0, s8 *, 0x880) = 0;
        return;
    case 1:
        temp_a2 = *(u8 *)0x3F34C3;
        var_t1 = 0;
        var_t0 = 0;
        if ((s32) temp_a2 > 0) {
            do {
                if (EM_FIELD(arg0, u8 *, 0x914) & (1 << var_t1)) {
                    var_t0 += 1;
                }
                var_t1 += 1;
            } while (var_t1 < (s32) temp_a2);
        }
        if (var_t0 == 0) {
            EM_FIELD(arg0, s8 *, 0x880) = 0;
            return;
        }
        EM_FIELD(arg0, s8 *, 0x880) = 1;
        EM_FIELD(arg0, s8 *, 0x881) = 1;
        EM_FIELD(arg0, s8 *, 0x882) = 0;
        temp_a3 = *(void *)0x3F34C3;
        var_a2 = 0;
        if ((s32) temp_a3 > 0) {
loop_13:
            if (!(EM_FIELD(arg0, u8 *, 0x914) & (1 << var_a2))) {
                var_a2 += 1;
                if (var_a2 < (s32) temp_a3) {
                    goto loop_13;
                }
            }
        }
        EM_FIELD(arg0, s8 *, 0x883) = var_a2;
        return;
    }
}

u8 *area_route_rnd32(EMW *em, int a1) {
    s32 var_t0;
    s32 var_t1;
    u8 temp_a0;
    u8 temp_a2;
    void *var_a1;

    temp_a2 = EM_FIELD(a1, u8 *, 2);
    var_t0 = 0;
    var_t1 = 0;
    var_a1 = a1 + 3;
    if ((s32) temp_a2 > 0) {
loop_2:
        temp_a0 = EM_FIELD(var_a1, u8 *, 2);
        var_a1 += 3;
        if (temp_a0 == 0) {
            goto block_7;
        }
        if ((temp_a0 & 0xFF) != 0xFF) {
            var_t0 = (var_t0 + temp_a0) & 0xFF;
            if ((em->x39A & 0x1F & 0xFFFF) >= var_t0) {
block_7:
                var_a1 += 1;
                var_t1 = (var_t1 + 1) & 0xFFFF;
                if (var_t1 >= (s32) temp_a2) {

                } else {
                    goto loop_2;
                }
            }
        }
    }
    return var_a1;
}

u8 *set_cmd(EMW *em) {
    u8 f = em->x83B;
    if (f & 0x20) {
        return em->cmd_find;
    }
    if (f & 0x10) {
        return em->cmd_kehai;
    }
    if (f & 1) {
        return em->cmd_route;
    }
    return em->cmd_pc;
}

void ret_cmd(EMW *em, u8 *p) {
    u8 f = em->x83B;
    if (f & 0x20) {
        em->cmd_find = p;
        return;
    }
    if (f & 0x10) {
        em->cmd_kehai = p;
        return;
    }
    if (f & 1) {
        em->cmd_route = p;
        return;
    }
    em->cmd_pc = p;
}

u8 *route_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[2];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *kehai_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[3];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *find_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[4];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *smell_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[8];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *yobi_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[10];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *ikari_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[11];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *action_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[9];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *area_route_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[5];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *area_move_ptr_set(EMW *em, u8 n) {
    return em->cmd_tbl[6][n];
}

u8 *option_route_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[2];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *ground_area_move_ptr_set(EMW *em, u8 n) {
    u8 **t = em->cmd_tbl[12];
    if (t == 0) {
        return 0;
    }
    return t[n];
}

u8 *no_floor_ptr_set(EMW *em) {
    u8 **t = em->cmd_tbl[13];
    if (t == 0) {
        return 0;
    }
    return *t;
}

u8 *unko_ptr_set(EMW *em) {
    u8 **t = em->cmd_tbl[14];
    if (t == 0) {
        return 0;
    }
    return *t;
}

u8 *cancel_prog_ck(EMW *em) {
    s32 var_s0;
    u8 temp_v0;

    temp_v0 = em->x917;
    if (temp_v0 == 0) {
        return 0;
    }
    var_s0 = 0;
    if ((temp_v0 & 0xFF & 0x40) && (em_cancel_act_ck((void *)0x40) == 0)) {
        var_s0 = unko_ptr_set(em);
        em->x917 = 0U;
        em->x83B = 0x40U;
    }
    if ((em->x917 & 0x80) && (em_cancel_act_ck(em, 0x80) == 0) && (var_s0 == 0)) {
        var_s0 = no_floor_ptr_set(em);
        em->x917 = 0U;
        em->x83B = 0x80U;
    }
    if ((em->x917 & 0x20) && (em_cancel_act_ck(em, 0x20) == 0)) {
        EM_FIELD(em, s32 *, 0x83C) = find_ptr_set(em, 0);
        if (var_s0 == 0) {
            var_s0 = EM_FIELD(em, s32 *, 0x83C);
        }
        em->x917 = (u8) (em->x917 & 0xCE);
        em->x83B = (u8) (em->x83B | 0x20);
    }
    if ((em->x917 & 0x10) && (em_cancel_act_ck(em, 0x10) == 0)) {
        EM_FIELD(em, s32 *, 0x834) = kehai_ptr_set(em, 0);
        if (var_s0 == 0) {
            var_s0 = EM_FIELD(em, s32 *, 0x834);
        }
        em->x917 = (u8) (em->x917 & 0xEF);
        em->x83B = (u8) (em->x83B | 0x10);
    }
    if ((em->x917 & 8) && (em_cancel_act_ck(em, 8) == 0)) {
        EM_FIELD(em, s32 *, 0x864) = ikari_ptr_set(em, 0);
        if (var_s0 == 0) {
            var_s0 = EM_FIELD(em, s32 *, 0x864);
        }
        em->x917 = (u8) (em->x917 & 0xF7);
        em->x83B = (u8) (em->x83B | 8);
    }
    if ((em->x917 & 4) && (em_cancel_act_ck(em, 4) == 0)) {
        EM_FIELD(em, s32 *, 0x860) = yobi_ptr_set(em, 0);
        if (var_s0 == 0) {
            var_s0 = EM_FIELD(em, s32 *, 0x860);
        }
        em->x917 = (u8) (em->x917 & 0xC8);
        em->x83B = (u8) (em->x83B | 4);
    }
    if ((em->x917 & 2) && (em_cancel_act_ck(em, 2) == 0)) {
        EM_FIELD(em, s32 *, 0x85C) = smell_ptr_set(em, 0);
        if (var_s0 == 0) {
            var_s0 = EM_FIELD(em, s32 *, 0x85C);
        }
        em->x917 = (u8) (em->x917 & 0xCC);
        em->x83B = (u8) (em->x83B | 2);
    }
    if (var_s0 != 0) {
        EM_FIELD(em, s32 *, 0x870) = var_s0;
    }
    return var_s0;
}

void reset_flag_ck(EMW *em) {
    if (EM_FIELD(em, u8 *, 0x84E) != 0) {
        EM_FIELD(em, s8 *, 0x839) = 0;
        em->x84E = 0;
        EM_FIELD(em, s8 *, 0x83B) = 0;
        em->cmd_idx = 0;
        em->cmd_pc = em_cmd_top(em);
        em->x928 = -1;
        em->x929 = -1;
        em->x92D = -1;
        em->x92C = -1;
        em->x92F = 0xFF;
        em->x92E = 0xFF;
        em->x854 = 0;
    }
}

