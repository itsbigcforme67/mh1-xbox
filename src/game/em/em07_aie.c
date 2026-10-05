/* em07_ai, run 5: sound_call_005937E0 .. ef_move_sub_00593B30 (game.bin 0x005937E0-0x00599C04). Matching functions of em07_ai_nm.c (that file holds the
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
static void sound_call_005937E0(EMW *em, int frame, int se, int joint, int vol);
static void sound_call_mov(EMW *em, int f0, int f1, int se, int joint, int vol);
void sound_call_mov2(EMW *em, int f0, int f1, int se, int joint, int vol);
static void quake_call_00593A90(EMW *em, int frame, int v);
static void move_default_00593AE0(EMW *em);
static void ef_move_sub_00593B30(EMW *em, EM07W *w);
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

static void sound_call_005937E0(EMW *em, int frame, int se, int joint, int vol) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, 0)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, vol, 0);
    }
}

static void sound_call_mov(EMW *em, int f0, int f1, int se, int joint, int vol) {
    f32 pos[3];

    sound_call_005937E0(em, f0, se, joint, vol);
    if (em_frame_check3(em, 0, (f32)f0, (f32)f1) && (*(u16 *)0x3F340E % 4) == 0) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, vol, 1);
    }
}

void sound_call_mov2(EMW *em, int f0, int f1, int se, int joint, int vol) {
    f32 pos[3];

    sound_call_005937E0(em, f0, se, joint, vol);
    if (em_frame_check3(em, 1, (f32)f0, (f32)f1) && (*(u16 *)0x3F340E % 4) == 0) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, vol, 1);
    }
}

static void quake_call_00593A90(EMW *em, int frame, int v) {
    if (em_frame_check(em, (f32)frame, 0)) {
        Em_set_quake_sub(em, v);
    }
}

static void move_default_00593AE0(EMW *em) {
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

static void ef_move_sub_00593B30(EMW *em, EM07W *w) {

    if (em->char0 != w->anim) {
        w->x06 = 0;
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_mov(em, 0x1cc, 0x24c, 0x46, 0x25, 3);
        sound_call_005937E0(em, 0xa0, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 4, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 4, 0x13, 0x21, 0xa);
        sound_call_005937E0(em, 0x122, 0x13, 0x21, 0xa);
        sound_call_005937E0(em, 4, 0xd, 3, 0xa);
        sound_call_005937E0(em, 0xbe, 0xf, 3, 0xa);
        sound_call_005937E0(em, 0x1b8, 0x11, 3, 0xa);
        if (em_frame_check(em, 4.0f, 0)) {
            shell05_set4(em, 0x47, 0x26);
        }
        if (em_frame_check(em, 74.0f, 0)) {
            shell05_set4(em, 0x47, 0x38);
        }
        if (em_frame_check(em, 172.0f, 0)) {
            shell05_set4(em, 0x47, 0x94);
        }
        break;
    case 0x3EA:
        sound_call_005937E0(em, 4, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0x5a, 0xb, 0x17, 0xa);
        sound_call_005937E0(em, 0x4c, 7, 0x17, 4);
        sound_call_005937E0(em, 0x4c, 3, 0x17, 3);
        sound_call_005937E0(em, 0x32, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0xb2, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0xa4, 5, 0xe, 4);
        sound_call_005937E0(em, 0xa4, 1, 0xe, 3);
        sound_call_005937E0(em, 0x8a, 0xe, 0x13, 0xa);
        sound_call_005937E0(em, 0x116, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x108, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x108, 0, 0x1d, 3);
        sound_call_005937E0(em, 0xee, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0x17a, 8, 8, 0xa);
        sound_call_005937E0(em, 0x16c, 5, 8, 4);
        sound_call_005937E0(em, 0x16c, 1, 8, 3);
        sound_call_005937E0(em, 0x152, 0x13, 0x19, 0xa);
        quake_call_00593A90(em, 0x92, 2);
        if (em_frame_check(em, 146.0f, 0)) {
            Shell22_set3(em, 4, w->x1A);
            w->x1A++;
            w->x1A &= 3;
        }
        quake_call_00593A90(em, 0xb2, 2);
        quake_call_00593A90(em, 0x122, 2);
        if (em_frame_check(em, 290.0f, 0)) {
            Shell22_set3(em, 4, w->x1A);
            w->x1A++;
            w->x1A &= 3;
        }
        quake_call_00593A90(em, 0x17e, 2);
        if (em_frame_check(em, 68.0f, 0)) {
            shell05_set(em, 5);
        }
        if (em_frame_check(em, 246.0f, 0)) {
            shell05_set(em, 6);
        }
        if (em_frame_check(em, 350.0f, 0)) {
            shell05_set(em, 7);
        }
        if (em_frame_check(em, 144.0f, 0)) {
            shell05_set(em, 8);
        }
        if (em_frame_check(em, 176.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 380.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 284.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 136.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x3EB:
        sound_call_mov(em, 0x60, 0x1c2, 0x47, 0x25, 3);
        sound_call_005937E0(em, 0xde, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0x178, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x2a, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x1c, 5, 0xe, 4);
        sound_call_005937E0(em, 0x1c, 1, 0xe, 3);
        sound_call_005937E0(em, 4, 0xe, 0x13, 0xa);
        sound_call_005937E0(em, 0x9a, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x8c, 4, 0x17, 4);
        sound_call_005937E0(em, 0x8c, 0, 0x17, 3);
        sound_call_005937E0(em, 0x72, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0xfa, 8, 8, 0xa);
        sound_call_005937E0(em, 0xec, 5, 8, 4);
        sound_call_005937E0(em, 0xec, 1, 8, 3);
        sound_call_005937E0(em, 0xc8, 0x13, 0x19, 0xa);
        sound_call_005937E0(em, 0x16a, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x15c, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x15c, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x136, 0x10, 5, 0xa);
        quake_call_00593A90(em, 0x2c, 2);
        if (em_frame_check(em, 44.0f, 0)) {
            Shell22_set3(em, 4, w->x1A);
            w->x1A++;
            w->x1A &= 3;
        }
        quake_call_00593A90(em, 0x9a, 2);
        quake_call_00593A90(em, 0xf8, 2);
        if (em_frame_check(em, 248.0f, 0)) {
            Shell22_set3(em, 4, w->x1A);
            w->x1A++;
            w->x1A &= 3;
        }
        quake_call_00593A90(em, 0x16e, 2);
        if (em_frame_check(em, 130.0f, 0)) {
            shell05_set(em, 1);
        }
        if (em_frame_check(em, 314.0f, 0)) {
            shell05_set(em, 2);
        }
        if (em_frame_check(em, 222.0f, 0)) {
            shell05_set(em, 3);
        }
        if (em_frame_check(em, 18.0f, 0)) {
            shell05_set(em, 4);
        }
        if (em_frame_check(em, 108.0f, 0)) {
            shell05_set4(em, 0x47, 0x42);
        }
        if (em_frame_check(em, 250.0f, 0)) {
            shell05_set4(em, 0x47, 0x22);
        }
        if (em_frame_check(em, 346.0f, 0)) {
            shell05_set4(em, 0x47, 0x3e);
        }
        if (em_frame_check(em, 442.0f, 0)) {
            shell05_set4(em, 0x47, 0x14);
        }
        if (em_frame_check(em, 44.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 254.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 360.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 152.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x3EC:
        sound_call_005937E0(em, 0xc2, 0x15, 0x30, 0xa);
        sound_call_005937E0(em, 0x18a, 0x14, 0x32, 0xa);
        sound_call_005937E0(em, 0x4e, 8, 8, 0xa);
        sound_call_005937E0(em, 0x40, 5, 8, 4);
        sound_call_005937E0(em, 0x40, 1, 8, 3);
        sound_call_005937E0(em, 0x26, 0x13, 0x19, 0xa);
        sound_call_005937E0(em, 0xc2, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0xb4, 4, 0x1d, 4);
        sound_call_005937E0(em, 0xb4, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x9a, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0x138, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x12a, 5, 0xe, 4);
        sound_call_005937E0(em, 0x12a, 1, 0xe, 3);
        sound_call_005937E0(em, 0x110, 0xe, 0x13, 0xa);
        sound_call_005937E0(em, 0x1a2, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x194, 4, 0x17, 4);
        sound_call_005937E0(em, 0x194, 0, 0x17, 3);
        sound_call_005937E0(em, 0x17a, 0xc, 0xb, 0xa);
        quake_call_00593A90(em, 0x52, 2);
        quake_call_00593A90(em, 0xc4, 2);
        quake_call_00593A90(em, 0x13a, 2);
        quake_call_00593A90(em, 0x1aa, 2);
        if (em_frame_check(em, 380.0f, 0)) {
            shell05_set(em, 9);
        }
        if (em_frame_check(em, 170.0f, 0)) {
            shell05_set(em, 0xa);
        }
        if (em_frame_check(em, 50.0f, 0)) {
            shell05_set(em, 0xb);
        }
        if (em_frame_check(em, 276.0f, 0)) {
            shell05_set(em, 0xc);
        }
        if (em_frame_check(em, 96.0f, 0)) {
            shell05_set4(em, 0x47, 0x1a);
        }
        if (em_frame_check(em, 158.0f, 0)) {
            shell05_set4(em, 0x47, 0x3c);
        }
        if (em_frame_check(em, 352.0f, 0)) {
            shell05_set4(em, 0x47, 0x1a);
        }
        if (em_frame_check(em, 394.0f, 0)) {
            shell05_set4(em, 0x47, 0x38);
        }
        if (em_frame_check(em, 310.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 80.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 192.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 424.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x3ED:
        sound_call_005937E0(em, 0xc2, 0x15, 0x30, 0xa);
        sound_call_005937E0(em, 0x18a, 0x14, 0x32, 0xa);
        sound_call_005937E0(em, 0x4e, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x40, 5, 0xe, 4);
        sound_call_005937E0(em, 0x40, 1, 0xe, 3);
        sound_call_005937E0(em, 0x26, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0xc2, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0xb4, 4, 0x17, 4);
        sound_call_005937E0(em, 0xb4, 0, 0x17, 3);
        sound_call_005937E0(em, 0x9a, 0x10, 0xb, 0xa);
        sound_call_005937E0(em, 0x138, 8, 8, 0xa);
        sound_call_005937E0(em, 0x12a, 5, 8, 4);
        sound_call_005937E0(em, 0x12a, 1, 8, 3);
        sound_call_005937E0(em, 0x110, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x1a2, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x194, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x194, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x17a, 0xc, 5, 0xa);
        quake_call_00593A90(em, 0x52, 2);
        quake_call_00593A90(em, 0xc4, 2);
        quake_call_00593A90(em, 0x13a, 2);
        quake_call_00593A90(em, 0x1aa, 2);
        if (em_frame_check(em, 170.0f, 0)) {
            shell05_set(em, 0xd);
        }
        if (em_frame_check(em, 380.0f, 0)) {
            shell05_set(em, 0xe);
        }
        if (em_frame_check(em, 276.0f, 0)) {
            shell05_set(em, 0xf);
        }
        if (em_frame_check(em, 50.0f, 0)) {
            shell05_set(em, 0x10);
        }
        if (em_frame_check(em, 96.0f, 0)) {
            shell05_set4(em, 0x47, 0x1a);
        }
        if (em_frame_check(em, 158.0f, 0)) {
            shell05_set4(em, 0x47, 0x3c);
        }
        if (em_frame_check(em, 352.0f, 0)) {
            shell05_set4(em, 0x47, 0x1a);
        }
        if (em_frame_check(em, 394.0f, 0)) {
            shell05_set4(em, 0x47, 0x38);
        }
        if (em_frame_check(em, 80.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 310.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 424.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 192.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x3EE:
        sound_call_005937E0(em, 4, 0x17, 0x23, 0xa);
        sound_call_005937E0(em, 4, 0x13, 0x21, 0xa);
        sound_call_mov(em, 0x9c, 0xc6, 0x22, 0x25, 0xa);
        sound_call_005937E0(em, 0x42, 0xb, 0x17, 0xa);
        sound_call_005937E0(em, 0x34, 7, 0x17, 4);
        sound_call_005937E0(em, 0x34, 3, 0x17, 3);
        sound_call_005937E0(em, 0xa, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0x98, 7, 0xe, 4);
        sound_call_005937E0(em, 0x98, 3, 0xe, 3);
        sound_call_005937E0(em, 0x8c, 0xc, 0x13, 0xa);
        quake_call_00593A90(em, 0x52, 2);
        quake_call_00593A90(em, 0xaa, 2);
        if (em_frame_check(em, 40.0f, 0)) {
            shell05_set(em, 0x11);
        }
        if (em_frame_check(em, 80.0f, 0)) {
            shell05_set4(em, 0x47, 0x64);
        }
        if (em_frame_check(em, 158.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 70.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x3EF:
        sound_call_mov(em, 2, 0x1e0, 0x48, 0x25, 4);
        sound_call_005937E0(em, 0x3c, 0x1e, 3, 0xa);
        sound_call_005937E0(em, 0x16e, 0xb, 0x17, 0xa);
        sound_call_005937E0(em, 0x160, 7, 0x17, 4);
        sound_call_005937E0(em, 0x160, 3, 0x17, 3);
        sound_call_005937E0(em, 0x138, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0x1b6, 7, 0xe, 4);
        sound_call_005937E0(em, 0x1b6, 3, 0xe, 3);
        sound_call_005937E0(em, 0x186, 0xc, 0x13, 0xa);
        quake_call_00593A90(em, 0x17c, 2);
        quake_call_00593A90(em, 0x1cc, 2);
        if (em_frame_check(em, 326.0f, 0)) {
            shell05_set(em, 0x12);
        }
        if (em_frame_check(em, 80.0f, 0)) {
            shell05_set4(em, 0x47, 0xc8);
        }
        if (em_frame_check(em, 450.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 368.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            shell05_set(em, 0x51);
            Eft15_set3(em, 5, 1.5f, 0x14);
        }
        break;
    case 0x3F0:
        sound_call_mov(em, 0x108, 0x294, 0x48, 0x25, 4);
        sound_call_mov(em, 0x108, 0x230, 0x1f, 3, 8);
        sound_call_005937E0(em, 0x118, 0x17, 0x23, 0xa);
        sound_call_005937E0(em, 4, 0xe, 0x23, 0xa);
        sound_call_005937E0(em, 0xcc, 0xc, 0x23, 0xa);
        sound_call_005937E0(em, 0x186, 0x10, 0x13, 0xa);
        sound_call_005937E0(em, 0x186, 0x12, 0x19, 0xa);
        sound_call_005937E0(em, 0x5a, 0x17, 0x30, 0xa);
        sound_call_005937E0(em, 0x10e, 0x16, 0x32, 0xa);
        sound_call_005937E0(em, 0xfc, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0xfc, 5, 0xe, 4);
        sound_call_005937E0(em, 0xfc, 1, 0xe, 3);
        sound_call_005937E0(em, 0x100, 8, 8, 0xa);
        sound_call_005937E0(em, 0x100, 5, 8, 4);
        sound_call_005937E0(em, 0x100, 1, 8, 3);
        if (em_frame_check(em, 326.0f, 0)) {
            shell05_set(em, 0x13);
        }
        if (em_frame_check(em, 326.0f, 0)) {
            shell05_set(em, 0x14);
        }
        break;
    case 0x3F2:
        sound_call_005937E0(em, 0x13c, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0xc8, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0xc4, 0x17, 0x23, 0xa);
        sound_call_005937E0(em, 0xc4, 0, 0xb, 0xa);
        sound_call_005937E0(em, 0xd2, 0x18, 0xb, 4);
        sound_call_005937E0(em, 0xd6, 0x19, 0xb, 0xa);
        sound_call_005937E0(em, 0xd6, 0x1b, 0x21, 0xa);
        sound_call_005937E0(em, 0xd6, 8, 0x21, 4);
        sound_call_mov(em, 0x140, 0x1e0, 0x26, 0x25, 0xa);
        sound_call_005937E0(em, 0x38, 8, 8, 0xa);
        sound_call_005937E0(em, 0x2a, 5, 8, 4);
        sound_call_005937E0(em, 0x2a, 1, 8, 3);
        sound_call_005937E0(em, 0x10, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x64, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x56, 5, 0xe, 4);
        sound_call_005937E0(em, 0x56, 1, 0xe, 3);
        sound_call_005937E0(em, 0x3c, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x8c, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x7e, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x7e, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x64, 0xc, 5, 0xa);
        sound_call_005937E0(em, 0xc2, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0xb4, 4, 0x17, 4);
        sound_call_005937E0(em, 0xb4, 0, 0x17, 3);
        sound_call_005937E0(em, 0x9a, 0x10, 0xb, 0xa);
        sound_call_005937E0(em, 0x198, 9, 0x17, 0xa);
        sound_call_005937E0(em, 0x18a, 4, 0x17, 4);
        sound_call_005937E0(em, 0x18a, 0, 0x17, 3);
        sound_call_005937E0(em, 0x170, 0x10, 0xb, 0xa);
        sound_call_005937E0(em, 0x1aa, 9, 0xe, 0xa);
        sound_call_005937E0(em, 0x19c, 5, 0xe, 4);
        sound_call_005937E0(em, 0x19c, 1, 0xe, 3);
        sound_call_005937E0(em, 0x182, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x1d2, 9, 8, 0xa);
        sound_call_005937E0(em, 0x1c4, 5, 8, 4);
        sound_call_005937E0(em, 0x1c4, 1, 8, 3);
        sound_call_005937E0(em, 0x1aa, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x1fa, 9, 0x1d, 0xa);
        sound_call_005937E0(em, 0x1ec, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x1ec, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x1d2, 0xc, 5, 0xa);
        sound_call_005937E0(em, 0x212, 5, 0xe, 4);
        sound_call_005937E0(em, 0x212, 1, 0xe, 3);
        sound_call_005937E0(em, 0x1f8, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x228, 4, 0x17, 4);
        sound_call_005937E0(em, 0x228, 0, 0x17, 3);
        sound_call_005937E0(em, 0x20e, 0x10, 0xb, 0xa);
        quake_call_00593A90(em, 0x3e, 2);
        quake_call_00593A90(em, 0x6a, 2);
        quake_call_00593A90(em, 0x96, 2);
        quake_call_00593A90(em, 0xd2, 5);
        quake_call_00593A90(em, 0x1a4, 2);
        quake_call_00593A90(em, 0x1dc, 2);
        quake_call_00593A90(em, 0x1fe, 2);
        quake_call_00593A90(em, 0x222, 2);
        if (em_frame_check(em, 150.0f, 0)) {
            shell05_set(em, 0x15);
        }
        if (em_frame_check(em, 102.0f, 0)) {
            shell05_set(em, 0x16);
        }
        if (em_frame_check(em, 24.0f, 0)) {
            shell05_set(em, 0x17);
        }
        if (em_frame_check(em, 64.0f, 0)) {
            shell05_set(em, 0x18);
        }
        if (em_frame_check(em, 184.0f, 0)) {
            shell05_set(em, 0x19);
        }
        if (em_frame_check(em, 322.0f, 0)) {
            shell05_set(em, 0x1a);
        }
        if (em_frame_check(em, 434.0f, 0)) {
            shell05_set(em, 0x1a);
        }
        if (em_frame_check(em, 210.0f, 0)) {
            shell05_set(em, 0x55);
        }
        if (em_frame_check(em, 98.0f, 0) || em_frame_check(em, 424.0f, 0) || em_frame_check(em, 540.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 58.0f, 0) || em_frame_check(em, 468.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 136.0f, 0) || em_frame_check(em, 504.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 192.0f, 0) || em_frame_check(em, 404.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x3F4:
        sound_call_mov(em, 6, 0x168, 0x47, 0x25, 3);
        sound_call_005937E0(em, 0x18, 0x13, 3, 0xa);
        sound_call_005937E0(em, 0x88, 0x14, 0x32, 0xa);
        if (em_frame_check(em, 130.0f, 0)) {
            shell05_set4(em, 0x47, 0x4c);
        }
        if (em_frame_check(em, 320.0f, 0)) {
            shell05_set4(em, 0x47, 0x42);
        }
        break;
    case 0x3F5:
        sound_call_005937E0(em, 6, 0x22, 0x25, 0xa);
        sound_call_mov(em, 0x8c, 0x12c, 0x23, 0x25, 4);
        sound_call_005937E0(em, 4, 0xd, 3, 0xa);
        sound_call_005937E0(em, 0x112, 0xd, 3, 0xa);
        sound_call_005937E0(em, 4, 0x15, 0x32, 0xa);
        sound_call_005937E0(em, 0x96, 0x17, 0x30, 0xa);
        sound_call_005937E0(em, 0xa0, 7, 0x30, 0xa);
        sound_call_005937E0(em, 0xa0, 2, 0x30, 0xa);
        sound_call_005937E0(em, 0x4a, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x3c, 4, 0x17, 4);
        sound_call_005937E0(em, 0x3c, 0, 0x17, 3);
        sound_call_005937E0(em, 0x12, 0x13, 0xb, 0xa);
        sound_call_005937E0(em, 0x1fc, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x1ee, 4, 0x17, 4);
        sound_call_005937E0(em, 0x1ee, 0, 0x17, 3);
        sound_call_005937E0(em, 0x1c6, 0x13, 0xb, 0xa);
        quake_call_00593A90(em, 0x4c, 2);
        quake_call_00593A90(em, 0x1fa, 2);
        if (em_frame_check(em, 162.0f, 0)) {
            shell05_set(em, 0x1b);
        }
        if (em_frame_check(em, 48.0f, 0)) {
            shell05_set(em, 0x1c);
        }
        if (em_frame_check(em, 442.0f, 0)) {
            shell05_set(em, 0x1d);
        }
        if (em_frame_check(em, 152.0f, 0)) {
            shell05_set4(em, 0x47, 0xa);
        }
        if (em_frame_check(em, 364.0f, 0)) {
            shell05_set4(em, 0x47, 0x4c);
        }
        if (em_frame_check(em, 500.0f, 0)) {
            shell05_set4(em, 0x47, 0x3a);
        }
        if (em_frame_check(em, 74.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        if (em_frame_check(em, 494.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x3F6:
        sound_call_005937E0(em, 0x8e, 0x17, 0x23, 0xa);
        sound_call_005937E0(em, 0x88, 0, 0x23, 0xa);
        sound_call_005937E0(em, 0x88, 0x18, 0x23, 4);
        sound_call_005937E0(em, 0x96, 0x19, 0x23, 0xa);
        sound_call_005937E0(em, 0x96, 8, 0x23, 4);
        sound_call_005937E0(em, 0x10, 0x12, 0x21, 0xa);
        sound_call_005937E0(em, 0xba, 0xd, 3, 0xa);
        sound_call_mov(em, 0xf6, 0x190, 0x4e, 0x25, 0xa);
        sound_call_005937E0(em, 0x54, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x46, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x46, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x1e, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0xfa, 0xa, 0x1d, 0xa);
        sound_call_005937E0(em, 0xec, 4, 0x1d, 4);
        sound_call_005937E0(em, 0xec, 0, 0x1d, 3);
        sound_call_005937E0(em, 0xc4, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x44, 0x16, 0x32, 0xa);
        sound_call_005937E0(em, 0x58, 0x17, 0x30, 0xa);
        sound_call_005937E0(em, 0xc4, 0x14, 0x30, 0xa);
        quake_call_00593A90(em, 0x56, 2);
        quake_call_00593A90(em, 0x9a, 5);
        quake_call_00593A90(em, 0x106, 2);
        if (em_frame_check(em, 134.0f, 0)) {
            shell05_set(em, 0x1e);
        }
        if (em_frame_check(em, 60.0f, 0)) {
            shell05_set(em, 0x1f);
        }
        if (em_frame_check(em, 216.0f, 0)) {
            shell05_set(em, 0x20);
        }
        if (em_frame_check(em, 156.0f, 0)) {
            shell05_set4(em, 0x47, 0x2c);
        }
        if (em_frame_check(em, 252.0f, 0)) {
            shell05_set4(em, 0x47, 0x1c);
        }
        if (em_frame_check(em, 82.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.5f, 0x13);
        }
        if (em_frame_check(em, 258.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.5f, 0x13);
        }
        break;
    case 0x3F7:
        sound_call_005937E0(em, 0x3a, 0x26, 0x25, 0xa);
        sound_call_005937E0(em, 0x140, 0x23, 0x25, 0xa);
        sound_call_005937E0(em, 4, 0xf, 3, 0xa);
        sound_call_005937E0(em, 0x122, 0x10, 3, 0xa);
        sound_call_005937E0(em, 0x78, 0xa, 0x17, 0xa);
        sound_call_005937E0(em, 0x6a, 7, 0x17, 4);
        sound_call_005937E0(em, 0x6a, 3, 0x17, 3);
        sound_call_005937E0(em, 0x3c, 0xc, 0x13, 0xa);
        sound_call_005937E0(em, 0x118, 0xa, 0x1d, 0xa);
        sound_call_005937E0(em, 0x10a, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x10a, 0, 0x1d, 3);
        sound_call_005937E0(em, 0xf6, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x19a, 0xa, 0x17, 0xa);
        sound_call_005937E0(em, 0x18c, 4, 0x17, 4);
        sound_call_005937E0(em, 0x18c, 0, 0x17, 3);
        sound_call_005937E0(em, 0x172, 0x13, 0xb, 0xa);
        sound_call_005937E0(em, 0x1dc, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x1dc, 5, 0xe, 4);
        sound_call_005937E0(em, 0x1dc, 1, 0xe, 3);
        sound_call_005937E0(em, 0x1e0, 8, 8, 0xa);
        sound_call_005937E0(em, 0x1e0, 5, 8, 4);
        sound_call_005937E0(em, 0x1e0, 1, 8, 3);
        sound_call_005937E0(em, 0x1fe, 0x11, 3, 0xa);
        sound_call_005937E0(em, 0x216, 0x18, 0x21, 0xa);
        sound_call_005937E0(em, 0x1fa, 0x17, 0x32, 0xa);
        quake_call_00593A90(em, 0x84, 2);
        quake_call_00593A90(em, 0x11c, 2);
        quake_call_00593A90(em, 0x1da, 5);
        if (em_frame_check(em, 70.0f, 0)) {
            shell05_set(em, 0x21);
        }
        if (em_frame_check(em, 348.0f, 0)) {
            shell05_set(em, 0x22);
        }
        if (em_frame_check(em, 204.0f, 0)) {
            shell05_set(em, 0x23);
        }
        if (em_frame_check(em, 458.0f, 0)) {
            shell05_set(em, 0x24);
        }
        if (em_frame_check(em, 458.0f, 0)) {
            shell05_set(em, 0x25);
        }
        if (em_frame_check(em, 64.0f, 0)) {
            shell05_set4(em, 0x47, 0x2a);
        }
        if (em_frame_check(em, 250.0f, 0)) {
            shell05_set4(em, 0x47, 0x32);
        }
        if (em_frame_check(em, 370.0f, 0)) {
            shell05_set4(em, 0x47, 0x32);
        }
        if (em_frame_check(em, 464.0f, 0)) {
            shell05_set4(em, 0x47, 0x12);
        }
        if (em_frame_check(em, 472.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.8f, 0x13);
        }
        if (em_frame_check(em, 470.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.8f, 0x13);
        }
        if (em_frame_check(em, 274.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 114.0f, 0) || em_frame_check(em, 394.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x3F8:
        sound_call_005937E0(em, 0xc8, 0x1e, 0x25, 0xa);
        sound_call_005937E0(em, 0x18, 0xa, 0x1d, 0xa);
        sound_call_005937E0(em, 0xa, 4, 0x1d, 4);
        sound_call_005937E0(em, 0xa, 0, 0x1d, 3);
        sound_call_005937E0(em, 4, 0xe, 0x13, 0xa);
        sound_call_005937E0(em, 0xa4, 0xa, 0x17, 0xa);
        sound_call_005937E0(em, 0x96, 4, 0x17, 4);
        sound_call_005937E0(em, 0x96, 0, 0x17, 3);
        sound_call_005937E0(em, 0x90, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x1a, 0x14, 0x32, 0xa);
        quake_call_00593A90(em, 0x1e, 2);
        quake_call_00593A90(em, 0xa8, 2);
        if (em_frame_check(em, 26.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.5f, 0x13);
        }
        if (em_frame_check(em, 164.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        if (em_frame_check(em, 148.0f, 0)) {
            shell05_set(em, 0x48);
        }
        if (em_frame_check(em, 12.0f, 0)) {
            shell05_set(em, 0x49);
        }
        if (em_frame_check(em, 182.0f, 0)) {
            shell05_set(em, 0x4a);
        }
        if (em_frame_check(em, 24.0f, 0)) {
            shell05_set(em, 0x4b);
        }
        if (em_frame_check(em, 24.0f, 0)) {
            shell05_set4(em, 0x47, 0x24);
        }
        if (em_frame_check(em, 170.0f, 0)) {
            shell05_set4(em, 0x47, 0x28);
        }
        break;
    case 0x3FC:
        sound_call_005937E0(em, 0x4e, 9, 8, 0xa);
        sound_call_005937E0(em, 0x40, 5, 8, 4);
        sound_call_005937E0(em, 0x40, 1, 8, 3);
        sound_call_005937E0(em, 0x26, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0xa4, 9, 0xe, 0xa);
        sound_call_005937E0(em, 0x96, 5, 0xe, 4);
        sound_call_005937E0(em, 0x96, 1, 0xe, 3);
        sound_call_005937E0(em, 0x7c, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x5e, 0x17, 0x32, 0xa);
        quake_call_00593A90(em, 0x52, 2);
        quake_call_00593A90(em, 0xa6, 2);
        if (em_frame_check(em, 60.0f, 0)) {
            shell05_set(em, 0x26);
        }
        if (em_frame_check(em, 156.0f, 0)) {
            shell05_set(em, 0x27);
        }
        if (em_frame_check(em, 100.0f, 0)) {
            shell05_set4(em, 0x47, 0x24);
        }
        if (em_frame_check(em, 154.0f, 0)) {
            shell05_set4(em, 0x47, 0x2e);
        }
        if (em_frame_check(em, 162.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 76.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        break;
    case 0x3FD:
    case 0x403:
        sound_call_mov(em, 0x120, 0x18e, 0x4e, 0x25, 3);
        sound_call_005937E0(em, 4, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0x140, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 4, 0x13, 0x21, 0xa);
        sound_call_005937E0(em, 0x122, 0x13, 0x21, 0xa);
        sound_call_005937E0(em, 4, 0xd, 3, 0xa);
        sound_call_005937E0(em, 0xbe, 0xf, 3, 0xa);
        if (em_frame_check(em, 28.0f, 0)) {
            shell05_set4(em, 0x47, 0x34);
        }
        if (em_frame_check(em, 154.0f, 0)) {
            shell05_set4(em, 0x47, 0x32);
        }
        break;
    case 0x3FE:
        sound_call_005937E0(em, 0x4e, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x40, 5, 0xe, 4);
        sound_call_005937E0(em, 0x40, 1, 0xe, 3);
        sound_call_005937E0(em, 0x26, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x8a, 8, 8, 0xa);
        sound_call_005937E0(em, 0x7c, 5, 8, 4);
        sound_call_005937E0(em, 0x7c, 1, 8, 3);
        sound_call_005937E0(em, 0x62, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x30, 0x16, 0x30, 0xa);
        quake_call_00593A90(em, 0x50, 2);
        quake_call_00593A90(em, 0x8c, 2);
        if (em_frame_check(em, 104.0f, 0)) {
            shell05_set(em, 0x28);
        }
        if (em_frame_check(em, 50.0f, 0)) {
            shell05_set(em, 0x29);
        }
        if (em_frame_check(em, 10.0f, 0)) {
            shell05_set4(em, 0x47, 0x2c);
        }
        if (em_frame_check(em, 74.0f, 0)) {
            shell05_set4(em, 0x47, 0x48);
        }
        if (em_frame_check(em, 78.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 138.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        break;
    case 0x3FF:
        sound_call_005937E0(em, 0x48, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x3a, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x3a, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x20, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0x9a, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x8c, 4, 0x17, 4);
        sound_call_005937E0(em, 0x8c, 0, 0x17, 3);
        sound_call_005937E0(em, 0x72, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 4, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0x2a, 0x17, 0x32, 0xa);
        quake_call_00593A90(em, 0x4a, 2);
        quake_call_00593A90(em, 0x9e, 2);
        if (em_frame_check(em, 132.0f, 0)) {
            shell05_set(em, 0x2a);
        }
        if (em_frame_check(em, 46.0f, 0)) {
            shell05_set(em, 0x2b);
        }
        if (em_frame_check(em, 30.0f, 0)) {
            shell05_set4(em, 0x47, 0x1e);
        }
        if (em_frame_check(em, 82.0f, 0)) {
            shell05_set4(em, 0x47, 0x4e);
        }
        if (em_frame_check(em, 70.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 154.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x400:
        sound_call_005937E0(em, 0x52, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x44, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x44, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x2a, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0xa4, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x96, 4, 0x17, 4);
        sound_call_005937E0(em, 0x96, 0, 0x17, 3);
        sound_call_005937E0(em, 0x7c, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0xf8, 8, 8, 0xa);
        sound_call_005937E0(em, 0xea, 5, 8, 4);
        sound_call_005937E0(em, 0xea, 1, 8, 3);
        sound_call_005937E0(em, 0xd0, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x14e, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x140, 5, 0xe, 4);
        sound_call_005937E0(em, 0x140, 1, 0xe, 3);
        sound_call_005937E0(em, 0x12a, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x38, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x11e, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x90, 0x16, 0x30, 0xa);
        quake_call_00593A90(em, 0x52, 2);
        quake_call_00593A90(em, 0xa6, 2);
        quake_call_00593A90(em, 0xfc, 2);
        quake_call_00593A90(em, 0x150, 2);
        if (em_frame_check(em, 120.0f, 0)) {
            shell05_set(em, 0x2c);
        }
        if (em_frame_check(em, 48.0f, 0)) {
            shell05_set(em, 0x2d);
        }
        if (em_frame_check(em, 216.0f, 0)) {
            shell05_set(em, 0x2e);
        }
        if (em_frame_check(em, 306.0f, 0)) {
            shell05_set(em, 0x2f);
        }
        if (em_frame_check(em, 50.0f, 0)) {
            shell05_set4(em, 0x47, 0x26);
        }
        if (em_frame_check(em, 120.0f, 0)) {
            shell05_set4(em, 0x47, 0x7a);
        }
        if (em_frame_check(em, 266.0f, 0)) {
            shell05_set4(em, 0x47, 0x40);
        }
        if (em_frame_check(em, 334.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 248.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 78.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 162.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x401:
        sound_call_005937E0(em, 0x4a, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x3c, 4, 0x17, 4);
        sound_call_005937E0(em, 0x3c, 0, 0x17, 3);
        sound_call_005937E0(em, 0x22, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0x9c, 8, 8, 0xa);
        sound_call_005937E0(em, 0x8e, 5, 8, 4);
        sound_call_005937E0(em, 0x8e, 1, 8, 3);
        sound_call_005937E0(em, 0x74, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0xcc, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0xbe, 5, 0xe, 4);
        sound_call_005937E0(em, 0xbe, 1, 0xe, 3);
        sound_call_005937E0(em, 0xa4, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x116, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x108, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x108, 0, 0x1d, 3);
        sound_call_005937E0(em, 0xee, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0x166, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x158, 4, 0x17, 4);
        sound_call_005937E0(em, 0x158, 0, 0x17, 3);
        sound_call_005937E0(em, 0x13e, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0x66, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x154, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x2c, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0xf4, 0x16, 0x30, 0xa);
        quake_call_00593A90(em, 0x50, 2);
        quake_call_00593A90(em, 0xa2, 2);
        quake_call_00593A90(em, 0xce, 2);
        quake_call_00593A90(em, 0x11c, 2);
        quake_call_00593A90(em, 0x16a, 2);
        if (em_frame_check(em, 34.0f, 0)) {
            shell05_set(em, 0x30);
        }
        if (em_frame_check(em, 208.0f, 0)) {
            shell05_set(em, 0x31);
        }
        if (em_frame_check(em, 96.0f, 0)) {
            shell05_set(em, 0x32);
        }
        if (em_frame_check(em, 162.0f, 0)) {
            shell05_set(em, 0x33);
        }
        if (em_frame_check(em, 300.0f, 0)) {
            shell05_set(em, 0x4c);
        }
        if (em_frame_check(em, 368.0f, 0)) {
            shell05_set(em, 0x4d);
        }
        if (em_frame_check(em, 18.0f, 0)) {
            shell05_set4(em, 0x47, 0x56);
        }
        if (em_frame_check(em, 280.0f, 0)) {
            shell05_set4(em, 0x47, 0x8c);
        }
        if (em_frame_check(em, 204.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 158.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 280.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 76.0f, 0) || em_frame_check(em, 358.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x402:
        sound_call_005937E0(em, 0x4e, 9, 0xe, 0xa);
        sound_call_005937E0(em, 0x40, 5, 0xe, 4);
        sound_call_005937E0(em, 0x40, 1, 0xe, 3);
        sound_call_005937E0(em, 0x26, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0xa4, 9, 8, 0xa);
        sound_call_005937E0(em, 0x96, 5, 8, 4);
        sound_call_005937E0(em, 0x96, 1, 8, 3);
        sound_call_005937E0(em, 0x7c, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x5e, 0x17, 0x32, 0xa);
        quake_call_00593A90(em, 0x52, 2);
        quake_call_00593A90(em, 0xa6, 2);
        if (em_frame_check(em, 156.0f, 0)) {
            shell05_set(em, 0x34);
        }
        if (em_frame_check(em, 60.0f, 0)) {
            shell05_set(em, 0x35);
        }
        if (em_frame_check(em, 100.0f, 0)) {
            shell05_set4(em, 0x47, 0x24);
        }
        if (em_frame_check(em, 154.0f, 0)) {
            shell05_set4(em, 0x47, 0x2e);
        }
        if (em_frame_check(em, 76.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 162.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        break;
    case 0x404:
        sound_call_005937E0(em, 0x4e, 8, 8, 0xa);
        sound_call_005937E0(em, 0x40, 5, 8, 4);
        sound_call_005937E0(em, 0x40, 1, 8, 3);
        sound_call_005937E0(em, 0x26, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x8a, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x7c, 5, 0xe, 4);
        sound_call_005937E0(em, 0x7c, 1, 0xe, 3);
        sound_call_005937E0(em, 0x62, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x30, 0x16, 0x30, 0xa);
        quake_call_00593A90(em, 0x50, 2);
        quake_call_00593A90(em, 0x8c, 2);
        if (em_frame_check(em, 50.0f, 0)) {
            shell05_set(em, 0x36);
        }
        if (em_frame_check(em, 104.0f, 0)) {
            shell05_set(em, 0x37);
        }
        if (em_frame_check(em, 10.0f, 0)) {
            shell05_set4(em, 0x47, 0x2c);
        }
        if (em_frame_check(em, 74.0f, 0)) {
            shell05_set4(em, 0x47, 0x48);
        }
        if (em_frame_check(em, 138.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 78.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        break;
    case 0x405:
        sound_call_005937E0(em, 0x48, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x3a, 4, 0x17, 4);
        sound_call_005937E0(em, 0x3a, 0, 0x17, 3);
        sound_call_005937E0(em, 0x20, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0x9a, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x8c, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x8c, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x72, 0x10, 5, 0xa);
        sound_call_005937E0(em, 4, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0x2a, 0x17, 0x32, 0xa);
        quake_call_00593A90(em, 0x4a, 2);
        quake_call_00593A90(em, 0x9e, 2);
        if (em_frame_check(em, 46.0f, 0)) {
            shell05_set(em, 0x38);
        }
        if (em_frame_check(em, 132.0f, 0)) {
            shell05_set(em, 0x39);
        }
        if (em_frame_check(em, 30.0f, 0)) {
            shell05_set4(em, 0x47, 0x1e);
        }
        if (em_frame_check(em, 82.0f, 0)) {
            shell05_set4(em, 0x47, 0x4e);
        }
        if (em_frame_check(em, 154.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 70.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x406:
        sound_call_005937E0(em, 0x52, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x44, 4, 0x17, 4);
        sound_call_005937E0(em, 0x44, 0, 0x17, 3);
        sound_call_005937E0(em, 0x2a, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0xa4, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x96, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x96, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x7c, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0xf8, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0xea, 5, 0xe, 4);
        sound_call_005937E0(em, 0xea, 1, 0xe, 3);
        sound_call_005937E0(em, 0xd0, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x14e, 8, 8, 0xa);
        sound_call_005937E0(em, 0x140, 5, 8, 4);
        sound_call_005937E0(em, 0x140, 1, 8, 3);
        sound_call_005937E0(em, 0x12a, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x38, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x11e, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x90, 0x16, 0x30, 0xa);
        quake_call_00593A90(em, 0x52, 2);
        quake_call_00593A90(em, 0xa6, 2);
        quake_call_00593A90(em, 0xfc, 2);
        quake_call_00593A90(em, 0x150, 2);
        if (em_frame_check(em, 48.0f, 0)) {
            shell05_set(em, 0x3a);
        }
        if (em_frame_check(em, 120.0f, 0)) {
            shell05_set(em, 0x3b);
        }
        if (em_frame_check(em, 306.0f, 0)) {
            shell05_set(em, 0x3c);
        }
        if (em_frame_check(em, 216.0f, 0)) {
            shell05_set(em, 0x3d);
        }
        if (em_frame_check(em, 50.0f, 0)) {
            shell05_set4(em, 0x47, 0x26);
        }
        if (em_frame_check(em, 120.0f, 0)) {
            shell05_set4(em, 0x47, 0x7a);
        }
        if (em_frame_check(em, 286.0f, 0)) {
            shell05_set4(em, 0x47, 0x40);
        }
        if (em_frame_check(em, 248.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 334.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 162.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 78.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x407:
        sound_call_005937E0(em, 0x4a, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x3c, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x3c, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x22, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0x9c, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x8e, 5, 0xe, 4);
        sound_call_005937E0(em, 0x8e, 1, 0xe, 3);
        sound_call_005937E0(em, 0x74, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0xcc, 8, 8, 0xa);
        sound_call_005937E0(em, 0xbe, 5, 8, 4);
        sound_call_005937E0(em, 0xbe, 1, 8, 3);
        sound_call_005937E0(em, 0xa4, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x116, 8, 0x17, 0xa);
        sound_call_005937E0(em, 0x108, 4, 0x17, 4);
        sound_call_005937E0(em, 0x108, 0, 0x17, 3);
        sound_call_005937E0(em, 0xee, 0xc, 0xb, 0xa);
        sound_call_005937E0(em, 0x166, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0x158, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x158, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x13e, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0x66, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x154, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x2c, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0xf4, 0x16, 0x30, 0xa);
        quake_call_00593A90(em, 0x50, 2);
        quake_call_00593A90(em, 0xa2, 2);
        quake_call_00593A90(em, 0xce, 2);
        quake_call_00593A90(em, 0x11c, 2);
        quake_call_00593A90(em, 0x16a, 2);
        if (em_frame_check(em, 208.0f, 0)) {
            shell05_set(em, 0x3e);
        }
        if (em_frame_check(em, 34.0f, 0)) {
            shell05_set(em, 0x3f);
        }
        if (em_frame_check(em, 162.0f, 0)) {
            shell05_set(em, 0x40);
        }
        if (em_frame_check(em, 96.0f, 0)) {
            shell05_set(em, 0x41);
        }
        if (em_frame_check(em, 368.0f, 0)) {
            shell05_set(em, 0x4e);
        }
        if (em_frame_check(em, 300.0f, 0)) {
            shell05_set(em, 0x4f);
        }
        if (em_frame_check(em, 18.0f, 0)) {
            shell05_set4(em, 0x47, 0x56);
        }
        if (em_frame_check(em, 280.0f, 0)) {
            shell05_set4(em, 0x47, 0x8c);
        }
        if (em_frame_check(em, 158.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 204.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 76.0f, 0) || em_frame_check(em, 358.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 280.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x424:
        sound_call_mov(em, 0xe4, 0x19a, 0x46, 0x25, 0xa);
        sound_call_005937E0(em, 0xb2, 0x18, 0x23, 0xa);
        sound_call_005937E0(em, 0xac, 0x19, 0x23, 4);
        sound_call_005937E0(em, 0xac, 8, 0x23, 0xa);
        sound_call_005937E0(em, 0x56, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x48, 5, 0xe, 4);
        sound_call_005937E0(em, 0x48, 1, 0xe, 3);
        sound_call_005937E0(em, 0x2e, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0xa4, 0xe, 0x13, 0xa);
        sound_call_005937E0(em, 0xa4, 0x10, 0x19, 0xa);
        sound_call_005937E0(em, 0x154, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x2c, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0xe4, 0xd, 0x21, 0xa);
        sound_call_005937E0(em, 0x104, 0x13, 3, 0xa);
        sound_call_005937E0(em, 0x48, 0xa, 0x1d, 0xa);
        quake_call_00593A90(em, 0x56, 2);
        quake_call_00593A90(em, 0x5a, 2);
        quake_call_00593A90(em, 0x90, 2);
        quake_call_00593A90(em, 0xaa, 5);
        quake_call_00593A90(em, 0xda, 2);
        if (em_frame_check(em, 2.0f, 0)) {
            shell05_set(em, 0x52);
        }
        if (em_frame_check(em, 58.0f, 0)) {
            shell05_set(em, 0x53);
        }
        if (em_frame_check(em, 110.0f, 0)) {
            shell05_set(em, 0x54);
        }
        if (em_frame_check(em, 150.0f, 0)) {
            shell05_set(em, 0x56);
        }
        if (em_frame_check(em, 130.0f, 0)) {
            Eft10_set(1.2f, em, 0, 3);
        }
        if (em_frame_check(em, 128.0f, 0)) {
            Eft10_set(1.4f, em, 0, 1);
        }
        if (em_frame_check(em, 146.0f, 0)) {
            Eft10_set(1.2f, em, 0, 4);
        }
        break;
    case 0x425:
        sound_call_mov(em, 4, 0xd2, 0x46, 0x25, 0xa);
        sound_call_mov(em, 0x104, 0x19a, 0x4b, 0x25, 4);
        sound_call_005937E0(em, 0x60, 9, 0xe, 0xa);
        sound_call_005937E0(em, 0x52, 5, 0xe, 4);
        sound_call_005937E0(em, 0x52, 1, 0xe, 3);
        sound_call_005937E0(em, 0x38, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0xa4, 9, 8, 0xa);
        sound_call_005937E0(em, 0x96, 5, 8, 4);
        sound_call_005937E0(em, 0x96, 1, 8, 3);
        sound_call_005937E0(em, 0x7c, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0xea, 9, 0x17, 0xa);
        sound_call_005937E0(em, 0xdc, 4, 0x17, 4);
        sound_call_005937E0(em, 0xdc, 0, 0x17, 3);
        sound_call_005937E0(em, 0xc2, 0x10, 0xb, 0xa);
        sound_call_005937E0(em, 0x13a, 9, 0x1d, 0xa);
        sound_call_005937E0(em, 0x12c, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x12c, 0, 0x1d, 3);
        sound_call_005937E0(em, 0x112, 0xc, 5, 0xa);
        sound_call_005937E0(em, 0x38, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x38, 0x16, 0x30, 0xa);
        quake_call_00593A90(em, 0x64, 2);
        quake_call_00593A90(em, 0xae, 2);
        quake_call_00593A90(em, 0xf0, 2);
        quake_call_00593A90(em, 0x146, 2);
        if (em_frame_check(em, 94.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 156.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 310.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 230.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x426:
        sound_call_mov(em, 4, 0xd2, 0x4c, 0x25, 4);
        sound_call_005937E0(em, 0xac, 0x17, 0x23, 0xa);
        sound_call_005937E0(em, 0x38, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x38, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 4, 0xe, 3, 0xa);
        sound_call_005937E0(em, 4, 0x13, 3, 0xa);
        break;
    case 0x427:
        sound_call_mov(em, 4, 0x11c, 0x4c, 0x25, 4);
        sound_call_005937E0(em, 0x2c, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x2c, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0x16, 9, 0xe, 0xa);
        sound_call_005937E0(em, 8, 5, 0xe, 4);
        sound_call_005937E0(em, 8, 1, 0xe, 3);
        sound_call_005937E0(em, 4, 0xe, 0x13, 0xa);
        sound_call_005937E0(em, 0x1c, 9, 0x1d, 0xa);
        sound_call_005937E0(em, 0xe, 4, 0x1d, 4);
        sound_call_005937E0(em, 0xe, 0, 0x1d, 3);
        sound_call_005937E0(em, 4, 0x10, 5, 0xa);
        sound_call_005937E0(em, 0x68, 9, 0x17, 0xa);
        sound_call_005937E0(em, 0x5a, 4, 0x17, 4);
        sound_call_005937E0(em, 0x5a, 0, 0x17, 3);
        sound_call_005937E0(em, 0x40, 0x10, 0xb, 0xa);
        sound_call_005937E0(em, 0xb4, 9, 0xe, 0xa);
        sound_call_005937E0(em, 0xa6, 5, 0xe, 4);
        sound_call_005937E0(em, 0xa6, 1, 0xe, 3);
        sound_call_005937E0(em, 0x8c, 0xe, 0x13, 0xa);
        sound_call_005937E0(em, 0xd6, 8, 0x1d, 0xa);
        sound_call_005937E0(em, 0xc8, 4, 0x1d, 4);
        sound_call_005937E0(em, 0xc8, 0, 0x1d, 3);
        sound_call_005937E0(em, 0xae, 0x10, 5, 0xa);
        quake_call_00593A90(em, 0x14, 2);
        quake_call_00593A90(em, 0x18, 2);
        quake_call_00593A90(em, 0x40, 2);
        quake_call_00593A90(em, 0x68, 2);
        quake_call_00593A90(em, 0xb4, 2);
        quake_call_00593A90(em, 0xd6, 2);
        if (em_frame_check(em, 186.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        if (em_frame_check(em, 58.0f, 0) || em_frame_check(em, 158.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.5f, 0x13);
        }
        if (em_frame_check(em, 32.0f, 0) || em_frame_check(em, 210.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        if (em_frame_check(em, 100.0f, 0)) {
            Eft13_set_em_scl(em, 0x17, 0.6f, 0x13);
        }
        break;
    case 0x428:
        sound_call_005937E0(em, 0x8a, 0x23, 0x25, 4);
        sound_call_005937E0(em, 0x34, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x34, 5, 0xe, 4);
        sound_call_005937E0(em, 0xfc, 1, 0xe, 3);
        sound_call_005937E0(em, 0x38, 8, 8, 0xa);
        sound_call_005937E0(em, 0x38, 5, 8, 4);
        sound_call_005937E0(em, 0x38, 1, 8, 3);
        break;
    case 0x429:
        sound_call_mov(em, 4, 0x18c, 0x49, 0x25, 4);
        sound_call_005937E0(em, 0x26, 9, 0x17, 0xa);
        sound_call_005937E0(em, 0x18, 4, 0x17, 4);
        sound_call_005937E0(em, 0x18, 0, 0x17, 3);
        sound_call_005937E0(em, 4, 0x10, 0xb, 0xa);
        sound_call_005937E0(em, 0xa8, 9, 0xe, 0xa);
        sound_call_005937E0(em, 0x9a, 5, 0xe, 4);
        sound_call_005937E0(em, 0x9a, 1, 0xe, 3);
        sound_call_005937E0(em, 0x80, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0xb4, 9, 8, 0xa);
        sound_call_005937E0(em, 0xa6, 5, 8, 4);
        sound_call_005937E0(em, 0xa6, 1, 8, 3);
        sound_call_005937E0(em, 0x8c, 0xe, 0x19, 0xa);
        sound_call_005937E0(em, 0x124, 9, 0x1d, 0xa);
        sound_call_005937E0(em, 0x116, 4, 0x1d, 4);
        sound_call_005937E0(em, 0x116, 0, 0x1d, 3);
        sound_call_005937E0(em, 0xfc, 0xc, 5, 0xa);
        quake_call_00593A90(em, 0x26, 2);
        quake_call_00593A90(em, 0xa4, 2);
        quake_call_00593A90(em, 0xbc, 2);
        quake_call_00593A90(em, 0x124, 2);
        if (em_frame_check(em, 162.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.8f, 0x13);
        }
        if (em_frame_check(em, 180.0f, 0)) {
            Eft13_set_em_scl(em, 8, 0.8f, 0x13);
        }
        if (em_frame_check(em, 288.0f, 0)) {
            Eft13_set_em_scl(em, 0x1d, 0.6f, 0x13);
        }
        break;
    case 0x42A:
        break;
    case 0x42B:
        sound_call_mov(em, 6, 0x274, 0x4d, 0x25, 4);
        sound_call_005937E0(em, 0x1a8, 0x19, 0x23, 0xa);
        sound_call_005937E0(em, 0x154, 0x1a, 0x13, 0xa);
        sound_call_005937E0(em, 0x190, 0x19, 0x13, 4);
        sound_call_005937E0(em, 0x190, 0x18, 0x13, 4);
        sound_call_005937E0(em, 0x19a, 0x18, 0x30, 4);
        sound_call_005937E0(em, 0xd8, 9, 0x17, 0xa);
        sound_call_005937E0(em, 0xca, 4, 0x17, 4);
        sound_call_005937E0(em, 0xca, 0, 0x17, 3);
        sound_call_005937E0(em, 0xae, 0x10, 0xb, 0xa);
        sound_call_005937E0(em, 0x144, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x140, 0x16, 0x30, 0xa);
        quake_call_00593A90(em, 0xd4, 2);
        quake_call_00593A90(em, 0x154, 2);
        quake_call_00593A90(em, 0x180, 2);
        quake_call_00593A90(em, 0x1a6, 2);
        quake_call_00593A90(em, 0x1d8, 2);
        if (em_frame_check(em, 338.0f, 0)) {
            Eft10_set(1.2f, em, 0, 0);
        }
        if (em_frame_check(em, 400.0f, 0)) {
            Eft10_set(1.4f, em, 0, 1);
        }
        if (em_frame_check(em, 406.0f, 0)) {
            Eft10_set(1.2f, em, 0, 2);
        }
        break;
    case 0x42C:
        sound_call_mov(em, 4, 0xd2, 0x4c, 0x25, 4);
        sound_call_005937E0(em, 0xac, 0x17, 0x23, 0xa);
        sound_call_005937E0(em, 0x38, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x38, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0x58, 8, 0xe, 0xa);
        sound_call_005937E0(em, 0x4a, 5, 0xe, 4);
        sound_call_005937E0(em, 0x4a, 1, 0xe, 3);
        sound_call_005937E0(em, 0x30, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0xf6, 9, 0xe, 0xa);
        sound_call_005937E0(em, 0xe8, 5, 0xe, 4);
        sound_call_005937E0(em, 0xe8, 1, 0xe, 3);
        sound_call_005937E0(em, 0xce, 0x13, 0x13, 0xa);
        sound_call_005937E0(em, 0x3c, 0x17, 0x32, 0xa);
        sound_call_005937E0(em, 0x3c, 0x16, 0x30, 0xa);
        sound_call_005937E0(em, 0x30, 0x19, 0xe, 0xa);
        quake_call_00593A90(em, 0x5a, 2);
        quake_call_00593A90(em, 0x6a, 2);
        quake_call_00593A90(em, 0xb4, 2);
        if (em_frame_check(em, 94.0f, 0)) {
            shell05_set(em, 0x57);
        }
        if (em_frame_check(em, 82.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.8f, 0x13);
        }
        if (em_frame_check(em, 236.0f, 0)) {
            Eft13_set_em_scl(em, 0xe, 0.5f, 0x13);
        }
        break;
    default:
        move_default_00593AE0(em);
        break;
    }
}
