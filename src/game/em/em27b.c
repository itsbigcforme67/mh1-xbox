/* em27 (part 2) - game.bin 0x00612DA0-0x00613958: sound_call, move_default and the per-animation sound/effect
 * script ef_move_sub. em27_uvmove and em27_effect_move are near-matches in em27_nm.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"

/* Per-monster work at EMW+0x444 (em27.c has the same start). */
typedef struct EM27W {
    u8 eff;             /* 0x00 em27_effect_move step */
    u8 _pad01[3];
    s16 anim;           /* 0x04 animation the sound/effect script follows */
    u16 tgt_ang;        /* 0x06 angle toward the target (mv00) */
    s32 spd[3];         /* 0x08 passed to speed_add_g (angle in [1]) */
    u8 adj_x;           /* 0x14 fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x15 */
    u8 adj_z;           /* 0x16 */
    u8 adj_type;        /* 0x17 table row */
    s16 adj_tm;         /* 0x18 time into the table */
    u8 yobi_st;         /* 0x1A yobi state */
    u8 has_tgt;         /* 0x1B */
    f32 dist;           /* 0x1C distance to the target (1000 with none) */
    f32 yobi[3];        /* 0x20 yobi position */
    f32 yobi_r;         /* 0x2C yobi range */
    u8 _pad30[4];
    u8 yobi_stg;        /* 0x34 stage */
    u8 _pad35[3];
    s32 x38;            /* 0x38 */
    s32 x3C;            /* 0x3C */
    f32 x40;            /* 0x40 saved 0x3BC */
    s32 x44;            /* 0x44 */
    s32 x48;            /* 0x48 */
    s32 x4C;            /* 0x4C */
} EM27W;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;
extern s16 em27_stay_timer_tbl[];
extern s16 em27_runaway_timer_tbl[];
extern u8 em27_act_tbl[8];

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set2(EMW *, s16, s16);
void em_dur_init(EMW *);
u16 em_act_search(void *);
void em27_act_set(EMW *em, int kind, u16 no, u16 arg);
void em27_to_normal(EMW *em, s16 a, s16 b);
u16 Em_Calc_angY(f32 *, f32 *);
int em_frame_check(EMW *, f32, int);
int em_frame_check2(EMW *, int, f32);
void SetVector(f32 *, f32, f32, f32);
void pull_em_yobi(f32 *);
void push_em_yobi(f32 *);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
u8 em27_fly_adjy2(EMW *);
void em27_fly_adjy2_init(EMW *, u8);
void shell15_set(EMW *, int);
int em_mode_timer_sub(EMW *);
void em_no_floor_ck2(EMW *);
void em_no_battle_area_ck(EMW *, int, int);
int Em_Yobi_Ck(EMW *, f32 *);
int em_cancel_act_ck(EMW *, u8);
void em_hinshi_ck(EMW *, f32);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void em_cmd_ck(EMW *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);
int Code_Make(int, int, int, int);
void Eft13_set_em(EMW *, int, int);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void em27_init(EMW *);
int Event_flag_ck(int);
extern f32 st01_pos_tbl[6];
extern f32 st34_pos_tbl[6];
extern f32 st38_pos_tbl_00671470[][6];
extern f32 st39_pos_tbl_006714A0[][6];
extern f32 st53_pos_tbl_00671500[6];
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void em_cmd_reset(EMW *);
void Em_Mahi_Start(EMW *);
void em_mahi_eff_set(EMW *, int);
f32 CalcDistanceXZ(f32 *, f32 *);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);

static void sound_call_00612DA0(EMW *em, int frame, int se, int joint) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, 0)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, 5, 0);
    }
}

static void move_default_00612E40(EMW *em) {
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

/* Sound and effect script per animation (sound_call(em, frame, se, joint) plays a sound at the
 * joint once the animation reaches the frame). */
void ef_move_sub_00612E90(EMW *em, EM27W *w) {
    f32 v[3];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_00612DA0(em, 200, Code_Make(14, 2, 14, 2), 0);
        break;
    case 0x3EA:
        sound_call_00612DA0(em, 8, 2, 0);
        sound_call_00612DA0(em, 32, 2, 0);
        break;
    case 0x3EB:
        sound_call_00612DA0(em, 20, 8, 0);
        sound_call_00612DA0(em, 16, 2, 0);
        sound_call_00612DA0(em, 20, 3, 0);
        if (em_frame_check(em, 18.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3EC:
        sound_call_00612DA0(em, 22, 8, 0);
        sound_call_00612DA0(em, 22, 2, 0);
        sound_call_00612DA0(em, 22, 3, 0);
        if (em_frame_check(em, 20.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3ED:
        sound_call_00612DA0(em, 4, 2, 0);
        sound_call_00612DA0(em, 10, 1, 0);
        break;
    case 0x3EE:
    case 0x3EF:
        sound_call_00612DA0(em, 6, 5, 0);
        sound_call_00612DA0(em, 26, 2, 0);
        sound_call_00612DA0(em, 26, 4, 0);
        sound_call_00612DA0(em, 42, 2, 0);
        sound_call_00612DA0(em, 48, 1, 0);
        break;
    case 0x3F0:
        sound_call_00612DA0(em, 8, 2, 0);
        sound_call_00612DA0(em, 8, 4, 0);
        sound_call_00612DA0(em, 26, 2, 0);
        sound_call_00612DA0(em, 32, 1, 0);
        sound_call_00612DA0(em, 64, 7, 0);
        if (em_frame_check(em, 12.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3F1:
    case 0x3F2:
        sound_call_00612DA0(em, 12, 2, 0);
        sound_call_00612DA0(em, 12, 4, 0);
        sound_call_00612DA0(em, 30, 2, 0);
        sound_call_00612DA0(em, 36, 1, 0);
        sound_call_00612DA0(em, 64, 7, 0);
        if (em_frame_check(em, 14.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3F3:
        sound_call_00612DA0(em, 68, 17, 0);
        sound_call_00612DA0(em, 164, 18, 0);
        sound_call_00612DA0(em, 34, 23, 0);
        sound_call_00612DA0(em, 260, 23, 0);
        sound_call_00612DA0(em, 288, 23, 0);
        break;
    case 0x3F4:
        sound_call_00612DA0(em, 118, 14, 0);
        break;
    case 0x3FC:
        sound_call_00612DA0(em, 22, 2, 0);
        sound_call_00612DA0(em, 22, 3, 0);
        sound_call_00612DA0(em, 22, 8, 0);
        break;
    case 0x3FD:
        sound_call_00612DA0(em, 4, 2, 0);
        sound_call_00612DA0(em, 6, 1, 0);
        sound_call_00612DA0(em, 36, 0, 0);
        break;
    case 0x3FE:
        sound_call_00612DA0(em, 40, 2, 0);
        sound_call_00612DA0(em, 60, 2, 0);
        sound_call_00612DA0(em, 118, 2, 0);
        sound_call_00612DA0(em, 26, 16, 0);
        break;
    case 0x3FF:
        sound_call_00612DA0(em, 22, 2, 0);
        sound_call_00612DA0(em, 22, 3, 0);
        sound_call_00612DA0(em, 22, 8, 0);
        if (em_frame_check(em, 32.0f, 0)) {
            shell15_set(em, 2);
        }
        if (em_frame_check(em, 22.0f, 0)) {
            Eft13_set_em(em, 0x1B, 0);
        }
        break;
    case 0x400:
        sound_call_00612DA0(em, 4, 2, 0);
        sound_call_00612DA0(em, 6, 1, 0);
        break;
    case 0x401:
        sound_call_00612DA0(em, 2, 16, 0);
        break;
    case 0x402:
        sound_call_00612DA0(em, 60, 0, 0);
        sound_call_00612DA0(em, 128, 2, 0);
        sound_call_00612DA0(em, 72, 28, 0);
        sound_call_00612DA0(em, 60, 8, 0);
        break;
    case 0x406:
        sound_call_00612DA0(em, 4, 9, 0);
        sound_call_00612DA0(em, 30, 2, 0);
        sound_call_00612DA0(em, 28, 1, 0);
        sound_call_00612DA0(em, 54, 8, 0);
        break;
    case 0x407:
    case 0x409:
    case 0x40B:
    case 0x40D:
        sound_call_00612DA0(em, 4, 11, 0);
        break;
    case 0x408:
    case 0x40A:
    case 0x40C:
    case 0x40E:
        sound_call_00612DA0(em, 4, 15, 0);
        break;
    case 0x40F:
        sound_call_00612DA0(em, 24, 3, 0);
        sound_call_00612DA0(em, 44, 1, 0);
        sound_call_00612DA0(em, 54, 2, 0);
        sound_call_00612DA0(em, 88, 0, 0);
        break;
    case 0x412:
        sound_call_00612DA0(em, 180, 9, 0);
        sound_call_00612DA0(em, 132, 11, 0);
        sound_call_00612DA0(em, 52, 10, 0);
        sound_call_00612DA0(em, 68, 1, 0);
        sound_call_00612DA0(em, 86, 0, 0);
        sound_call_00612DA0(em, 132, 1, 0);
        sound_call_00612DA0(em, 140, 0, 0);
        break;
    case 0x413:
        sound_call_00612DA0(em, 4, 17, 0);
        sound_call_00612DA0(em, 66, 17, 0);
        sound_call_00612DA0(em, 130, 18, 0);
        sound_call_00612DA0(em, 48, 23, 0);
        sound_call_00612DA0(em, 122, 0, 0);
        sound_call_00612DA0(em, 162, 15, 0);
        break;
    case 0x414:
        sound_call_00612DA0(em, 4, 14, 0);
        v[1] = 10.0f;
        v[2] = 40.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 15, v, 1.0f);
        break;
    case 0x416:
        sound_call_00612DA0(em, 10, 13, 0);
        break;
    case 0x417:
        sound_call_00612DA0(em, 14, 25, 0);
        break;
    case 0x41A:
        sound_call_00612DA0(em, 32, 21, 0);
        sound_call_00612DA0(em, 68, 21, 0);
        sound_call_00612DA0(em, 98, 22, 0);
        sound_call_00612DA0(em, 20, 0, 0);
        sound_call_00612DA0(em, 160, 23, 0);
        break;
    case 0x41B:
        sound_call_00612DA0(em, 10, 20, 0);
        sound_call_00612DA0(em, 64, 20, 0);
        sound_call_00612DA0(em, 6, 1, 0);
        sound_call_00612DA0(em, 122, 1, 0);
        sound_call_00612DA0(em, 134, 0, 0);
        break;
    case 0x41C:
        sound_call_00612DA0(em, 8, 21, 0);
        sound_call_00612DA0(em, 98, 22, 0);
        sound_call_00612DA0(em, 22, 1, 0);
        sound_call_00612DA0(em, 58, 0, 0);
        sound_call_00612DA0(em, 106, 1, 0);
        sound_call_00612DA0(em, 158, 0, 0);
        break;
    case 0x41D:
        sound_call_00612DA0(em, 84, 24, 0);
        sound_call_00612DA0(em, 126, 24, 0);
        sound_call_00612DA0(em, 216, 26, 0);
        sound_call_00612DA0(em, 258, 26, 0);
        sound_call_00612DA0(em, 292, 27, 0);
        sound_call_00612DA0(em, 156, 11, 0);
        sound_call_00612DA0(em, 14, 0, 0);
        sound_call_00612DA0(em, 46, 2, 0);
        break;
    default:
        move_default_00612E40(em);
        break;
    }
}
