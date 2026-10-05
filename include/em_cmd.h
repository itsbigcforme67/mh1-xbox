#ifndef EM_CMD_H
#define EM_CMD_H
/* Monster command interpreter (game.bin 0x55B060-0x55E9FC, src/game/em/em_cmd*.c).
 * Every monster runs a byte-code program; handlers take the program pointer after the
 * opcode and return the pointer to continue at. */
#include "em.h"
#include "game.h"
#include "pl.h"
#include "fl.h"

#ifndef NULL
#define NULL ((void *)0)
#endif
#define EM_FIELD(p, T, o) (*(T)((u8 *)(p) + (o)))

/* Condition failed: skip to the matching else (mode 1) or end (mode 2) marker of command
 * CODE, then past it (and past a following end marker). q is the program pointer. */
#define CMD_SKIP(em, q, code)                                      \
    for (;;) {                                                     \
        if ((q)[0] == (code) && (q)[1] == 1) {                     \
            break;                                                 \
        }                                                          \
        if ((q)[0] == (code) && (q)[1] == 2) {                     \
            break;                                                 \
        }                                                          \
        (q) = cmd_end_search(em, q, code, 2);                      \
    }                                                              \
    (q) = next_cmd_search(em, q);                                  \
    if ((q)[0] == (code) && (q)[1] == 2) {                         \
        (q) = next_cmd_search(em, q);                              \
    }

/* Select command (code CODE): n entries follow, each a value byte and a body; run the body
 * whose value equals CUR, else skip the whole command. Modes 1/2: skip to the end marker. */
#define CMD_SEL_FUNC(name, code, vtype, ctype, cur)                                      \
u8 *name(EMW *em, u8 *p) {                                                     \
    u8 *q;                                                                     \
    u8 *r;                                                                     \
    u8 n;                                                                      \
    vtype v;                                                                   \
    ctype c;                                                                   \
    s32 i;                                                                     \
    u16 more;                                                                  \
                                                                               \
    q = p;                                                                     \
    switch (*q++) {                                                            \
    case 0:                                                                    \
        n = *q;                                                                \
        q += 3;                                                                \
        for (i = 0; i < n; i++) {                                              \
            v = *q;                                                            \
            c = (cur);                                                         \
            q += 1;                                                            \
            if (c == v) {                                                      \
                break;                                                         \
            }                                                                  \
            if (v < c) {                                                       \
                r = cmd_end_search(em, q, code, 3);                            \
                q = r;                                                         \
                if (r[1] == 2 || r[1] == 3) {                                  \
                    q = q + 2;                                                 \
                    break;                                                     \
                }                                                              \
                q = q + 2;                                                     \
            } else {                                                           \
                more = 1;                                                      \
                do {                                                           \
                    r = cmd_end_search(em, q, code, 3);                        \
                    if ((r[0] == (code) && r[1] == 2) ||                       \
                        (r[0] == (code) && r[1] == 3)) {                       \
                        more = 0;                                              \
                    }                                                          \
                    q = next_cmd_search(em, r);                                \
                } while (more);                                                \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        break;                                                                 \
    case 1:                                                                    \
        q += 1;                                                                \
    case 2:                                                                    \
        more = 1;                                                              \
        do {                                                                   \
            q = cmd_end_search(em, q, code, 3);                                \
            if (q[0] == (code) && q[1] == 3) {                                 \
                more = 0;                                                      \
            }                                                                  \
            q = next_cmd_search(em, q);                                        \
        } while (more);                                                        \
        break;                                                                 \
    case 3:                                                                    \
        break;                                                                 \
    }                                                                          \
    return q;                                                                  \
}

/* Same, but a current value of 0xFF (none) never matches. */
#define CMD_SEL_FUNC_W(name, code, vtype, ctype, cur)                                      \
u8 *name(EMW *em, u8 *p) {                                                     \
    u8 *q;                                                                     \
    u8 *r;                                                                     \
    u8 n;                                                                      \
    vtype v;                                                                   \
    ctype c;                                                                   \
    s32 i;                                                                     \
    u16 more;                                                                  \
                                                                               \
    q = p;                                                                     \
    switch (*q++) {                                                            \
    case 0:                                                                    \
        n = *q;                                                                \
        q += 3;                                                                \
        for (i = 0; i < n; i++) {                                              \
            v = *q;                                                            \
            c = (cur);                                                         \
            q += 1;                                                            \
            if (c == v && c != 0xFF) {                                                      \
                break;                                                         \
            }                                                                  \
            if (v < c && c != 0xFF) {                                                       \
                r = cmd_end_search(em, q, code, 3);                            \
                q = r;                                                         \
                if (r[1] == 2 || r[1] == 3) {                                  \
                    q = q + 2;                                                 \
                    break;                                                     \
                }                                                              \
                q = q + 2;                                                     \
            } else {                                                           \
                more = 1;                                                      \
                do {                                                           \
                    r = cmd_end_search(em, q, code, 3);                        \
                    if ((r[0] == (code) && r[1] == 2) ||                       \
                        (r[0] == (code) && r[1] == 3)) {                       \
                        more = 0;                                              \
                    }                                                          \
                    q = next_cmd_search(em, r);                                \
                } while (more);                                                \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        break;                                                                 \
    case 1:                                                                    \
        q += 1;                                                                \
    case 2:                                                                    \
        more = 1;                                                              \
        do {                                                                   \
            q = cmd_end_search(em, q, code, 3);                                \
            if (q[0] == (code) && q[1] == 3) {                                 \
                more = 0;                                                      \
            }                                                                  \
            q = next_cmd_search(em, q);                                        \
        } while (more);                                                        \
        break;                                                                 \
    case 3:                                                                    \
        break;                                                                 \
    }                                                                          \
    return q;                                                                  \
}

/* tables of programs, one per monster kind (version picked by the byte at 0x3F341E) */
extern u8 ***em_cmd0_tbl[], ***em_cmd1_tbl[], ***em_cmd2_tbl[], ***em_cmd3_tbl[];
extern u8 ***em_cmd4_tbl[], ***em_cmd5_tbl[], ***em_cmd6_tbl[];
extern u8 *em_area_mv_tbl[];
extern u8 check_hate_tbl[];
extern u8 area_move_high_y_tbl[];
extern s16 em02_runaway_timer_tbl[];
extern s16 em_atk_mode_timer_tbl[];
extern s32 em_atk_bit;

u8 *em_cmd_kehai_ck(EMW *, u8 *);
u8 *em_cmd_target_set(EMW *, u8 *);
u8 *em_cmd_dansa_sel(EMW *, u8 *);
u8 *em_cmd_ninshiki_ck(EMW *, u8 *);
u8 *em_cmd_area_move_ck(EMW *, u8 *);
u8 *em_cmd_main_jump(EMW *, u8 *);
u8 *em_cmd_stand_ck(EMW *, u8 *);
u8 *em_cmd_fly_ck(EMW *, u8 *);
u8 *em_cmd_body_status_set(EMW *, u8 *);
u8 *em_cmd_mode_ck(EMW *, u8 *);
u8 *em_cmd_flag_set(EMW *, u8 *);
u8 *em_cmd_flag_clear(EMW *, u8 *);
u8 *em_cmd_stage_no_ck(EMW *, u8 *);
u8 *em_cmd_route_set(EMW *, u8 *);
u8 *em_cmd_route_ck(EMW *, u8 *);
u8 *em_cmd_kehai_pl_set(EMW *, u8 *);
u8 *em_cmd_find_ck(EMW *, u8 *);
u8 *em_cmd_pl_target_set(EMW *, u8 *);
u8 *em_cmd_angle_ck(EMW *, u8 *);
u8 *em_cmd_stage_no_sel(EMW *, u8 *);
u8 *em_cmd_action_set(EMW *, u8 *);
u8 *em_cmd_area_route_set(EMW *, u8 *);
u8 *em_cmd_area_route_move(EMW *, u8 *);
u8 *em_cmd_area_route_ck(EMW *, u8 *);
u8 *em_cmd_escape_area_set(EMW *, u8 *);
u8 *em_cmd_mind_ck(EMW *, u8 *);
u8 *em_cmd_mind_no_ck(EMW *, u8 *);
u8 *em_cmd_mind_sel(EMW *, u8 *);
u8 *em_cmd_mind_move_end(EMW *, u8 *);
u8 *em_cmd_pl_ang_sel(EMW *, u8 *);
u8 *em_cmd_thirst_ck(EMW *, u8 *);
u8 *em_cmd_near_pos_ck(EMW *, u8 *);
u8 *em_cmd_emtype_ck(EMW *, u8 *);
u8 *em_cmd_repeat_cnt_set(EMW *, u8 *);
u8 *em_cmd_repeat_cnt_clr(EMW *, u8 *);
u8 *em_cmd_demo_flag_set(EMW *, u8 *);
u8 *em_cmd_type_sel(EMW *, u8 *);
u8 *em_cmd_all_pl_same_stage_ck(EMW *, u8 *);
u8 *em_cmd_stay_timer_ck(EMW *, u8 *);
u8 *em_cmd_runaway_timer_ck(EMW *, u8 *);
u8 *em_cmd_flag_ck(EMW *, u8 *);
u8 *em_cmd_myemtype_sel(EMW *, u8 *);
u8 *em_cmd_smell_set(EMW *, u8 *);
u8 *em_cmd_search_data_set(EMW *, u8 *);
u8 *em_cmd_egg_ck(EMW *, u8 *);
u8 *em_cmd_egg_cancel_ck(EMW *, u8 *);
u8 *em_cmd_yobi_pos_set(EMW *, u8 *);
u8 *em_cmd_body_status_ck(EMW *, u8 *);
u8 *em_cmd_body_status_sel(EMW *, u8 *);
u8 *em_cmd_act_st_ck(EMW *, u8 *);
u8 *em_cmd_ikari_ck(EMW *, u8 *);
u8 *em_cmd_water_ck(EMW *, u8 *);
u8 *em_cmd_eye_dmg_ck(EMW *, u8 *);
u8 *em_cmd_sensor_ck(EMW *, u8 *);
u8 *em_cmd_boss_work_ck(EMW *, u8 *);
u8 *em_cmd_boss_atk_ck(EMW *, u8 *);
u8 *em_cmd_before_stage_ck(EMW *, u8 *);
u8 *em_cmd_before_stage_sel(EMW *, u8 *);
u8 *em_cmd_ground_area_move(EMW *, u8 *);
u8 *em_cmd_em_mode_change(EMW *, u8 *);
u8 *em_cmd_em_hp_vital_add(EMW *, u8 *);
u8 *em_cmd_horm_pos_ang_ck(EMW *, u8 *);
u8 *em_cmd_boss_same_stage_ck(EMW *, u8 *);
u8 *em_cmd_pl_fishing_ck(EMW *, u8 *);
u8 *em_cmd_target_pl_act_ck(EMW *, u8 *);
u8 *em_cmd_fish_ok_ck(EMW *, u8 *);
u8 *em_cmd_timer_set(EMW *, u8 *);
u8 *em_cmd_pl_land_target(EMW *, u8 *);
u8 *em_cmd_pl_look_ck(EMW *, u8 *);
u8 *em_cmd_kehai_clear(EMW *, u8 *);
u8 *em_cmd_hate_clear(EMW *, u8 *);
u8 *em_cmd_horm_pos_set(EMW *, u8 *);
u8 *em_cmd_thirst_add(EMW *, u8 *);
u8 *em_cmd_hungry_add(EMW *, u8 *);
u8 *em_cmd_suimin_add(EMW *, u8 *);
u8 *em_cmd_swim_ck(EMW *, u8 *);
u8 *em_cmd_all_pl_target_sel(EMW *, u8 *);
u8 *em_cmd_samestage_pl_target_sel(EMW *, u8 *);
u8 *em_cmd_target_pl_samestage_ck(EMW *, u8 *);
u8 *em_cmd_target_pl_hate_high_ck(EMW *, u8 *);
u8 *em_cmd_quest_no_ck(EMW *, u8 *);
u8 *em_cmd_quest_no_sel(EMW *, u8 *);
u8 *em_cmd_boss_pl_target_set(EMW *, u8 *);
u8 *em_cmd_tenjo_ck(EMW *, u8 *);
u8 *em_cmd_target_land_no_ck(EMW *, u8 *);
u8 *em_cmd_ninshiki_timer_sub(EMW *, u8 *);
u8 *em_cmd_tenjostage_ck(EMW *, u8 *);
u8 *em_cmd_smell_set_ck(EMW *, u8 *);
u8 *em_cmd_my_floor_ck(EMW *, u8 *);
u8 *em_cmd_st25_pl_target_sel(EMW *, u8 *);
u8 *em_cmd_st25_gate_ck(EMW *, u8 *);
u8 *em_cmd_runaway_timer_set(EMW *, u8 *);
u8 *em_cmd_male_ck(EMW *, u8 *);
u8 *em_cmd_target_pl_hate_ck(EMW *, u8 *);
u8 *em_cmd_all_pl_hate_clear(EMW *, u8 *);
u8 *em_cmd_pl_ride_ck(EMW *, u8 *);
u8 *em_cmd_em_master_ck(EMW *, u8 *);
u8 *em_cmd_em_cmd_reset(EMW *, u8 *);
u8 *em_cmd_rnd32(EMW *, u8 *);
u8 *em_cmd_contents(EMW *, u8 *);
u8 *em_cmd_sub_contents(EMW *, u8 *);
u8 *em_cmd_range_ck(EMW *, u8 *);
u8 *em_cmd_end_command(EMW *, u8 *);
u8 *em_cmd_position_set(EMW *, u8 *);
u8 *em_cmd_vec_set(EMW *, u8 *);
u8 *em_cmd_demo_start(EMW *, u8 *);
u8 *em_cmd_wait_set(EMW *, u8 *);
u8 *em_cmd_em_atk_bit(EMW *, u8 *);

void em_cmd_init(EMW *);
void em_cmd_ck(EMW *);
u8 *em_cmd_top(EMW *);
void em_cmd_reset(EMW *);

u8 *set_cmd(EMW *);
void ret_cmd(EMW *, u8 *);
u8 *else_ck(EMW *, u8 *, int);
u8 *cmd_end_search(EMW *, u8 *, int, int);
u8 *next_cmd_search(EMW *, u8 *);
u8 *cancel_prog_ck(EMW *);
void reset_flag_ck(EMW *);
u8 *route_ptr_set(EMW *, u8);
u8 *kehai_ptr_set(EMW *, u8);
u8 *find_ptr_set(EMW *, u8);
u8 *smell_ptr_set(EMW *, u8);
u8 *yobi_ptr_set(EMW *, u8);
u8 *ikari_ptr_set(EMW *, u8);
u8 *action_ptr_set(EMW *, u8);
u8 *area_route_ptr_set(EMW *, u8);
u8 *area_move_ptr_set(EMW *, u8);
u8 *option_route_ptr_set(EMW *, u8);
u8 *ground_area_move_ptr_set(EMW *, u8);
u8 *no_floor_ptr_set(EMW *);
u8 *unko_ptr_set(EMW *);
u8 *area_route_rnd32(EMW *, int);

f32 CalcDistanceXZ(f32 *, f32 *);
void cmd_target_kind_set(EMW *, f32 *);
void em_area_move_init(EMW *);
void em_g_init_flag_set(EMW *, u8);
void em_hp_add(EMW *, int);
void em_hungry_add(EMW *, s32);
void em_thirst_add(EMW *, s32);
void em_suimin_add(EMW *, s32);
void Em_Mode_Chg(EMW *, int, int);
int em_pl_pos_set(EMW *, u8, f32 *);
void em_search_data_set(EMW *, u8);
void em_type_act_set(EMW *, int, u16, u16);
void World_calc2(u8, f32 *, f32 *);
void SetVector(f32 *, f32, f32, f32);
u8 GetTenjoHit(f32 *, f32 *, u16 *);
int GetWallHitLine(f32 *, f32 *, f32 *, u16);
int GetWaterData();
int Pl_stg_ck_tw(EMW *, PLW *);
int pl_flag_ck(PLW *, int);
int st_mv_ptr_ck(EMW *);
s16 act_ck(EMW *, u16, u16);
EM_STG_POS *gp_ck(EMW *, EM_STG_POS *, s16);
void *Stage_data_get(u8);
int em_cancel_act_ck(EMW *, u8);
u16 Em_Calc_angY(f32 *, f32 *);
u32 ran_suu(int);
#endif
