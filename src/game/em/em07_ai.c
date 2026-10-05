/* em07_ai, run 1: em07_local_area_move_init .. em07_to_normal (game.bin 0x0058F4A0-0x0058FB64). Matching functions of em07_ai_nm.c (that file holds the
 * whole code including the near-matches); see it for the description. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"

/* Raw access to EMW fields that em.h does not name yet. */
#define EMF(em, T, o) (*(T *)((u8 *)(em) + (o)))

/* em07's part of the per-monster work at EMW+0x444 (em07.c has the same start). */
typedef struct EM07W {
    u8 _pad00;
    u8 eff;             /* 0x01 effect script step (em07_effect_move) */
    u8 _pad02[2];
    s16 anim;           /* 0x04 animation the sound/effect script follows */
    s8 x06;             /* 0x06 */
    u8 _pad07;
    s8 item_pt;         /* 0x08 pick point number of the item */
    s8 x09;             /* 0x09 */
    s16 x0A;            /* 0x0A */
    f32 xC;             /* 0x0C */
    f32 dist;           /* 0x10 distance to the target (500 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 has_tgt;         /* 0x16 */
    u8 x17;             /* 0x17 */
    s8 x18;             /* 0x18 */
    s8 x19;             /* 0x19 */
    u8 x1A;             /* 0x1A */
    s8 hagi[3];         /* 0x1B hagi pick point numbers, -1 none */
    s16 x1E;            /* 0x1E */
} EM07W;

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
void em07_act_set(EMW *em, int kind, u16 no, u16 arg);
void em07_to_normal(EMW *em);
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
void em07_item_pos_set(EMW *em, f32 *o);
void em07_hagi_set(EMW *em);
void em07_hagi_move(EMW *em);
void em07_hagi_clr(EMW *em);
void sound_call_005937E0(EMW *em, int frame, int se, int joint, int vol);
void sound_call_mov(EMW *em, int f0, int f1, int se, int joint, int vol);
void sound_call_mov2(EMW *em, int f0, int f1, int se, int joint, int vol);
void quake_call_00593A90(EMW *em, int frame, int v);
void move_default_00593AE0(EMW *em);
void ef_move_sub_00593B30(EMW *em, EM07W *w);
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

#define EM07_TURN(em, tgt)                                                \
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

void shell05_set(EMW *, int);
void shell05_set2(EMW *, f32 *, int);
void Quest_failed_set(EMW *);

void em07_main_sub(EMW *em);

#define UV_RESET(i)       \
    do {                  \
        uv[i][0] = 0.0f;  \
        uv[i][1] = 0.0f;  \
        tm[i] = 0xFFFF;   \
        ty[i] = 0xFF;     \
    } while (0)

void em07_local_area_move_init(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 p[3];

    w->x18 = 0;
    if (w->item_pt != -1) {
        Ext_pick_point_st(w->item_pt, em->stg);
        em07_item_pos_set(em, p);
        Ext_pick_point_pos(w->item_pt, p);
    }
}

void em07_local_init(EMW *em) {
    eft01_set((PLW *)em, 2);
}

void em07_item_pos_set(EMW *em, f32 *o) {
    f32 in[3];
    f32 q[3];
    f32 p[3];

    in[1] = 250.0f;
    in[2] = 120.0f;
    in[0] = 0.0f;
    get_joint_pos_em(em, 3, p);
    flvecApplyMat33(q, in, get_joint_wmat_em(em, 3));
    o[0] = p[0] + q[0];
    o[1] = p[1] + q[1];
    o[2] = p[2] + q[2];
}

void em07_item_set(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    STIEM s;
    f32 pos[3];

    if (w->x09 <= 0) {
        w->item_pt = -1;
    }
    s.rad = 340.0f;
    if (quest_w.x14E == 0) {
        s.id = 0x9B;
    } else {
        s.id = 0x9C;
    }
    s.x14 = 2;
    s.cnt = w->x09;
    s.stg = em->stg;
    s.x19 = 1;
    s.x1A = 0;
    em07_item_pos_set(em, pos);
    w->item_pt = Ext_pick_point_set(&s, pos);
}

void em07_hagi_set(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    STIEM s;
    f32 pos[3];
    int i;

    if (quest_w.x14E == 0) {
        s.id = 0xE4;
    } else {
        s.id = 0xE5;
    }
    for (i = 0; i < 3; i++) {
        s.rad = hagi_r_tbl_00657788[i];
        s.stg = em->stg;
        s.x1A = 0;
        s.x19 = 1;
        get_joint_pos_em(em, hagi_tbl_003886B0[i][0], pos);
        pos[1] = em->x5AC;
        s.x14 = 2;
        s.cnt = hagi_tbl_003886B0[i][1];
        w->hagi[i] = Ext_pick_point_set(&s, pos);
    }
}

void em07_hagi_move(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 pos[3];
    int i;

    for (i = 0; i < 3; i++) {
        if (w->hagi[i] != -1) {
            get_joint_pos_em(em, hagi_tbl_003886B0[i][0], pos);
            pos[1] = em->x5AC;
            Ext_pick_point_pos(w->hagi[i], pos);
            if (Ext_pick_point_cnt_ck(w->hagi[i]) <= 0) {
                Ext_pick_point_clr(w->hagi[i]);
                w->hagi[i] = -1;
            }
        }
    }
}

void em07_hagi_clr(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    int i;

    for (i = 0; i < 3; i++) {
        if (w->hagi[i] != -1) {
            Ext_pick_point_clr(w->hagi[i]);
            w->hagi[i] = -1;
        }
    }
}

void em07_init(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    if (quest_w.no == 0) {
        switch (game_w.stage) {
        case 11:
            em->pos[0] = 33000.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 30200.0f;
            em->ang[1] = 0xC000;
            break;
        case 12:
            em->pos[0] = 7400.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 26300.0f;
            em->ang[1] = 0x4000;
            break;
        case 13:
            em->pos[0] = 5000.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 8000.0f;
            em->ang[1] = 0x8000;
            break;
        case 14:
            em->pos[0] = 2400.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 11000.0f;
            em->ang[1] = 0xE39;
            break;
        case 15:
            em->pos[0] = 7000.0f;
            em->pos[1] = 580.0f;
            em->pos[2] = 7000.0f;
            em->ang[1] = 0;
            break;
        case 28:
            em->pos[0] = 30800.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 32600.0f;
            em->ang[1] = 0x6C00;
            break;
        case 30:
            em->pos[0] = 23500.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 3500.0f;
            em->ang[1] = 0xBA06;
            break;
        default:
            em->pos[0] = 10000.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f;
            em->ang[1] = 0x8000;
            break;
        }
    }
    em->x792 = em->x302 = em_hp_vital_set2(em, 0x2710, 0xD05);
    em->x734 = 3;
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->x888 = 1;
    em->x7EE = 0xF;
    em_char_set(em, 1, 0, 0);
    em_act_set(em, 0, 1);
    w->xC = 1500.0f;
    w->x17 = 0;
    w->x0A = 0;
    w->x18 = 0;
    w->x1E = 0x96;
    w->x09 = 3;
    em07_item_set(em);
}

void em07_to_normal(EMW *em) {
    em->act_spd = 1.0f;
    em->x3F4 = 0;
    em->x839 = 1;
    if (em->x388 == 0) {
        em->x388 = 0;
        em07_act_set(em, 0, 4, 0);
    } else {
        em->x388 = 1;
        em07_act_set(em, 0, 1, 0);
    }
}
