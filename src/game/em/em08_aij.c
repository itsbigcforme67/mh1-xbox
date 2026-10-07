/* em08 run: em08_uvmove (static), sound/effect script, hire_move group and em08_effect_move as one translation unit (0x005A3AA0-0x005A6F8C) */
/* em08_ai, run 10: sound_call_sub_005A3CA0 .. ef_move_sub_005A3E50 (game.bin 0x005A3CA0-0x005A699C). Matching functions of em08_ai_nm.c (that file holds the
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
#define UVR(i) \
    do { \
        em->uv[i][0] = 0.0f; \
        em->uv[i][1] = 0.0f; \
        em->uvtm[i] = 0xFFFF; \
        em->uvty[i] = 0xFF; \
    } while (0)

static void em08_uvmove(EMW *em) {
    int i;

    for (i = 0; i < 4; i++) {
        if (em->uvtm[i] != 0xFFFF) {
            em->uvtm[i] += 1;
        }
        switch (em->uvty[i]) {
        case 0:
            UVR(i);
            break;
        case 1:
            if (em->uvtm[i] >= 0x3E) {
                UVR(i);
            } else {
                int k = ((u32)em->uvtm[i] >> 1) + 1;

                em->uv[i][0] = 0.125f * (f32)(k % 8);
                em->uv[i][1] = 0.25f * (f32)(k / 8 % 4);
            }
            break;
        case 2:
            em->uv[i][0] = 0.125f;
            em->uv[i][1] = 0.0f;
            em->uvtm[i] = 0xFFFF;
            em->uvty[i] = 0xFF;
            break;
        case 3:
            if (em->uvtm[i] >= 0xC) {
                UVR(i);
            } else {
                int k = ((u32)em->uvtm[i] >> 1) + 2;

                em->uv[i][0] = 0.125f * (f32)(k % 4);
                em->uv[i][1] = 0.25f * (f32)(k / 4 % 4);
            }
            break;
        case 0xFF:
            break;
        }
    }
}

void sound_call_sub_005A3CA0(EMW *em, int se, int joint);
static void sound_call_005A3D10(EMW *em, int frame, int se, int joint);
static void quake_call_005A3DB0(EMW *em, int frame, int v);
static void move_default_005A3E00(EMW *em);
static void ef_move_sub_005A3E50(EMW *em, EM08W *w);

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

void sound_call_sub_005A3CA0(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
    Em_se_req2(em, se, 0, pos, 7, 0);
}

static void sound_call_005A3D10(EMW *em, int frame, int se, int joint) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, 0)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, 7, 0);
    }
}

static void quake_call_005A3DB0(EMW *em, int frame, int v) {
    if (em_frame_check(em, (f32)frame, 0)) {
        Em_set_quake_sub(em, v);
    }
}

static void move_default_005A3E00(EMW *em) {
    f32 (*uv)[3] = (f32 (*)[3])((u8 *)em + 0x5C0);
    u16 *tm = (u16 *)((u8 *)em + 0x5F0);
    u8 *ty = (u8 *)em + 0x5F8;

    uv[0][0] = 0.0f;
    uv[0][1] = 0.0f;
    tm[0] = 0xFFFF;
    ty[0] = 0xFF;
    uv[1][0] = 0.0f;
    uv[1][1] = 0.0f;
    tm[1] = 0xFFFF;
    ty[1] = 0xFF;
    uv[2][0] = 0.0f;
    uv[2][1] = 0.0f;
    tm[2] = 0xFFFF;
    ty[2] = 0xFF;
    uv[3][0] = 0.0f;
    uv[3][1] = 0.0f;
    tm[3] = 0xFFFF;
    ty[3] = 0xFF;
}

static void ef_move_sub_005A3E50(EMW *em, EM08W *w) {
    f32 va[4];
    f32 vb[4];
    f32 vc[4];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_005A3D10(em, 0x40, 0x20, 0x23);
        sound_call_005A3D10(em, 0xc8, Code_Make(0x2d, 2, 0x2d, 2), 0x23);
        sound_call_005A3D10(em, 0x19a, Code_Make(0x21, 4, 0x21, 4), 0x23);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x3EB:
        sound_call_005A3D10(em, 0x34, 1, 0x14);
        sound_call_005A3D10(em, 0x74, 1, 0x1a);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 2);
        }
        if (em_frame_check(em, 50.0f, 0)) {
            atk_shell_set(em, 3);
        }
        if (em_frame_check(em, 120.0f, 0)) {
            atk_shell_set(em, 4);
        }
        break;
    case 0x3EC:
        sound_call_005A3D10(em, 0xa, 0x2e, 0x23);
        sound_call_005A3D10(em, 0x1c, 6, 0x14);
        sound_call_005A3D10(em, 0x34, 6, 0x1a);
        sound_call_005A3D10(em, 0x4c, 6, 0x14);
        sound_call_005A3D10(em, 0x64, 6, 0x1a);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 22.0f, 0)) {
            atk_shell_set(em, 0x13);
        }
        if (em_frame_check(em, 16.0f, 0) || em_frame_check(em, 56.0f, 0) || em_frame_check(em, 102.0f, 0)) {
            atk_shell_set(em, 0x14);
        }
        if (em_frame_check(em, 30.0f, 0) || em_frame_check(em, 78.0f, 0)) {
            atk_shell_set(em, 0x15);
        }
        if (em_frame_check(em, 18.0f, 0) || em_frame_check(em, 66.0f, 0)) {
            Eft13_set_em_scl(em, 0x1a, 4.0f, 3);
        }
        if (em_frame_check(em, 42.0f, 0) || em_frame_check(em, 90.0f, 0)) {
            Eft13_set_em_scl(em, 0x14, 4.0f, 3);
        }
        break;
    case 0x3ED:
        sound_call_005A3D10(em, 4, 0x21, 0x23);
        sound_call_005A3D10(em, 0x1a, 1, 0x14);
        sound_call_005A3D10(em, 0x38, 1, 0x1a);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 5);
        }
        if (em_frame_check(em, 30.0f, 0)) {
            atk_shell_set(em, 6);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 2.0f, 78.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, 1);
                }
            }
        }
        break;
    case 0x3EE:
        sound_call_005A3D10(em, 4, 0x21, 0x23);
        sound_call_005A3D10(em, 0x1a, 1, 0x14);
        sound_call_005A3D10(em, 0x38, 1, 0x1a);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 30.0f, 0)) {
            atk_shell_set(em, 7);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 8);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 2.0f, 78.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, 0);
                }
            }
        }
        break;
    case 0x3F2:
        sound_call_005A3D10(em, 6, 0x31, 0x23);
        sound_call_005A3D10(em, 6, 4, 0x1a);
        sound_call_005A3D10(em, 0x4c, 0, 0x14);
        sound_call_005A3D10(em, 0x1c, 0x12, 0x14);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 42.0f, 0)) {
            atk_shell_set(em, 0x11);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0x12);
        }
        if (em_frame_check(em, 74.0f, 0)) {
            atk_shell_set(em, 0x12);
        }
        break;
    case 0x3F3:
        sound_call_005A3D10(em, 4, 0x20, 0x23);
        sound_call_005A3D10(em, 0xe, 0xb, 6);
        sound_call_005A3D10(em, 0x12, 0xb, 0xc);
        sound_call_005A3D10(em, 0x46, 0xb, 6);
        sound_call_005A3D10(em, 0x42, 0xb, 0xc);
        sound_call_005A3D10(em, 0x7a, 0xb, 6);
        sound_call_005A3D10(em, 0x7e, 0xb, 0xc);
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x3F9:
        sound_call_005A3D10(em, 6, 0x27, 0x23);
        sound_call_005A3D10(em, 0x1c, 6, 0x14);
        sound_call_005A3D10(em, 0x38, 6, 0x1a);
        break;
    case 0x3FA:
        sound_call_005A3D10(em, 0x38, 7, 0x1a);
        sound_call_005A3D10(em, 0x1a, 0xd, 6);
        sound_call_005A3D10(em, 0x1e, 0xd, 0xc);
        sound_call_005A3D10(em, 0x52, 0xb, 6);
        sound_call_005A3D10(em, 0x56, 0xb, 0xc);
        sound_call_005A3D10(em, 0x88, 0xb, 6);
        sound_call_005A3D10(em, 0x8c, 0xb, 0xc);
        sound_call_005A3D10(em, 0xb6, 0xb, 6);
        sound_call_005A3D10(em, 0xba, 0xb, 0xc);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 42.0f, 0) || em_frame_check(em, 102.0f, 0) || em_frame_check(em, 156.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        break;
    case 0x3FB:
        sound_call_005A3D10(em, 2, 0xb, 6);
        sound_call_005A3D10(em, 6, 0xb, 0xc);
        sound_call_005A3D10(em, 0xc, 4, 0x1a);
        sound_call_005A3D10(em, 4, 1, 0x14);
        sound_call_005A3D10(em, 0xe, 9, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0xd);
        }
        if (em_frame_check(em, 16.0f, 0)) {
            ground_land_eff_set_005A7050(em);
        }
        break;
    case 0x401:
        sound_call_005A3D10(em, 4, 0x1f, 0x23);
        sound_call_005A3D10(em, 0x6c, 0x22, 0x23);
        sound_call_005A3D10(em, 0xaa, 0x20, 0x23);
        sound_call_005A3D10(em, 0xd6, 0x22, 0x23);
        sound_call_005A3D10(em, 0x114, 0x27, 0x23);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x407:
        sound_call_005A3D10(em, 2, 0x2d, 0x23);
        sound_call_005A3D10(em, 0xb6, 0x2d, 0x23);
        sound_call_005A3D10(em, 0x16c, 0x2d, 0x23);
        va[1] = 10.0f;
        va[2] = 140.0f;
        va[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, va, 1.6f);
        break;
    case 0x408:
        sound_call_005A3D10(em, 4, 0x2f, 0x23);
        sound_call_005A3D10(em, 0x46, 0, 0x1a);
        sound_call_005A3D10(em, 0x8e, 3, 0x14);
        sound_call_005A3D10(em, 4, 0x1e, 0);
        sound_call_005A3D10(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_005A3D10(em, 4, 0x2f, 0x23);
        sound_call_005A3D10(em, 0x46, 0x2d, 0x23);
        sound_call_005A3D10(em, 0x7e, 3, 0x1a);
        sound_call_005A3D10(em, 0xb6, 3, 0x14);
        sound_call_005A3D10(em, 0x38, 0x1d, 0);
        break;
    case 0x40A:
        sound_call_005A3D10(em, 0x24, 0x2f, 0x23);
        hire_req_set_005A6F90(em, w, 3);
        vb[1] = 10.0f;
        vb[2] = 140.0f;
        vb[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, vb, 1.6f);
        break;
    case 0x40C:
        sound_call_005A3D10(em, 0x24, 0x24, 0x23);
        sound_call_005A3D10(em, 0x14, 0, 0x1a);
        sound_call_005A3D10(em, 0x28, 0, 0x14);
        sound_call_005A3D10(em, 0x3c, 0x15, 0x22);
        sound_call_005A3D10(em, 0x3c, 0xc, 0xc);
        sound_call_005A3D10(em, 0xc, 0xc, 6);
        sound_call_005A3D10(em, 0xa6, 0, 0x1a);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 50.0f, 0)) {
            atk_shell_set(em, 1);
        }
        break;
    case 0x40D:
        sound_call_005A3D10(em, 0x1e, 0x20, 0x23);
        sound_call_005A3D10(em, 0xc, 0xc, 0x1a);
        sound_call_005A3D10(em, 0x1c, 2, 0x1a);
        sound_call_005A3D10(em, 0x2e, 3, 0x14);
        sound_call_005A3D10(em, 0x48, 1, 0x1a);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x40E:
        sound_call_005A3D10(em, 4, 0x2f, 0x23);
        sound_call_005A3D10(em, 0x48, 0x16, 0);
        sound_call_005A3D10(em, 0xa4, 9, 0);
        sound_call_005A3D10(em, 0x90, 3, 0xc);
        sound_call_005A3D10(em, 0x88, 0xe, 6);
        sound_call_005A3D10(em, 0xb2, 4, 0x22);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 148.0f, 0)) {
            atk_shell_set(em, 0xf);
        }
        vc[1] = 10.0f;
        vc[2] = 140.0f;
        vc[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, vc, 1.6f);
        if (em_frame_check(em, 156.0f, 0)) {
            Eft20_set(1.0f, em, 0, 0);
        }
        break;
    case 0x413:
        sound_call_005A3D10(em, 8, 0x23, 0x23);
        sound_call_005A3D10(em, 0xe, 0xf, 6);
        sound_call_005A3D10(em, 0x1e, 0x14, 0x2a);
        sound_call_005A3D10(em, 0x2a, 0, 0x1a);
        sound_call_005A3D10(em, 0x48, 1, 0x14);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 30.0f, 0)) {
            atk_shell_set(em, 0x10);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 6.0f, 38.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, 1);
                }
            }
        }
        break;
    case 0x417:
        sound_call_005A3D10(em, 2, 0x12, 0);
        sound_call_005A3D10(em, 0x50, 0x18, 0x23);
        sound_call_005A3D10(em, 0x14, 0x28, 0x23);
        sound_call_005A3D10(em, 0x4a, 0x29, 0x23);
        sound_call_005A3D10(em, 0x4c, 0xd, 6);
        sound_call_005A3D10(em, 0x48, 0xd, 0xc);
        sound_call_005A3D10(em, 0xb4, 0x13, 6);
        sound_call_005A3D10(em, 0xbb, 0x13, 0xc);
        sound_call_005A3D10(em, 0xf0, 0x12, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 66.0f, 0)) {
            atk_shell_set(em, 0x19);
        }
        break;
    case 0x418:
        sound_call_005A3D10(em, 2, 0x22, 0x23);
        sound_call_005A3D10(em, 0x22, 0x22, 0x23);
        sound_call_005A3D10(em, 0x40, 0x22, 0x23);
        sound_call_005A3D10(em, 0x72, 0x20, 0x23);
        sound_call_005A3D10(em, 0x10, 1, 0x14);
        break;
    case 0x423:
        sound_call_005A3D10(em, 0xc, 0x1d, 0);
        sound_call_005A3D10(em, 6, 0x22, 0x23);
        sound_call_005A3D10(em, 0x68, 3, 0x1a);
        break;
    case 0x424:
        sound_call_005A3D10(em, 6, 0x2b, 0x23);
        sound_call_005A3D10(em, 0x94, 0x22, 0x23);
        sound_call_005A3D10(em, 0x48, 0x13, 0x2a);
        sound_call_005A3D10(em, 0x30, 6, 0x1a);
        sound_call_005A3D10(em, 0x30, 0x11, 0x14);
        sound_call_005A3D10(em, 0x30, 3, 0x14);
        sound_call_005A3D10(em, 0x74, 3, 0x14);
        sound_call_005A3D10(em, 0x94, 3, 0x1a);
        sound_call_005A3D10(em, 0xc0, 3, 0x1a);
        sound_call_005A3D10(em, 0x34, 0xc, 6);
        sound_call_005A3D10(em, 0x38, 0xd, 0xc);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 44.0f, 0)) {
            Eft13_set_em_scl(em, 0x1b, 1.0f, 7);
        }
        if (em_frame_check(em, 62.0f, 0)) {
            Eft13_set_em_scl(em, 0x16, 1.0f, 7);
        }
        if (em_frame_check(em, 80.0f, 0)) {
            Eft20_set(0.5f, em, 1, 0x80);
        }
        break;
    case 0x425:
        sound_call_005A3D10(em, 6, 0x2b, 0x23);
        sound_call_005A3D10(em, 0xa, 0x12, 0x1a);
        sound_call_005A3D10(em, 0x26, 6, 0x1a);
        sound_call_005A3D10(em, 0x4e, 4, 0x14);
        sound_call_005A3D10(em, 0x6e, 3, 0x14);
        sound_call_005A3D10(em, 0x5a, 0x15, 0);
        sound_call_005A3D10(em, 0xac, 3, 0x1a);
        if (em_frame_check(em, 24.0f, 0)) {
            Eft13_set_em_scl(em, 0x1a, 1.0f, 7);
        }
        break;
    case 0x426:
        sound_call_005A3D10(em, 4, 0x27, 0x23);
        sound_call_005A3D10(em, 0x56, 0x1f, 0x23);
        sound_call_005A3D10(em, 0x10, 3, 0x14);
        sound_call_005A3D10(em, 4, 0xd, 6);
        sound_call_005A3D10(em, 8, 0xd, 0xc);
        sound_call_005A3D10(em, 0x62, 0x13, 0x2a);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x427:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x13, 0);
        sound_call_005A3D10(em, 0xe, 1, 0x1a);
        sound_call_005A3D10(em, 0x4c, 0x1f, 0x23);
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x428:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x13, 0);
        sound_call_005A3D10(em, 0x44, 0x20, 0x23);
        sound_call_005A3D10(em, 0x7a, 3, 0x1a);
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x42A:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x13, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 268.0f, 0)) {
            Eft20_set(0.9f, em, 0, 0);
        }
        break;
    case 0x42B:
        sound_call_005A3D10(em, 0x46, 0x2d, 0x23);
        sound_call_005A3D10(em, 0x46, 0x1e, 0);
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x42C:
        sound_call_005A3D10(em, 0x1a, 0x2c, 0x23);
        sound_call_005A3D10(em, 0x2e, 9, 0);
        sound_call_005A3D10(em, 4, 0x14, 0);
        sound_call_005A3D10(em, 0x1a, 0x2e, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 50.0f, 0)) {
            Eft20_set(0.8f, em, 0, 0);
        }
        break;
    case 0x42D:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x13, 0x2a);
        sound_call_005A3D10(em, 0x18, 9, 0);
        sound_call_005A3D10(em, 0x18, 0x11, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 26.0f, 0)) {
            Eft20_set(1.0f, em, 0x12, 0x80);
        }
        break;
    case 0x42E:
        sound_call_005A3D10(em, 0x14, 8, 0);
        sound_call_005A3D10(em, 0x24, 4, 0x22);
        sound_call_005A3D10(em, 4, 0x2c, 0x23);
        sound_call_005A3D10(em, 0x46, 8, 0);
        sound_call_005A3D10(em, 0x2a, 0x12, 0);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 16.0f, 0) || em_frame_check(em, 66.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x42F:
        sound_call_005A3D10(em, 4, 0x20, 0x23);
        sound_call_005A3D10(em, 0x3e, 1, 0x14);
        sound_call_005A3D10(em, 0x38, 6, 0x1a);
        sound_call_005A3D10(em, 4, 0x1d, 0);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x432:
        sound_call_005A3D10(em, 4, 0x2a, 0);
        sound_call_005A3D10(em, 4, 0x13, 0x2a);
        sound_call_005A3D10(em, 0x18, 9, 0);
        sound_call_005A3D10(em, 0x18, 0x11, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 26.0f, 0)) {
            Eft20_set(1.0f, em, 0x12, 0x81);
        }
        break;
    case 0x433:
        sound_call_005A3D10(em, 0x14, 8, 0);
        sound_call_005A3D10(em, 0x24, 4, 0x22);
        sound_call_005A3D10(em, 4, 0x2c, 0x23);
        sound_call_005A3D10(em, 0x46, 8, 0);
        sound_call_005A3D10(em, 0x2a, 0x12, 0);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 16.0f, 0) || em_frame_check(em, 66.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x434:
        sound_call_005A3D10(em, 0x10, 0x2a, 0x23);
        sound_call_005A3D10(em, 0x66, 0x2b, 0x23);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x437:
        sound_call_005A3D10(em, 0xc, 0x24, 0x23);
        sound_call_005A3D10(em, 0x24, 0x22, 0x23);
        sound_call_005A3D10(em, 0x66, 0x22, 0x23);
        sound_call_005A3D10(em, 0xa4, 0x22, 0x23);
        break;
    case 0x447:
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x448:
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x44C:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x44D:
        sound_call_005A3D10(em, 4, 0x65, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x44E:
        sound_call_005A3D10(em, 4, 0x17, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x44F:
        sound_call_005A3D10(em, 4, 0x24, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0);
        sound_call_005A3D10(em, 0x20, 0x15, 0x14);
        sound_call_005A3D10(em, 0x24, 0x15, 0x1a);
        sound_call_005A3D10(em, 0x46, 0x17, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 10.0f, 0)) {
            atk_shell_set(em, 0x1a);
        }
        break;
    case 0x450:
        sound_call_005A3D10(em, 0x14, 0x2e, 0x23);
        sound_call_005A3D10(em, 4, 0x15, 0x23);
        sound_call_005A3D10(em, 0x22, 0x30, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x451:
        sound_call_005A3D10(em, 4, 0x15, 0x23);
        sound_call_005A3D10(em, 0x1e, 0x30, 0x2a);
        sound_call_005A3D10(em, 0x30, 0x30, 0x2a);
        sound_call_005A3D10(em, 4, 0x1d, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x452:
        sound_call_005A3D10(em, 4, 0x16, 0);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x453:
        sound_call_005A3D10(em, 4, 0x16, 0);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x454:
        sound_call_005A3D10(em, 4, 0x2e, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0);
        sound_call_005A3D10(em, 0x30, 0x30, 0x2a);
        sound_call_005A3D10(em, 4, 0x16, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 10.0f, 0)) {
            atk_shell_set(em, 0xe);
        }
        break;
    case 0x457:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x458:
        sound_call_005A3D10(em, 0x34, 0x29, 0x23);
        sound_call_005A3D10(em, 0x34, 0x18, 0x23);
        sound_call_005A3D10(em, 0x6a, 0x16, 0);
        sound_call_005A3D10(em, 4, 0x16, 0);
        sound_call_005A3D10(em, 4, 0x19, 0);
        hire_req_set_005A6F90(em, w, 0);
        quake_call_005A3DB0(em, 0x1a, 3);
        if (em_frame_check(em, 26.0f, 0)) {
            em08_vib_set(em);
            atk_shell_set(em, 0x1d);
        }
        break;
    case 0x459:
        sound_call_005A3D10(em, 4, 0x33, 0x23);
        sound_call_005A3D10(em, 2, 0x16, 0x2a);
        if (em_frame_check(em, 28.0f, 0)) {
            atk_shell_set(em, 0x1b);
        }
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45A:
        sound_call_005A3D10(em, 4, 0x16, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45B:
        sound_call_005A3D10(em, 4, 0x16, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45C:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45D:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45E:
        sound_call_005A3D10(em, 0x26, 0x24, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0x14);
        break;
    case 0x45F:
        sound_call_005A3D10(em, 4, 0x23, 0x23);
        sound_call_005A3D10(em, 0x42, 0x13, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0x18);
        }
        break;
    case 0x460:
        sound_call_005A3D10(em, 4, 0x16, 0x23);
        sound_call_005A3D10(em, 0x1e, 0x12, 0x14);
        sound_call_005A3D10(em, 0x1e, 0x15, 0);
        sound_call_005A3D10(em, 0x1e, 0x14, 0);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x461:
        sound_call_005A3D10(em, 0xe, 0x10, 0);
        sound_call_005A3D10(em, 0xa, 9, 0);
        sound_call_005A3D10(em, 0x38, 8, 0x23);
        sound_call_005A3D10(em, 0x38, 0x12, 0x14);
        sound_call_005A3D10(em, 0x1e, 0x20, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 10.0f, 0)) {
            Eft20_set(0.7f, em, 0, 0x89);
        }
        break;
    case 0x462:
        sound_call_005A3D10(em, 4, 0x35, 0);
        sound_call_005A3D10(em, 4, 0x16, 0);
        sound_call_005A3D10(em, 0x32, 0x35, 0);
        sound_call_005A3D10(em, 0x46, 0x35, 0);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0x17);
        }
        if (em_frame_check(em, 24.0f, 0) || em_frame_check(em, 42.0f, 0) || em_frame_check(em, 62.0f, 0) || em_frame_check(em, 82.0f, 0)) {
            Eft13_set_em_scl(em, 6, 8.0f + (f32)((u16)ran_suu(1) & 0x3FF) * 0.0005f, 3);
        }
        if (em_frame_check(em, 10.0f, 0) || em_frame_check(em, 32.0f, 0) || em_frame_check(em, 52.0f, 0) || em_frame_check(em, 74.0f, 0)) {
            Eft13_set_em_scl(em, 0xc, 8.0f + (f32)((u16)ran_suu(1) & 0x3FF) * 0.0005f, 3);
        }
        break;
    case 0x464:
        sound_call_005A3D10(em, 0x18, 0x12, 0x14);
        sound_call_005A3D10(em, 0x18, 0x15, 0);
        sound_call_005A3D10(em, 0x36, 0, 0x14);
        sound_call_005A3D10(em, 0x3c, 4, 0x1a);
        sound_call_005A3D10(em, 4, 0x16, 0);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x466:
        sound_call_005A3D10(em, 4, 0x2b, 0x23);
        sound_call_005A3D10(em, 0x5a, 0x30, 0x2a);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x467:
        sound_call_005A3D10(em, 0x1a, 0x2c, 0x23);
        sound_call_005A3D10(em, 0x2e, 8, 0);
        sound_call_005A3D10(em, 4, 0x14, 0);
        sound_call_005A3D10(em, 0x1a, 0x2e, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 42.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x468:
        sound_call_005A3D10(em, 4, 0x2c, 0x23);
        sound_call_005A3D10(em, 4, 0x10, 0);
        sound_call_005A3D10(em, 0x32, 9, 0);
        sound_call_005A3D10(em, 4, 0x1d, 0);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 42.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x469:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x10, 0);
        sound_call_005A3D10(em, 0x32, 9, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0x23);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 46.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x46A:
        sound_call_005A3D10(em, 4, 0x1d, 0);
        sound_call_005A3D10(em, 0x86, 0x1d, 0);
        sound_call_005A3D10(em, 0x86, 0x2f, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x46B:
        sound_call_005A3D10(em, 4, 0x17, 0);
        sound_call_005A3D10(em, 0x20, 0x16, 0);
        sound_call_005A3D10(em, 0x6e, 0x15, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x46C:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x46D:
        sound_call_005A3D10(em, 0xa, 8, 0x14);
        sound_call_005A3D10(em, 4, 0x1f, 0x23);
        sound_call_005A3D10(em, 0xa, 0x12, 0x14);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 6.0f, 0) || em_frame_check(em, 10.0f, 0)) {
            Eft13_set_em_scl(em, 2, 7.0f, 3);
        }
        break;
    case 0x46E:
        sound_call_005A3D10(em, 4, 0x34, 0x23);
        sound_call_005A3D10(em, 0x1e, 0x30, 0);
        sound_call_005A3D10(em, 4, 0x16, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0x1c);
        }
        break;
    case 0x46F:
        sound_call_005A3D10(em, 4, 0x34, 0x23);
        sound_call_005A3D10(em, 0x1e, 0x30, 0);
        sound_call_005A3D10(em, 4, 0x16, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x470:
        sound_call_005A3D10(em, 4, 0x12, 0x1a);
        sound_call_005A3D10(em, 0x12, 6, 0x1a);
        sound_call_005A3D10(em, 0x66, 4, 0x14);
        sound_call_005A3D10(em, 0xd8, 1, 0x1a);
        sound_call_005A3D10(em, 0x108, 3, 0x14);
        sound_call_005A3D10(em, 0x38, 0x15, 0);
        sound_call_005A3D10(em, 0x38, 0x25, 0);
        sound_call_005A3D10(em, 4, 0x1f, 0x23);
        sound_call_005A3D10(em, 0x54, 0x1d, 0);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 60.0f, 0)) {
            atk_shell_set(em, 0x16);
        }
        break;
    case 0x471:
        sound_call_005A3D10(em, 4, 0x2f, 0x23);
        sound_call_005A3D10(em, 0x122, 0x2d, 0x23);
        break;
    case 0x472:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 0x50, 0x29, 0x23);
        sound_call_005A3D10(em, 0x9e, 0x2c, 0x23);
        break;
    default:
        move_default_005A3E00(em);
        break;
    }
}

extern HIRE_E *hire_normal_add_tbl_00659250[4];
extern HIRE_E *hire_down_add_tbl_00659300[4];


static void hire_move_sub2_005A69A0(EMW *em, EM08W *w, int i) {
    switch (w->st[i].b) {
    case 0:
        if (w->xF == 2 || w->xF == 3) {
            if (w->ang[i].a != hire_down_angx_003887E8[i]) {
                w->st[i].b = 2;
                w->tmr[i] = 0x14;
            }
        } else if (w->xF == 1) {
            if (w->ang[i].a != 0) {
                w->st[i].b = 1;
                w->tmr[i] = 0x14;
            }
        }
        break;
    case 1:
        if (--w->tmr[i] <= 0) {
            w->ang[i].a = 0;
            w->st[i].b = 0;
            return;
        }
        w->ang[i].a += ((0x10000 - w->ang[i].a) / w->tmr[i]) & 0xFFFF;
        break;
    case 2:
        if (--w->tmr[i] <= 0) {
            w->ang[i].a = hire_down_angx_003887E8[i];
            w->st[i].b = 0;
            return;
        }
        w->ang[i].a -= (((0x10000 - (hire_down_angx_003887E8[i] - w->ang[i].a)) & 0xFFFF) / w->tmr[i]) & 0xFFFF;
        break;
    }
}

static void hire_move_sub1_005A6B70(EMW *em, EM08W *w, int i) {
    switch (w->st[i].a) {
    case 0:
        if (w->xF == 2) {
            w->st[i].a++;
            w->tm[i] = hire_start_timer_tbl1_003887D0[i];
        } else if (w->xF == 1) {
            w->st[i].a++;
            w->tm[i] = hire_start_timer_tbl0_003887C8[i];
        }
        break;
    case 1:
        if (--w->tm[i] <= 0) {
            if (w->xF == 2) {
                w->st[i].a = 3;
            } else {
                w->st[i].a = 2;
            }
            w->tm[i] = 0;
            w->cnt[i] = 0;
        }
        break;
    case 2: {
        HIRE_E *tbl;
        HIRE_E *p;
        s16 cnt0;
        u16 cnt;
        u8 k;

        cnt0 = w->cnt[i];
        w->cnt[i] = cnt0 + 1;
        cnt = cnt0;
        tbl = hire_normal_add_tbl_00659250[i];
        p = tbl;
        k = 0;
        for (;;) {
            if (k && !p->t) {
                k |= 0x80;
                break;
            }
            if (p->t < cnt) {
                p++;
                k++;
                continue;
            }
            break;
        }
        if (k & 0x80) {
            if (w->xF == 2) {
                w->st[i].a = 1;
                w->tm[i] = hire_remove_timer_tbl1_003887E0[i];
            } else if (w->xF == 1) {
                w->st[i].a = 1;
                w->tm[i] = hire_remove_timer_tbl0_003887D8[i];
            } else {
                w->st[i].a = 0;
            }
            w->ang[i].b = tbl[k & 0x7F].v;
            return;
        }
        w->ang[i].b += tbl[k].v;
        break;
    }
    case 3: {
        HIRE_E *tbl;
        HIRE_E *p;
        s16 cnt0;
        u16 cnt;
        u8 k;

        cnt0 = w->cnt[i];
        w->cnt[i] = cnt0 + 1;
        cnt = cnt0;
        tbl = hire_down_add_tbl_00659300[i];
        p = tbl;
        k = 0;
        for (;;) {
            if (k && !p->t) {
                k |= 0x80;
                break;
            }
            if (p->t < cnt) {
                p++;
                k++;
                continue;
            }
            break;
        }
        if (k & 0x80) {
            if (w->xF == 2) {
                w->st[i].a = 1;
                w->tm[i] = hire_remove_timer_tbl1_003887E0[i];
            } else if (w->xF == 1) {
                w->st[i].a = 1;
                w->tm[i] = hire_remove_timer_tbl0_003887D8[i];
            } else {
                w->st[i].a = 0;
            }
            w->ang[i].b = tbl[k & 0x7F].v;
            return;
        }
        w->ang[i].b += tbl[k].v;
        break;
    }
    }
}

void hire_move_005A6EB0(EMW *em, EM08W *w) {
    int i;

    i = 0;
    do {
        hire_move_sub1_005A6B70(em, w, i);
        hire_move_sub2_005A69A0(em, w, i);
        i++;
    } while (i < 4);
}

void em08_effect_move(EMW *em) {
    EM08W *w = (EM08W *)em->ex;
    u8 e = em->ex[0];

    switch (e) {
    case 0:
        *(u8 *)w = e + 1;
        break;
    case 1:
        ef_move_sub_005A3E50(em, w);
        hire_move_005A6EB0(em, w);
        break;
    }
    em08_uvmove(em);
}
