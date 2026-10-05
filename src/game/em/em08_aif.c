/* em08_ai, run 6: em_fly10_0059D720 .. em_atk08_0059F910 (game.bin 0x0059D720-0x0059FAA4). Matching functions of em08_ai_nm.c (that file holds the
 * whole code including the near-matches); see it for the description. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"

/* Raw access to EMW fields that em.h does not name yet. */
#define EMF(em, T, o) (*(T *)((u8 *)(em) + (o)))

/* em08's part of the per-monster work at EMW+0x444. */
typedef struct EM08W {
    u8 _pad00[4];
    s16 anim;           /* 0x04 animation the sound/effect script follows */
    u8 _pad06;
    u8 x07;             /* 0x07 */
    u8 _pad08[2];
    u16 dang;           /* 0x0A angle left to turn */
    u8 _pad0C;
    u8 has_tgt;         /* 0x0D */
    u8 _pad0E;
    u8 xF;              /* 0x0F */
    s16 x10;            /* 0x10 */
    u8 _pad12[2];
    s32 x14;            /* 0x14 */
    s32 x18;            /* 0x18 */
    s32 vel[3];         /* 0x1C speed vector */
    s32 x28;            /* 0x28 */
    s32 x2C;            /* 0x2C */
    s32 x30;            /* 0x30 */
    f32 dist;           /* 0x34 distance left to walk */
    s8 x38;             /* 0x38 */
    s8 x39;             /* 0x39 */
    s8 x3A;             /* 0x3A */
    u8 _pad3B[7];
    s16 tm[4];          /* 0x42 hire (helper cat) per-slot timers */
    struct {
        u8 a;
        u8 b;
    } st[4];            /* 0x4A hire slot states */
    struct {
        u16 a;
        u16 b;
    } ang[4];           /* 0x52 hire slot angles */
    s16 cnt[4];         /* 0x62 */
    s8 tmr[4];          /* 0x6A */
} EM08W;

typedef struct HIRE_E {
    u16 t;
    u16 v;
} HIRE_E;

extern GAME_W game_w;

void Eft19_set(EMW *, int, int);
void eft09_set(EMW *, int);
void Eft20_set(f32, EMW *, int, int);
void shell01_set(EMW *, int);
s16 em_hp_vital_set2(EMW *, s16, s16);
int em_act_search(void *);
void em_char_set(EMW *, int, int, int);
void em_char_set2();
void em_char_set2();
void em_act_set(EMW *, int, u16);
int em_frame_check(EMW *, f32, int);
int em_frame_check2(EMW *, int, f32);
u16 Em_Calc_angY(f32 *, f32 *);
void Em_set_quake_sub(EMW *, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);
void em_hp_add(EMW *, s16);
void em_range_set(EMW *, s8);
void em_search_data_set(EMW *, u8);
void em_thirst_add(EMW *, s32);
void em_thirst_end(EMW *);
void em_hungry_add(EMW *, s32);
void em_niku_eat_set(EMW *);
void Em_Suimin_Start(EMW *);
void Em_Sleep_Start(EMW *);
void em_hinshi_end(EMW *);
int em_sleep_hp_add(EMW *, s16, s16, s16);
void cmd_target_kind_set(EMW *, f32 *);
void target_kind_set(EMW *, f32 *);
void Em_Sleep2_Start(EMW *);
void Em_Sleep2_End(EMW *);
void Em_Sleep_End(EMW *);
void Em_Mahi_End(EMW *);
void em_suimin_end(EMW *);
void em_hungry_end(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void mot_miration_ret(EMW *, f32 *);
void em_rate_clear(EMW *);
void em_rate_clear_g(EMW *);
u16 senkai_target(EMW *);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
void xang_calc_target(EMW *, int *, f32, f32);
void xang_calc_pl(EMW *, int *, f32, f32);
f32 CalcDistanceXZ(f32 *, f32 *);
void NextStage_Dir_Set(EMW *, f32 *);
void Em_Next_Stage_Pos();
int AreaFieldInCheck(u8, f32 *);
void WyvernAreaMove(PLW *);
typedef struct FLYNEED {
    u8 _pad00[0x14];
    s32 x14;
} FLYNEED;
extern FLYNEED *em_thirst_tbl[];
extern FLYNEED *em_hungry_tbl[];
typedef struct STAGE_DATA {
    u8 _pad00[0x10];
    f32 width;          /* 0x10 */
    f32 depth;          /* 0x14 */
    f32 floor_y;        /* 0x18 */
} STAGE_DATA;
typedef f32 (*EM_POSP)[3];
STAGE_DATA *Stage_data_get(u8);
EM_POSP gp_ptr_ck(EMW *em, EM_STG_POS *p);
void em_area_move_init(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void eft11_set(EMW *, f32 *, int);
int em_pl_pos_set(EMW *, u8, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void Eft17_set(EMW *, int, int, int);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
int ran_suu();
void em_action_timer_calc(EMW *, int);
f32 flSqrt(f32);
void SetVector(f32 *, f32, f32, f32);
void senkai_player(EMW *);
s8 smell_search(EMW *, int, f32 *);
void ground_point_search(EMW *);
void xang_set_pl(EMW *, int, f32);
int Pl_stg_ck_tw(EMW *, PLW *);
int em_target_pl_samestage_ck(EMW *);
void World_calc2(u8, f32 *, f32 *);
void em_cmd_reset(EMW *);
void Quest_enemy_hagi_set(EMW *, int);
void Em_Sleep_Flag_Ck(EMW *);
void em_ana_loop_cnt_set(EMW *);
void em_mahi_eff_set(EMW *, int);
void em_tail_off_sub(EMW *);
void wyvern_kill_cnt_up(void *, int);
extern EMW em_work[];
int em08_fly_adjy2(EMW *);
void em08_fly_adjy2_init(EMW *, int);
void Eft08_set(f32 *, int, int, f32);
void shell21_set(EMW *, int);
void shell23_set(EMW *, int);
void vib_set_pl(PLW *, int);
int Code_Make(int, int, int, int);
extern u16 hire_down_angx_003887E8[4];
extern s16 hire_start_timer_tbl0_003887C8[4];
extern s16 hire_start_timer_tbl1_003887D0[4];
extern s16 hire_remove_timer_tbl0_003887D8[4];
extern s16 hire_remove_timer_tbl1_003887E0[4];
extern HIRE_E *hire_normal_add_tbl_00659250[4];
extern HIRE_E *hire_down_add_tbl_00659300[4];
void em08_effect_move(EMW *em);
void hire_req_set_005A6F90(EMW *em, EM08W *w, u8 req);
void atk_shell_set(EMW *em, int no);
void em08_vib_set(EMW *em);
void ground_land_eff_set_005A7050(EMW *em);
void swim_eff_set_005A7070(f32 scale, EMW *em);
void swim_eff_set2_005A7120(f32 scale, EMW *em);
void em21_target_ang_calc(EMW *em, int arg1);
void em08_uvmove(EMW *em);
void sound_call_sub_005A3CA0(EMW *em, int se, int joint);
void sound_call_005A3D10(EMW *em, int frame, int se, int joint);
void quake_call_005A3DB0(EMW *em, int frame, int v);
void move_default_005A3E00(EMW *em);
void ef_move_sub_005A3E50(EMW *em, EM08W *w);

extern s16 em08_stay_timer_tbl[];
extern s16 em08_runaway_timer_tbl[];
void em08_act_set(EMW *em, int kind, u16 no, u16 arg);
void em08_to_normal(EMW *em);
void em08_to_swim(EMW *em);
int em08_act_sub(EMW *em, int arg1);
void em08_main_sub(EMW *em, EM08W *w);
void Quest_enemy_capture();
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void cpRotMatrixYXZ2(s32 *, FLMAT *);
void Eft13_set_em(EMW *, int, int);
int Event_flag_ck(int);
int em_mode_timer_sub(EMW *);
void em_no_floor_ck(EMW *);
void em_hinshi_ck(EMW *, f32);
void em_egg_ck(EMW *);
void em_thirst_ck(EMW *);
void em_hungry_ck(EMW *);
void em_sleep_ck(EMW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void em_sleep2_dmg_timer_set(EMW *);
int em_hokaku_ck(EMW *, f32);
void em_ikari_add(EMW *, s16);
void em_cmd_ck(EMW *);
void em_dur_set(EMW *, int);
void Eft13_set_em_scl(EMW *, int, f32, int);
void Eft15_set3(EMW *, int, f32, int);
int em_frame_check3(EMW *, int, f32, f32);

extern s8 hagi_tbl_003886B0[3][2];
extern f32 hagi_r_tbl_00657788[3];
void get_joint_pos_em(EMW *, int, f32 *);
f32 *get_joint_wmat_em(EMW *, int);
void flvecApplyMat33(f32 *, f32 *, f32 *);
void Ext_pick_point_st(int, int);
int Ext_pick_point_set(STIEM *, f32 *);
void Ext_pick_point_pos(int, f32 *);
int Ext_pick_point_cnt_ck(int);
void Ext_pick_point_clr(int);
int softdip_ck(int);
void eft01_set(PLW *, int);
void em_uvmove(EMW *em);
void net_send_em(EMW *, int, int);
int Quest_clear_ck(int);
void RedDragonEscapeCamera(EMW *);
void Eft02_set3(f32, EMW *, int, int, int, f32 *);
void Eft13_set_pos2(f32, EMW *, f32 *, int);
void Eft10_set(f32, EMW *, int, int);
void Shell22_set3(EMW *, int, int);
void shell05_set4(EMW *, int, int);
void bridge_eff_set(EMW *);
void toride_eff_set(EMW *);

#define EM08_TURN(em, tgt)                                                \
    do {                                                                  \
        int d;                                                            \
        d = (u16)((u16)(tgt) - (em)->ang[1]);                             \
        if (d <= 0x8000) {                                                \
            if (d <= 0x3F) {                                              \
                (em)->ang[1] += d;                                        \
            } else {                                                      \
                (em)->ang[1] += 0x40;                                     \
            }                                                             \
        } else if (d > 0xFFC0) {                                          \
            (em)->ang[1] += d;                                            \
        } else {                                                          \
            (em)->ang[1] -= 0x40;                                         \
        }                                                                 \
    } while (0)

#define EM08_TURNN(em, tgt, st)                                           \
    do {                                                                  \
        int d;                                                            \
        d = (u16)((u16)(tgt) - (em)->ang[1]);                             \
        if (d <= 0x8000) {                                                \
            if (d < (st)) {                                               \
                (em)->ang[1] += d;                                        \
            } else {                                                      \
                (em)->ang[1] += (st);                                     \
            }                                                             \
        } else if (d > 0x10000 - (st)) {                                  \
            (em)->ang[1] += d;                                            \
        } else {                                                          \
            (em)->ang[1] -= (st);                                         \
        }                                                                 \
    } while (0)

#define UV_RESET(i)       \
    do {                  \
        uv[i][0] = 0.0f;  \
        uv[i][1] = 0.0f;  \
        tm[i] = 0xFFFF;   \
        ty[i] = 0xFF;     \
    } while (0)

void em_fly10_0059D720(EMW *em, EM08W *w) {
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_v0_2;

    switch (em->x05) {
    case 0:
        em->x388 = 4;
        em_char_set(em, 0x67, 0, 0);
        em->x05++;
        em->x07 = 0;
        em->x3F4 = 0;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z <= 50.0f) {
            em->adj_z = 50.0f;
            break;
        }
        break;
    case 1:
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 300.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_9;
            }
        } else {
block_9:
            temp_v0_2 = w->x07 - 1;
            w->x07 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) <= 0) {
                em->x05++;
                em08_to_swim(em);
            } else {
                em->x883 += 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em->work08 = (s32)((1.5f * CalcDistanceXZ(em->pos, em->tgt_pos)) / 50.0f);
                em_act_set(em, 2, 0xA);
            }
        }
        EM08_TURNN(em, Em_Calc_angY(em->pos, em->tgt_pos), 0x200);
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

void em_fly11_0059D960(EMW *em, EM08W *w) {
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_v0_2;

    switch (em->x05) {
    case 0:
        em->x388 = 4;
        em_char_set(em, 0x67, 0, 0);
        em->x05++;
        em->x07 = 0;
        em->x3F4 = 0;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z <= 50.0f) {
            em->adj_z = 50.0f;
            break;
        }
        break;
    case 1:
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 300.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_9;
            }
        } else {
block_9:
            temp_v0_2 = w->x07 - 1;
            w->x07 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) <= 0) {
                em->x05++;
                em08_to_swim(em);
            } else {
                em->x883 -= 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em->work08 = (s32)((1.5f * CalcDistanceXZ(em->pos, em->tgt_pos)) / 50.0f);
                em_act_set(em, 2, 0xB);
            }
        }
        EM08_TURNN(em, Em_Calc_angY(em->pos, em->tgt_pos), 0x200);
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

void em_fly12_0059DBA0(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x72, 0, 0);
        em->work08 = 0x84;
        em_rate_clear(em);
        em->adj_z = 40.0f;
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_to_swim(em);
        }
        em->ang[1] += 0x1F0;
        em->ang[1] = (s32) (u16) EMF(em, s32, 0xA4);
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

void em_fly13_0059DCA0(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x73, 0, 0);
        em->work08 = 0x84;
        em_rate_clear(em);
        em->adj_z = 40.0f;
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_to_swim(em);
        }
        em->ang[1] -= 0x1F0;
        em->ang[1] = (s32) (u16) EMF(em, s32, 0xA4);
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

void em_fly14_0059DDA0(EMW *em, EM08W *w) {
    u8 temp_a3;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_rate_clear(em);
        em_char_set(em, 0x77, 0, 0);
        em->adj_y = 132.0f;
        em->adj_z = 60.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] < (em->x7E4 - em->x7E0))) {
            temp_a3 = em->x05;
            em->x05 = temp_a3 + 1;
            em->x388 = 2;
            em->adj_y = 50.0f;
            em->x3C0[1] = -5.0f;
            swim_eff_set2_005A7120(8.0f, em);
            em08_vib_set(em);
            Em_set_quake_sub(em, 3);
            break;
        }
        break;
    case 2:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if ((em->adj_y < 0.0f) && (em->pos[1] < em->x5AC)) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
        }
        break;
    }
}

void em_fly15_0059DF70(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x79, 0, 0);
        em_rate_clear(em);
        em->pos[1] = em->x7E4 - em->x7E0;
        break;
    case 1:
        if (em_frame_check(em, 10.0f, 0) != 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

void em_fly16_0059E020(EMW *em, EM08W *w) {
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_2;

    switch (em->x05) {
    case 0x0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x78, 0, 0);
        em_rate_clear(em);
        break;
    case 0x1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 2;
            em_char_set(em, 0x77, 0, 0);
            em->adj_y = 50.0f;
            em->adj_z = 80.0f;
            em->x3C0[1] = -4.0f;
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 0x2:
        w->vel[0] = 0;
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add_g(em, w->vel);
        if (em->pos[1] <= (em->x7E4 - em->x7E0)) {
            em->x388 = 4;
        }
        if (em->adj_y < 0.0f) {
            if (*(u8 *)0x3F3404 == em->stg) {
                if (em->x7E9 != 0) {
                    if (em->pos[1] < (em->x7E4 - em->x7E0)) {
                        em->x05++;
                        em->x388 = 4;
                        em->pos[1] = em->x7E4 - em->x7E0;
                        em->adj_y = 0.0f;
                        em->x3C0[1] = 0.0f;
                        em_char_set(em, 0x67, 0xA, 0);
                        w->dist = 1000.0f;
                        em->work08 = 0x3C;
                        em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
                        swim_eff_set2_005A7120(8.0f, em);
                        return;
                    }
                } else if (em->pos[1] < em->x5AC) {
                    em->x05 = 0x63;
                    em->pos[1] = em->x5AC;
                    em08_act_set(em, 0, 4, 4);
                    swim_eff_set2_005A7120(8.0f, em);
                    return;
                }
            } else if (em->pos[1] < (em->x7E4 - em->x7E0)) {
                em->x05++;
                em->x388 = 4;
                em->pos[1] = em->x7E4 - em->x7E0;
                em->adj_y = 0.0f;
                em->x3C0[1] = 0.0f;
                em_char_set(em, 0x67, 0xA, 0);
                w->dist = 1000.0f;
                em->work08 = 0x3C;
                em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
                swim_eff_set2_005A7120(8.0f, em);
                return;
            }
        }
        break;
    case 0x3:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        w->dist -= em->adj_z;
        if (w->dist <= 0.0f || em->adj_z <= 0.0f) {
            em->x05++;
            em08_to_swim(em);
            break;
        }
        break;
    case 0x63:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
        }
        break;
    }
}

void em_fly17_0059E400(EMW *em, EM08W *w) {
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_2;

    switch (em->x05) {
    case 0x0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_rate_clear(em);
        em_char_set(em, 0x77, 0, 0);
        em->adj_y = 100.0f;
        em->adj_z = 60.0f;
        em->x3C0[1] = -8.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 0x1:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] <= (em->x7E4 - em->x7E0))) {
            em->x388 = 2;
            swim_eff_set2_005A7120(8.0f, em);
            em->x05++;
            em08_vib_set(em);
            Em_set_quake_sub(em, 3);
            break;
        }
        break;
    case 0x2:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (em->pos[1] < (em->x7E4 - em->x7E0)) {
            em->x388 = 4;
        }
        if (em->adj_y < 0.0f) {
            if (*(u8 *)0x3F3404 == em->stg) {
                if (em->x7E9 != 0) {
                    if (em->pos[1] < (em->x7E4 - em->x7E0)) {
                        em->x05++;
                        em->x388 = 4;
                        em->pos[1] = em->x7E4 - em->x7E0;
                        em->adj_y = 0.0f;
                        em->x3C0[1] = 0.0f;
                        em_char_set(em, 0x67, 0xA, 0);
                        w->dist = 1000.0f;
                        em->work08 = 0x3C;
                        em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
                        swim_eff_set2_005A7120(8.0f, em);
                        return;
                    }
                } else if (em->pos[1] < em->x5AC) {
                    em->x05 = 0x63;
                    em->pos[1] = em->x5AC;
                    em08_act_set(em, 0, 4, 4);
                    swim_eff_set2_005A7120(8.0f, em);
                    return;
                }
            } else if (em->pos[1] < (em->x7E4 - em->x7E0)) {
                em->x05++;
                em->pos[1] = em->x7E4 - em->x7E0;
                em->x388 = 4;
                em->adj_y = 0.0f;
                em->x3C0[1] = 0.0f;
                em_char_set(em, 0x67, 0xA, 0);
                w->dist = 1000.0f;
                em->work08 = 0x3C;
                em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
                swim_eff_set2_005A7120(8.0f, em);
                return;
            }
        }
        break;
    case 0x3:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        w->dist -= em->adj_z;
        if (w->dist <= 0.0f || em->adj_z <= 0.0f) {
            em->x05++;
            em08_to_swim(em);
            break;
        }
        break;
    case 0x63:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
        }
        break;
    }
}

void em_fly18_0059E820(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x78, 0, 0);
        em_rate_clear(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 2;
            em_char_set(em, 0x77, 0, 0);
            em->adj_y = 50.0f;
            em->adj_z = 100.0f;
            em->x3C0[1] = -4.0f;
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 2:
        w->vel[0] = 0;
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add_g(em, w->vel);
        if ((em->adj_y < 0.0f) && (em->pos[1] < em->x5AC)) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
        }
        break;
    }
}

void em_fly19_0059E9C0(EMW *em, EM08W *w) {
    s32 temp_v1_2;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x82, 0, 0);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05++;
            em->x88B = 1;
            em08_act_set(em, 2, 0x18, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 2, 0x18, 4);
        }
        break;
    }
}

void em_fly20_0059EA90(EMW *em, EM08W *w) {
    s32 temp_v1_2;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x82, 0, 0);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05++;
            em->x88B = 1;
            em08_act_set(em, 2, 0x19, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em->x88B = 1;
            em08_act_set(em, 2, 0x19, 4);
        }
        break;
    }
}

void em_fly21_0059EB70(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x83, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

void em_fly22_0059EC00(EMW *em, EM08W *w) {
    f32 temp_f3;
    s32 temp_v1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em_rate_clear(em);
        em_char_set(em, 0x77, 0, 0);
        em->work08 = 0x96;
        em->tgt_pos[0] = 6000.0f;
        em->tgt_pos[1] = -1000.0f;
        em->tgt_pos[2] = 10000.0f;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->x3C0[1] = -2.0f;
        em->adj_y = ((em->tgt_pos[1] - em->pos[1]) / (f32)em->work08) - ((em->x3C0[1] * (f32)em->work08) / 2.0f);
        em->adj_z = CalcDistanceXZ(em->pos, em->tgt_pos) / (f32) em->work08;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05++;
            em->pos[1] = -1000.0f;
            em08_act_set(em, 2, 0, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em->pos[1] = -1000.0f;
            em08_act_set(em, 2, 0, 4);
        }
        break;
    }
}

void em_fly23_0059EDA0(EMW *em, EM08W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    u8 temp_a2;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x762 = 0;
        em_char_set(em, 0x12, 0xA, 0x1E);
        em08_fly_adjy2_init(em, 3);
        em->x388 = 2;
        swim_eff_set2_005A7120(8.0f, em);
        break;
    case 1:
        em08_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05++;
        }
        break;
    case 2:
        if ((em08_fly_adjy2(em) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
            em->x05++;
            em_char_set(em, 0xB, 0, 0);
            em_rate_clear(em);
            temp_f1 = em->x5AC;
            if (em->pos[1] < temp_f1) {
                em->pos[1] = temp_f1;
            }
            temp_f1_2 = (em->x5AC - em->pos[1]) / 30.0f;
            em->adj_y = temp_f1_2;
            if (!(temp_f1_2 < 0.0f)) {
                em->adj_y = -10.0f;
            }
        }
        break;
    case 3:
        w->vel[0] = 0;
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em_char_set(em, 0x13, 0, 0);
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            w->x10 = 0x96;
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
    temp_f1_3 = em->x5AC;
    if (em->pos[1] < temp_f1_3) {
        em->pos[1] = temp_f1_3;
    }
}

void em_fly24_0059EFD0(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x83, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

void em_fly25_0059F070(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x83, 0, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

void em_atk00_0059F110(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x24, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

void em_atk01_0059F190(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x2F, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 78.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0x71C);
            if (em->kind == 8) {
                Shell08_set_ang(em, 0x22, 0x7, 0, 0x71C, 0);
            } else {
                Shell08_set_ang(em, 0x22, 0x7, 0x1, 0x71C, 0);
            }
        }
        if (em_frame_check(em, 80.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0x71C);
        }
        if (em_frame_check(em, 82.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0x71C);
        }
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

void em_atk02_0059F2F0(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x2B, 0, 0);
        em_action_timer_calc(em, 0);
        break;
    case 1:
        em->ang[1] -= 0x200;
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em_char_set(em, 0x2B, 0, 0);
            break;
        }
        break;
    case 2:
        em->ang[1] -= 0x200;
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

void em_atk03_0059F3E0(EMW *em, EM08W *w) {
    em08_to_swim(em);
}

void em_atk04_0059F3F0(EMW *em, EM08W *w) {
    em08_to_swim(em);
}

void em_atk05_0059F400(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0x70, 0, 0);
        em->x388 = 4;
        break;
    case 1:
        if (em_frame_check(em, 58.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0);
            if (em->kind == 8) {
                Shell08_set_ang(em, 0x22, 0x7, 0, 0xE39, 0);
            } else {
                Shell08_set_ang(em, 0x22, 0x7, 0x1, 0xE39, 0);
            }
        }
        if (em_frame_check(em, 26.0f, 0) != 0) {
            swim_eff_set2_005A7120(8.0f, em);
        }
        if (em_frame_check(em, 98.0f, 0) != 0) {
            swim_eff_set2_005A7120(8.0f, em);
        }
        if (em_frame_check(em, 60.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0);
        }
        if (em_frame_check(em, 62.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

void em_atk06_0059F5C0(EMW *em, EM08W *w) {
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    u8 temp_a1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0x6C, 0, 0);
        em->x388 = 4;
        em_rate_clear(em);
        em->adj_z = 100.0f;
        em->adj_y = 80.0f;
        em->x3C0[1] = -4.5f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] <= (em->x7E4 - em->x7E0))) {
            temp_a1 = em->x05;
            em->x05 = temp_a1 + 1;
            em->x388 = 2;
            swim_eff_set2_005A7120(8.0f, em);
            em08_vib_set(em);
            Em_set_quake_sub(em, 3);
            break;
        }
    default:
        break;
    case 2:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] <= em->x7E4)) {
            em->x05++;
            em->adj_y = 0.0f;
            em->work08 = 0x1E;
            break;
        }
        break;
    case 3:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05++;
            break;
        }
        break;
    case 4:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if ((em->pos[1] <= (em->x7E4 - em->x7E0)) && (em->adj_y < 0.0f)) {
            em->x05++;
            em->x388 = 4;
            em->adj_y = 0.0f;
            em->x3C0[1] = 0.0f;
            em_char_set(em, 0x67, 0xA, 0);
            w->dist = 1000.0f;
            em->work08 = 0x3C;
            em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 5:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        w->dist -= em->adj_z;
        if (w->dist <= 0.0f || em->adj_z <= 0.0f) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

void em_atk07_0059F890(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x88, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

void em_atk08_0059F910(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0x71, 0, 0);
        em_rate_clear(em);
        em->adj_y = 16.0f;
        em->adj_z = 100.0f;
        em->x3C0[1] = -0.8f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        em08_vib_set(em);
        Em_set_quake_sub(em, 3);
        break;
    case 1:
        if (em_frame_check(em, 54.0f, 0) != 0) {
            em->work08 = 0x12;
            em->x3C0[2] = -em->adj_z / (f32) em->work08;
        }
        if ((em_frame_check2(em, 0, 84.0f) != 0) && (em->pos[1] <= (em->x7E4 - em->x7E0))) {
            em->adj_y = 0.0f;
            em->x3C0[1] = 0.0f;
        }
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (em->adj_z <= 0.0f) {
            em->adj_z = 0.0f;
            em->x3C0[2] = 0.0f;
        }
        if (em->x194 == 0) {
            em->x388 = 4;
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}
