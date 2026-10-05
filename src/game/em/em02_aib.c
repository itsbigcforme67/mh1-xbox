/* em02_ai, run 2: em_fly00_005803B0 .. em02_main_sub (game.bin 0x005803B0-0x00583928). Matching functions of em02_ai_nm.c (that file holds the
 * whole code including the near-matches); see it for the description. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"

/* Raw access to EMW fields that em.h does not name yet. */
#define EMF(em, T, o) (*(T *)((u8 *)(em) + (o)))

/* em02's part of the per-monster work at EMW+0x444 (em02.c has the same start). */
typedef struct EM02W {
    u8 _pad00;
    u8 eff;             /* 0x01 effect script step (em02_effect_move) */
    s16 anim;           /* 0x02 animation the sound/effect script follows */
    u8 _pad04;
    u8 x05;             /* 0x05 */
    s16 x06;            /* 0x06 */
    u8 _pad08[8];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    s8 x18;             /* 0x18 1 while flying */
    u8 _pad19;
    s8 x1A;             /* 0x1A attack repeat counter */
    u8 _pad1B;
    s32 turn_left;      /* 0x1C */
    s32 bank_max;       /* 0x20 */
    f32 tp[3];          /* 0x24 saved position */
    s32 spd[3];         /* 0x30 passed to speed_add(_g) (angle in [1]) */
    s32 pitch_spd;      /* 0x3C ang[0] step */
    s32 turn;           /* 0x40 maximum turn per call */
    s32 bank_spd;       /* 0x44 ang[2] step */
    u8 _pad48[4];
    u8 adj_x;           /* 0x4C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x4D */
    u8 adj_z;           /* 0x4E */
    u8 adj_type;        /* 0x4F table row */
    s16 adj_tm;         /* 0x50 time into the table */
    u8 _pad52[2];
    s8 hagi[3];         /* 0x54 hagi pick point numbers, -1 none */
} EM02W;

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

extern s8 hagi_tbl_00388508[3][2];
extern f32 hagi_r_tbl_006544B0[3];
void em02_act_set(EMW *em, int kind, u16 no, u16 arg);
void em02_fly_adjy(EMW *, int);
u8 em02_fly_adjy2(EMW *);
void em02_fly_adjy2_init(EMW *, u8);
u16 em02_senkai_target(EMW *);
void em02_main_sub(EMW *em, EM02W *w);
void em02_to_normal(EMW *em);
void em02_senkai_player(EMW *);
void shell04_set(EMW *, int);
void Eft17_set(EMW *, int, int, int);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void mot_miration_ret(EMW *, f32 *);
void xang_calc_target(EMW *, int *, f32, f32);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
int em_frame_check2(EMW *, int, f32);
int em_frame_check(EMW *, f32, int);
void em02_to_fly(EMW *em, int mode);
void em_no_battle_area_ck(EMW *, int, int);
void F_DragonEscapeCamera(EMW *);
int Quest_clear_ck(int);
void get_joint_pos_em(EMW *, int, f32 *);
int Ext_pick_point_set(STIEM *, f32 *);
void Ext_pick_point_pos(int, f32 *);
int Ext_pick_point_cnt_ck(int);
void Ext_pick_point_clr(int);
void quake_call_00587230(EMW *em, int frame, int v);
void sound_call_sub_00587280(EMW *em, int se, int joint);
void sound_call_005872F0(EMW *em, int frame, int se, int joint);
void move_default_00583BA0(EMW *em);
void ef_move_sub_00583BF0(EMW *em, EM02W *w);
void em02_uvmove(EMW *em);
void em_uvset(EMW *em, u32 frame, u16 idx, s8 type);
void Em_set_quake_sub(EMW *, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);

#define FLY_FLOOR(em)                    \
    if ((em)->pos[1] < (em)->x5AC) {     \
        (em)->pos[1] = (em)->x5AC;       \
    }

/* Turn toward the target by at most 0x40 per frame (same as em01/em03). */
#define EM02_TURN(em)                                                     \
    do {                                                                  \
        int d;                                                            \
        d = (u16)((u16)Em_Calc_angY((em)->pos, (em)->tgt_pos) - (em)->ang[1]); \
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

#define ATK_SIMPLE(NAME, CH)                     \
    static void NAME(EMW *em, EM02W *w) {        \
        switch (em->x05) {                       \
        case 0:                                  \
            em->x05++;                           \
            em->x388 = 0;                        \
            em->x3F4 = 0;                        \
            em_char_set(em, CH, 0, 0);           \
            break;                               \
        case 1:                                  \
            if (em->x194 == 0) {                 \
                em->x05++;                       \
                em02_to_normal(em);              \
            }                                    \
            break;                               \
        }                                        \
    }

#define DMG_SIMPLE(NAME, CH)                     \
    static void NAME(EMW *em, EM02W *w) {        \
        switch (em->x05) {                       \
        case 0:                                  \
            em->x05++;                           \
            em->x388 = 0;                        \
            em->x3F4 = 0;                        \
            em_cmd_reset(em);                    \
            em_char_set(em, CH, 0, 0);           \
            break;                               \
        case 1:                                  \
            if (em->x194 == 0) {                 \
                em->x05++;                       \
                em02_to_normal(em);              \
            }                                    \
            break;                               \
        }                                        \
    }

#define UV_RESET(i) \
    do { \
        uv[i][0] = 0.0f; \
        uv[i][1] = 0.0f; \
        tm[i] = 0xFFFF; \
        ty[i] = 0xFF; \
    } while (0)

void em_fly00_005803B0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 9, 0, 0);
        em02_fly_adjy2_init(em, 0);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check(em, 134.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 2:
        if (em02_fly_adjy2(em)) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    }
}

void em_fly01_005804B0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em->ang[0] = 0;
        em->ang[2] = 0;
        em_char_set(em, 0xB, 0, 0);
        em_rate_clear(em);
        em->adj_y = -10.0f;
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (!(700.0f + em->x5AC <= em->pos[1])) {
            em->x05++;
            em->adj_y = -20.0f;
        }
        break;
    case 2:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em_char_set(em, 0xC, 0, 0);
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->x388 = 0;
            Em_set_quake_sub(em, 1);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_fly02_00580680(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        if (em->char0 != 0x3F2) {
            em_char_set2(em, 0x3F2, 0, 0, 0);
        }
        if (em->x2DE != 0x4BA) {
            em_char_set2(em, 0x4BA, 0, 0, 1);
        }
        if (em->x2E0 != 0x582) {
            em_char_set2(em, 0x582, 0, 0, 2);
        }
        w->x18 = 0;
        SetVector(w->tp, em->pos[0], em->pos[1], em->pos[2]);
        break;
    case 1:
        em02_fly_adjy(em, 1);
        if (!(em->pos[1] < w->tp[1])) {
            em->pos[1] = w->tp[1];
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        if (!(em->pos[1] < w->tp[1])) {
            em->pos[1] = w->tp[1];
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_fly03_00580800(EMW *em, EM02W *w) {
    int t;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        em->work08 = 0x12C;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        t = em02_senkai_target(em) & 0xFF;
        if (--em->work08 <= 0 || t != 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

void em_fly04_00580900(EMW *em, EM02W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        em02_senkai_target(em);
        w->spd[1] = em->ang[1];
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 100.0f * em->adj_z) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

void em_fly05_00580A50(EMW *em, EM02W *w) {
    f32 v[4]; /* never filled: the original passes this uninitialized local (Capcom bug) */

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 10.0f;
        break;
    case 1:
        w->spd[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
        speed_add(em, w->spd);
        if (--em->work08 <= 0 || !(em->pos[1] <= 1500.0f + em->tgt_pos[1])) {
            em->x05++;
            em_rate_clear(em);
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_fly06_00580B80(EMW *em, EM02W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        w->spd[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 100.0f * em->adj_z) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

void em_fly07_00580CD0(EMW *em, EM02W *w) {
    f32 v[4]; /* never filled (see fly05) */

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = -10.0f;
        break;
    case 1:
        w->spd[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
        speed_add(em, w->spd);
        if (--em->work08 <= 0 || em->pos[1] <= 500.0f + em->x5AC) {
            em->x05++;
            em_rate_clear(em);
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_fly08_00580E00(EMW *em, EM02W *w) {
    f32 v[4]; /* never filled (see fly05) */

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        em_rate_clear(em);
        em->adj_y = 10.0f;
        break;
    case 1:
        w->spd[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
        speed_add(em, w->spd);
        if (!(em->pos[1] < 6000.0f)) {
            em->x05++;
            em_rate_clear(em);
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_fly09_00580F10(EMW *em, EM02W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        em02_senkai_target(em);
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 100.0f * em->adj_z) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

void em_fly10_00581040(EMW *em, EM02W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        em02_senkai_target(em);
        w->spd[1] = em->ang[1];
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (em->x74C & 0xF000000F) {
            em->pos[1] += 20.0f;
        }
        if (--em->work08 <= 0 || d <= 100.0f * em->adj_z) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

ATK_SIMPLE(em_atk00_005811C0, 0x12)

ATK_SIMPLE(em_atk01_00581240, 0x13)

void em_atk02_005812C0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0xD, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 102.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_atk03_005813A0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0xE, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 98.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

ATK_SIMPLE(em_atk04_00581480, 0xF)

/* Shared by actions 5, 10 and 11: mode 0/1/2 picks the shell angle (move03 passes it as a third argument). */
void em_atk05_00581500(EMW *em, EM02W *w, int mode) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x1F, 0, 0);
        em->work08 = 0x5A;
        break;
    case 1:
        em02_senkai_player(em);
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
        }
        break;
    case 2:
        em02_senkai_player(em);
        if (em_frame_check(em, 30.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            switch ((u8)mode) {
            case 0:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x38E4, 0);
                break;
            case 1:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x2AAB, 0);
                break;
            case 2:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x1E94, 0);
                break;
            }
            w->x1A = w->x1A - 1;
        }
        if (w->x1A <= 0 && em_frame_check(em, 76.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xA, 0xA, 0x1E);
        }
        if (em->x194 == 0) {
            em_char_set(em, 0x10, 4, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_atk06_00581770(EMW *em, EM02W *w) {
    f32 v[4];

    em->x8BB = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 5, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 7, 0, 0);
            shell04_set(em, 2);
        }
        break;
    case 2:
        if (w->has_tgt != 0) {
            EM02_TURN(em);
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_atk07_00581930(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x21, 0, 0);
        em02_fly_adjy2_init(em, 3);
        break;
    case 1:
        if (em_frame_check(em, 70.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 2:
        if (em_frame_check(em, 94.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0x38E4, 0);
        }
        em02_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_atk08_00581A90(EMW *em, EM02W *w) {
    em->x8BB = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 5, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_atk09_00581B50(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x1F, 0, 0);
        em->work08 = 0x5A;
        break;
    case 1:
        em02_senkai_player(em);
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
        }
        break;
    case 2:
        em02_senkai_player(em);
        if (em_frame_check(em, 30.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            w->x1A = w->x1A - 1;
            switch (w->x1A) {
            case 2:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x38E4, 0);
                break;
            case 1:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x2AAB, 0);
                break;
            case 0:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x1E94, 0);
                break;
            }
        }
        if (w->x1A <= 0 && em_frame_check(em, 76.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xA, 0xA, 0x1E);
        }
        if (em->x194 == 0) {
            em_char_set(em, 0x10, 4, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

DMG_SIMPLE(em_dmg00_00581DC0, 0x14)

DMG_SIMPLE(em_dmg01_00581E50, 0x15)

void em_dmg02_00581EE0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em_frame_check2(em, 0, 50.0f) == 0) {
            em->ang[1] += 0x28E;
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_dmg03_00581FA0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        em_cmd_reset(em);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (180.0f + em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x1A, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_dmg04_00582100(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1B, 0, 0);
        em->work08 = 0x78;
        em_cmd_reset(em);
        game_w.flag1B3 |= 2;
        em->x762 = 3;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x1C, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->x762 = 0;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_dmg05_00582220(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x22, 0, 0);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check(em, 296.0f, 0)) {
            em->x05++;
            em_char_set(em, 0x1B, 0xC, 0x124);
        }
        break;
    case 2:
        em_mahi_eff_set(em, 2);
        if (--em->work08 <= 0) {
            em->x05++;
            em02_act_set(em, 0, 6, 4);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em02_act_set(em, 0, 6, 4);
        }
        break;
    }
}

void em_dmg06_00582350(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x17, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 50.0f) == 0) {
            em->ang[1] -= 0x28E;
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_dmg07_00582410(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x14, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_dmg08_005824A0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        em_cmd_reset(em);
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (180.0f + em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x1A, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_rate_clear(em);
            em02_to_normal(em);
        }
        break;
    }
}

void em_die00_00582600(EMW *em, EM02W *w) {
    em->x40E = 5;
    em->x888 = 0;
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x18, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05++;
            em02_hagi_set(em);
            em->ex[0x90] = 0;
        }
        break;
    case 2:
        em02_hagi_move(em);
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
            em02_hagi_clr(em);
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em02_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_die01_00582760(EMW *em, EM02W *w) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1D, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        game_w.flag1B3 |= 2;
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05++;
            em02_hagi_set(em);
            em->ex[0x90] = 0;
        }
        break;
    case 2:
        em02_hagi_move(em);
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
            em02_hagi_clr(em);
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em02_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_die02_005828E0(EMW *em, EM02W *w) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x1A, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em_frame_check(em, 134.0f, 0)) {
            em->x05++;
            em->act_spd = 0.0f;
            em->work08 = 0;
            em->x388 = 3;
            Quest_enemy_die(em);
            em02_hagi_set(em);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        em->act_spd = 0.0f;
        em02_hagi_move(em);
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05++;
        }
        break;
    case 4:
        em02_hagi_move(em);
        break;
    case 5:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
            em02_hagi_clr(em);
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em02_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_move00_00582B40(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_act00_0057FA50(em, w); break;
    case 1: em_act01_0057FB30(em, w); break;
    case 2: em_act02_0057FBC0(em, w); break;
    case 3: em_act03_0057FC50(em, w); break;
    case 4: em_act04_0057FCD0(em, w); break;
    case 5: em_act05_0057FDF0(em, w); break;
    case 6: em_act06_0057FEC0(em, w); break;
    }
}

void em_move01_00582BF0(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_mv00_0057FF80(em, w); break;
    case 1: em_mv01_005800C0(em, w); break;
    }
}

void em_move02_00582C40(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_fly00_005803B0(em, w); break;
    case 1: em_fly01_005804B0(em, w); break;
    case 2: em_fly02_00580680(em, w); break;
    case 3: em_fly03_00580800(em, w); break;
    case 4: em_fly04_00580900(em, w); break;
    case 5: em_fly05_00580A50(em, w); break;
    case 6: em_fly06_00580B80(em, w); break;
    case 7: em_fly07_00580CD0(em, w); break;
    case 8: em_fly08_00580E00(em, w); break;
    case 9: em_fly09_00580F10(em, w); break;
    case 10: em_fly10_00581040(em, w); break;
    }
}

void em_move03_00582D30(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_atk00_005811C0(em, w); break;
    case 1: em_atk01_00581240(em, w); break;
    case 2: em_atk02_005812C0(em, w); break;
    case 3: em_atk03_005813A0(em, w); break;
    case 4: em_atk04_00581480(em, w); break;
    case 5: em_atk05_00581500(em, w, 0); break;
    case 6: em_atk06_00581770(em, w); break;
    case 7: em_atk07_00581930(em, w); break;
    case 8: em_atk08_00581A90(em, w); break;
    case 9: em_atk09_00581B50(em, w); break;
    case 10: em_atk05_00581500(em, w, 1); break;
    case 11: em_atk05_00581500(em, w, 2); break;
    }
}

void em_move04_00582E30(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_dmg00_00581DC0(em, w); break;
    case 1: em_dmg01_00581E50(em, w); break;
    case 2: em_dmg02_00581EE0(em, w); break;
    case 3: em_dmg03_00581FA0(em, w); break;
    case 4: em_dmg04_00582100(em, w); break;
    case 5: em_dmg05_00582220(em, w); break;
    case 6: em_dmg06_00582350(em, w); break;
    case 7: em_dmg07_00582410(em, w); break;
    case 8: em_dmg08_005824A0(em, w); break;
    }
}

void em_move05_00582F00(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_die00_00582600(em, w); break;
    case 1: em_die01_00582760(em, w); break;
    case 2: em_die02_005828E0(em, w); break;
    }
}

void em_demo00_00582F70(EMW *em, EM02W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x14, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x20, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 9, 0, 0);
            em02_fly_adjy2_init(em, 0);
        }
        break;
    case 3:
        if (em_frame_check(em, 134.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 4:
        if (em02_fly_adjy2(em)) {
            em->x05++;
            em_char_set(em, 0xA, 0, 0);
            w->x18 = 0;
        }
        break;
    case 5:
        em02_fly_adjy(em, 1);
        em->pos[1] = em->pos[1] + 10.0f;
        if (!(em->pos[1] < 7500.0f)) {
            em->x05++;
        }
        break;
    default:
        break;
    }
}

void em_demo01_00583120(EMW *em, EM02W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->pos[0] = 17300.0f;
        em->pos[1] = 0.0f;
        em->pos[2] = 31370.0f;
        em->tgt_pos[0] = 11980.0f;
        em->tgt_pos[1] = 5860.0f;
        em->tgt_pos[2] = 15200.0f;
        em->ang[0] = 0;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->ang[2] = 0;
        em->tgt_pos[0] = 17300.0f;
        em->tgt_pos[1] = 5860.0f;
        em->tgt_pos[2] = 31370.0f;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0x96;
        em_rate_clear(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 9, 0, 0);
            em02_fly_adjy2_init(em, 0);
            w->x18 = 0;
        }
        break;
    case 2:
        if (em_frame_check(em, 134.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 3:
        if (em02_fly_adjy2(em)) {
            em->x05++;
            em_char_set(em, 0xA, 0, 0);
            w->spd[0] = 0;
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
        }
        break;
    case 4:
        em02_fly_adjy(em, 1);
        em->pos[1] = em->pos[1] + 20.0f;
        if (!(em->pos[1] < em->tgt_pos[1])) {
            em->x05++;
            w->turn = 0x100;
            em->tgt_pos[0] = 11980.0f;
            em->tgt_pos[1] = 5860.0f;
            em->tgt_pos[2] = 15200.0f;
            em_rate_clear(em);
            em->adj_z = 50.0f;
        }
        break;
    case 5:
        em02_senkai_target(em);
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 10.0f * em->adj_z) {
            em->x05++;
            em_rate_clear(em);
            em->adj_y = -10.0f;
        }
        break;
    case 6:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (!(700.0f + em->x5AC <= em->pos[1])) {
            em->x05++;
            em->adj_y = -20.0f;
        }
        break;
    case 7:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em_char_set(em, 0xC, 0, 0);
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->x388 = 0;
            Em_set_quake_sub(em, 1);
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 9:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x20, 0, 0);
        }
        break;
    case 10:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0xD, 0, 0);
        }
        break;
    case 11:
        if (em_frame_check(em, 102.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 12:
        if (Event_flag_ck(0x1B) == 1) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    default:
        break;
    }
}

void em_demo02_00583610(EMW *em, EM02W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x1F, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check(em, 30.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0x38E4, 0);
        }
        if (em_frame_check(em, 76.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xA, 0xA, 0x1E);
        }
        break;
    case 3:
        em02_fly_adjy(em, 1);
        em->pos[1] = em->pos[1] + 10.0f;
        if (!(em->pos[1] < 7500.0f)) {
            em->x05++;
        }
        break;
    }
}

void em_move06_005837B0(EMW *em, EM02W *w) {
    em->act_spd = 1.0f;
    switch (em->x15) {
    case 0: em_demo00_00582F70(em, w); break;
    case 1: em_demo01_00583120(em, w); break;
    case 2: em_demo02_00583610(em, w); break;
    }
}

void em02_main_sub(EMW *em, EM02W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->mode) {
    case 0: em_move00_00582B40(em, w); break;
    case 1: em_move01_00582BF0(em, w); break;
    case 2: em_move02_00582C40(em, w); break;
    case 3: em_move03_00582D30(em, w); break;
    case 4: em_move04_00582E30(em, w); break;
    case 5: em_move05_00582F00(em, w); break;
    case 6: em_move06_005837B0(em, w); break;
    case 7: em_move06_005837B0(em, w); break;
    }
    if (em->pos[0] <= 0.0f || em->pos[2] <= 0.0f) {
        em_dur_set(em, 0);
    }
}
