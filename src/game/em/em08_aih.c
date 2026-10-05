/* em08_ai, run 8: em_demo01_005A1700 .. em_move06_005A2D90 (game.bin 0x005A1700-0x005A2DDC). Matching functions of em08_ai_nm.c (that file holds the
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

void em_demo01_005A1700(EMW *em, EM08W *w) {

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 1, 0, 0);
        em->x01 = 0;
        em->x839 = 0;
        break;
    case 1:
        if (Event_flag_ck(0x11) == 1) {
            em->x05++;
            em->x01 = 1;
            em08_to_normal(em);
        }
        break;
    }
}

void em_die00_005A17A0(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;

    if (em->kind == 0x22) {
        em->x40C = 0xA;
    }
    em->x40E = 0xA;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em_char_set(em, 0x44, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
        if (em_frame_check(em, 212.0f, 0) != 0) {
            Em_set_quake_sub(em, 2);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x2:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 4;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x3:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x4:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_die01_005A19D0(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;

    if (em->kind == 0x22) {
        em->x40C = 0xA;
    }
    em->x40E = 0xA;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em_char_set(em, 0x61, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x2:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 4;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x3:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x4:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_die02_005A1BD0(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;

    if (em->kind == 0x22) {
        em->x40C = 0xA;
    }
    em->x40E = 0xA;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em_rate_clear(em);
        em->adj_y = 100.0f;
        em->x3C0[1] = -2.75f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 0x1:
        w->vel[1] = (s32) em->ang[1];
        if (!(em->pos[1] < (em->x7E4 - em->x7E0))) {
            em->x388 = 2;
            em->adj_y = 80.0f;
            speed_add_g(em, w->vel);
            em->x05++;
            break;
        }
        em->x388 = 4;
        speed_add(em, w->vel);
        break;
    case 0x2:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if ((em->pos[1] < em->x7E4) && (em->adj_y < 0.0f)) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x79, 0, 0);
            break;
        }
        break;
    case 0x3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x44, 0, 0);
            em->x3F4 = 0;
            em->x388 = 3;
            Quest_enemy_die(em);
            break;
        }
        break;
    case 0x4:
        if (em_frame_check(em, 212.0f, 0) != 0) {
            Em_set_quake_sub(em, 2);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x5:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 7;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x6:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x7:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_die03_005A1F70(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;

    if (em->kind == 0x22) {
        em->x40C = 0xA;
    }
    em->x40E = 0xA;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em_char_set(em, 0x4B, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x80, 0, 0);
            break;
        }
        break;
    case 0x2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x7F, 0, 0);
        }
    case 0x3:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x4:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 6;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x5:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x6:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_die04_005A21F0(EMW *em, EM08W *w) {
    switch (em->x05) {
    case 0:
        if (Quest_enemy_revival_ck() == 1) {
            Quest_enemy_revival_set(em);
            em_status_init(em);
            em08_init(em);
            em_cmd_reset(em);
            em->x839 = 0;
            em->mode = 5;
            em->x15 = 4;
            em->x388 = 4;
            em_act_set(em, 2, 0);
        } else {
            em->x04 += 1;
        }
        break;
    }
}

void em_die05_005A22A0(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;
    u8 var_v1;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em->x388 = 2;
        em->x3C0[1] = -3.0f;
        em->x3C0[0] = 0.0f;
        em->x3C0[2] = 0.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 0x1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (!(em->pos[1] < em->x7E4)) {
            em->x388 = 2;
        } else {
            em->x388 = 4;
        }
        if (em->pos[1] < em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x79, 0, 0);
            break;
        }
        break;
    case 0x2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x44, 0, 0);
            em->x3F4 = 0;
            em->x388 = 3;
            Quest_enemy_die(em);
            break;
        }
        break;
    case 0x3:
        if (em_frame_check(em, 212.0f, 0) != 0) {
            Em_set_quake_sub(em, 2);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x4:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 6;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x5:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x6:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_move00_005A25C0(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_act00_0059A7B0(em, w);
        break;
    case 1:
        em_act01_0059A880(em, w);
        break;
    case 2:
        em_act02_0059A960(em, w);
        break;
    case 3:
        em_act03_0059AA10(em, w);
        break;
    case 4:
        em_act04_0059AD80(em, w);
        break;
    case 5:
        em_act05_0059AE70(em, w);
        break;
    case 6:
        em_act06_0059AF80(em, w);
        break;
    case 7:
        em_act07_0059B050(em, w);
        break;
    case 8:
        em_act08_0059B0D0(em, w);
        break;
    case 9:
        em_act09_0059B210(em, w);
        break;
    case 10:
        em_act10_0059B300(em, w);
        break;
    case 11:
        em_act11_0059B440(em, w);
        break;
    case 12:
        em_act12_0059B530(em, w);
        break;
    case 13:
        em_act13_0059B640(em, w);
        break;
    case 14:
        em_act14_0059B710(em, w);
        break;
    case 15:
        em_act15_0059B920(em, w);
        break;
    case 16:
        em_act16_0059B9F0(em, w);
        break;
    case 17:
        em_act17_0059BC00(em, w);
        break;
    case 18:
        em_act18_0059BCD0(em, w);
        break;
    case 19:
        em_act19_0059BE30(em, w);
        break;
    case 20:
        em_act20_0059BF00(em, w);
        break;
    case 21:
        em_act21_0059C140(em, w);
        break;
    case 40:
        em_act40_0059C080(em, w);
        break;
    }
}

void em_move01_005A2830(EMW *em, EM08W *w) {
    u8 temp_a2;

    temp_a2 = EMF(em, u8, 0x15);
    switch (temp_a2) {
    case 0:
        em_mv00_0059C1D0(em, w);
        break;
    case 1:
        em_mv01_0059C310(em, w);
        break;
    case 2:
        em_mv02_0059C480(em, w);
        break;
    case 3:
        em_mv03_0059C6D0(em, w);
        break;
    }
}

void em_move02_005A28C0(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_fly00_0059C810(em, w);
        break;
    case 1:
        em_fly01_0059C8A0(em, w);
        break;
    case 2:
        em_fly02_0059C930(em, w);
        break;
    case 3:
        em_fly03_0059CA30(em, w);
        break;
    case 4:
        em_fly04_0059CCE0(em, w);
        break;
    case 5:
        em_fly05_0059CF30(em, w);
        break;
    case 6:
        em_fly06_0059D0F0(em, w);
        break;
    case 7:
        em_fly07_0059D190(em, w);
        break;
    case 8:
        em_fly08_0059D2E0(em, w);
        break;
    case 9:
        em_fly09_0059D420(em, w);
        break;
    case 10:
        em_fly10_0059D720(em, w);
        break;
    case 11:
        em_fly11_0059D960(em, w);
        break;
    case 12:
        em_fly12_0059DBA0(em, w);
        break;
    case 13:
        em_fly13_0059DCA0(em, w);
        break;
    case 14:
        em_fly14_0059DDA0(em, w);
        break;
    case 15:
        em_fly15_0059DF70(em, w);
        break;
    case 16:
        em_fly16_0059E020(em, w);
        break;
    case 17:
        em_fly17_0059E400(em, w);
        break;
    case 18:
        em_fly18_0059E820(em, w);
        break;
    case 19:
        em_fly19_0059E9C0(em, w);
        break;
    case 20:
        em_fly20_0059EA90(em, w);
        break;
    case 21:
        em_fly21_0059EB70(em, w);
        break;
    case 22:
        em_fly22_0059EC00(em, w);
        break;
    case 23:
        em_fly23_0059EDA0(em, w);
        break;
    case 24:
        em_fly24_0059EFD0(em, w);
        break;
    case 25:
        em_fly25_0059F070(em, w);
    default:
        break;
    }
}

void em_move03_005A2AA0(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_atk00_0059F110(em, w);
        break;
    case 1:
        em_atk01_0059F190(em, w);
        break;
    case 2:
        em_atk02_0059F2F0(em, w);
        break;
    case 3:
        em_atk03_0059F3E0(em, w);
        break;
    case 4:
        em_atk04_0059F3F0(em, w);
        break;
    case 5:
        em_atk05_0059F400(em, w);
        break;
    case 6:
        em_atk06_0059F5C0(em, w);
        break;
    case 7:
        em_atk07_0059F890(em, w);
        break;
    case 8:
        em_atk08_0059F910(em, w);
    default:
        break;
    }
}

void em_move04_005A2B70(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_dmg00_0059FAB0(em, w);
        break;
    case 1:
        em_dmg01_0059FB90(em, w);
        break;
    case 2:
        em_dmg02_0059FC20(em, w);
        break;
    case 3:
        em_dmg03_0059FCB0(em, w);
        break;
    case 4:
        em_dmg04_0059FD40(em, w);
        break;
    case 5:
        em_dmg05_0059FE40(em, w);
        break;
    case 6:
        em_dmg06_0059FFE0(em, w);
        break;
    case 7:
        em_dmg07_005A00F0(em, w);
        break;
    case 8:
        em_dmg08_005A0250(em, w);
        break;
    case 9:
        em_dmg09_005A0350(em, w);
        break;
    case 10:
        em_dmg10_005A0470(em, w);
        break;
    case 11:
        em_dmg11_005A0500(em, w);
        break;
    case 12:
        em_dmg12_005A05E0(em, w);
        break;
    case 13:
        em_dmg13_005A06C0(em, w);
        break;
    case 14:
        em_dmg14_005A07B0(em, w);
        break;
    case 15:
        em_dmg15_005A08C0(em, w);
        break;
    case 16:
        em_dmg16_005A0B60(em, w);
        break;
    case 17:
        em_dmg17_005A0C40(em, w);
        break;
    case 18:
        em_dmg18_005A0E60(em, w);
        break;
    case 19:
        em_dmg19_005A0F50(em, w);
    default:
        break;
    }
}

void em_move05_005A2CF0(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_die00_005A17A0(em, w);
        break;
    case 1:
        em_die01_005A19D0(em, w);
        break;
    case 2:
        em_die02_005A1BD0(em, w);
        break;
    case 3:
        em_die03_005A1F70(em, w);
        break;
    case 4:
        em_die04_005A21F0(em, w);
        break;
    case 5:
        em_die05_005A22A0(em, w);
    default:
        break;
    }
}

void em_move06_005A2D90(EMW *em, EM08W *w) {
    u8 temp_a2;

    temp_a2 = EMF(em, u8, 0x15);
    switch (temp_a2) {
    case 0:
        em_demo00_005A0FF0(em, w);
        break;
    case 1:
        em_demo01_005A1700(em, w);
        break;
    }
}
