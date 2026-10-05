/* em_cmd_r01 - monster command interpreter 0x0055B060-0x0055C434: em_cmd_init, em_cmd_ck, em_cmd_kehai_ck, em_cmd_ninshiki_ck. Whole file in em_cmd_nm.c. */
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
