/* em01 AI, run 4: em_mv06_00569D60 .. em_fly08_0056AD20 (game.bin 0x00569D60-0x0056AFB4). Matching functions of em01_ai_nm.c (that file holds the
 * whole AI including the near-matches). See em01_ai_nm.c for the description. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"

/* Raw access to EMW fields that em.h does not name yet. */
#define EMF(em, T, o) (*(T *)((u8 *)(em) + (o)))
/* x3AC holds a float distance in atk 11 (em.h calls it s32). */
#define EM_F3AC(em) (*(f32 *)&(em)->x3AC)

/* em01's part of the per-monster work at EMW+0x444 (em01.c has the same start). */
typedef struct EM01W {
    u8 eff;             /* 0x00 effect script step (em01_effect_move) */
    u8 _pad01;
    s16 anim;           /* 0x02 animation the sound/effect script follows */
    u8 _pad04;
    u8 x05;             /* 0x05 fly mode (4 circling, 1 fly 19) */
    s16 x06;            /* 0x06 (150 set after a landing) */
    u8 x08;             /* 0x08 cleared by every em01_act_set */
    u8 _pad09[7];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    s8 x18;             /* 0x18 1 while flying (set by fly 6 and 8) */
    u8 x19;             /* 0x19 row counter of em_act_search2 */
    s8 x1A;             /* 0x1A attack repeat counter */
    u8 _pad1B;
    s32 turn_left;      /* 0x1C */
    s32 bank_max;       /* 0x20 bank limit for senkai_sub */
    f32 tp[3];          /* 0x24 saved target position (atk 11 dive) */
    s32 spd[3];         /* 0x30 passed to speed_add(_g) (angle in [1]) */
    s32 pitch_spd;      /* 0x3C ang[0] step */
    s32 turn;           /* 0x40 maximum turn per call */
    s32 bank_spd;       /* 0x44 ang[2] step */
    u8 x48;             /* 0x48 */
    u8 x49;             /* 0x49 */
    u8 x4A;             /* 0x4A */
    u8 _pad4B;
    u8 adj_x;           /* 0x4C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x4D */
    u8 adj_z;           /* 0x4E */
    u8 adj_type;        /* 0x4F table row */
    s16 adj_tm;         /* 0x50 time into the table */
    s16 x52;            /* 0x52 cooldown frames (atk 8) */
} EM01W;

/* The effect flag at ex+0xA3 (EMW+0x4E7) is read as xA3 in em01_ai. */
typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;

extern s16 em01_stay_timer_tbl[];
extern s16 em01_runaway_timer_tbl[];
extern void *em01_act_add[3];
extern void *em01_rail_add[2];
extern void *em01_rail_half_add[2];

void Eft19_set(EMW *, int, int);
void eft09_set(EMW *, int);
void Eft20_set(f32, EMW *, int, int);
void shell01_set(EMW *, int);
s16 em_hp_vital_set2(EMW *, s16, s16);
int em_act_search(void *);
void em_char_set(EMW *, int, int, int);
void em_char_set2();
void em_act_set(EMW *, int, u16);
void em01_act_set(EMW *em, int kind, u16 no, u16 arg);
int em_frame_check(EMW *, f32, int);
int em_frame_check2(EMW *, int, f32);
u16 Em_Calc_angY(f32 *, f32 *);
void sound_call_sub_00574CD0(EMW *em, int se, int joint);
void sound_call_00574D40(EMW *em, int frame, int se, int joint);
void sound_call_parts_00574DA0(EMW *em, int frame, int se, int joint, u8 mode);
void quake_call_00574E40(EMW *em, int frame, int v);
void move_default_00574E90(EMW *em);
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
void em01_fly_adjy(EMW *, int);
u8 em01_fly_adjy2(EMW *);
void em01_fly_adjy2_init(EMW *, u8);
void em01_senkai_sub(EMW *, int, int);
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
void ground_land_eff_set_0057A7E0(EMW *em);
void get_joint_pos_em(EMW *, int, f32 *);
void eft11_set(EMW *, f32 *, int);
int em_pl_pos_set(EMW *, u8, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void takeoff_eff_set_0057A890(EMW *em);
void takeon_eff_set_0057A900(EMW *em);
void hover_eff_set2_0057A9A0(EMW *em);
int kyusyu_char_set_0057A9F0(EMW *em);
int kyusyu_char_set2_0057AAA0(EMW *em);
void kyusyu_senkai_ret_0057AB40(EMW *em, EM01W *w);
void ef_move_sub_00574EE0(EMW *em, EM01W *w);
void em01_uvmove(EMW *em);
void em01_senkai_sub2(EMW *, int, int);
void em01_senkai_sub3(EMW *, int, int);
int em01_horm_main(EMW *);
void em01_horm_init(EMW *);
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
void wyvern_kill_cnt_up(u8 *, int);
extern u8 User_data[];
extern EMW em_work[];
void Quest_enemy_capture();
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void cpRotMatrixYXZ2(s32 *, FLMAT *);
void Eft13_set_em(EMW *, int, int);
int Event_flag_ck(int);
u16 em01_demo_senkai_target(EMW *);
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
void em01_main_sub(EMW *em, EM01W *w);
void Eft13_set_em_scl(EMW *, int, f32, int);
void Eft15_set3(EMW *, int, f32, int);
int em_frame_check3(EMW *, int, f32, f32);
void em01_to_normal();
void em01_to_fly();
void em01_frame_reset();
void em01_reset_char_set();

/* Turn toward the target by at most 0x40 per frame (same as em03). */
#define EM01_TURN(em)                                                     \
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

#define FLY_FLOOR(em)                    \
    if ((em)->pos[1] < (em)->x5AC) {     \
        (em)->pos[1] = (em)->x5AC;       \
    }

#define DMG_SIMPLE(NAME, CH)                     \
    static void NAME(EMW *em, EM01W *w) {        \
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
                em01_to_normal(em, 0, 0);        \
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

void em_mv06_00569D60(EMW *em, EM01W *w) {
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0xA, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM01_TURN(em);
            mot_miration_ret(em, v);
            w->dist = w->dist - v[2];
            if (w->dist <= 0.0f) {
                em->work08 = 1;
            }
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_mv07_00569EC0(EMW *em, EM01W *w) {
    u16 v;
    u32 d;
    int a;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x36, 0, 0);
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        a = em->ang[1];
        v = w->dang;
        d = (v - (a & 0xFFFF)) & 0xFFFF;
        if ((u32)((d + 0x200) & 0xFFFF) < 0x400U) {
            em->ang[1] = v;
        } else if (d < 0x8000U) {
            em->ang[1] = (a + 0x200) & 0xFFFF;
        } else {
            em->ang[1] = (a - 0x200) & 0xFFFF;
        }
        if (em_frame_check2(em, 0, 60.0f)) {
            em->x05++;
        }
        break;
    case 2:
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_fly00_0056A000(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em01_fly_adjy2_init(em, 2);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check(em, 38.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em01_fly_adjy2(em);
        }
        break;
    case 2:
        if (em01_fly_adjy2(em)) {
            em->x05++;
            em01_to_fly(em, 0);
        }
        break;
    }
}

void em_fly01_0056A100(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em->ang[0] = 0;
        em->ang[2] = 0;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_y = -10.0f;
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (!(630.0f + em->x5AC <= em->pos[1]) &&
            (em_frame_check(em, 10.0f, 0) || em_frame_check(em, 86.0f, 0) || em_frame_check(em, 160.0f, 0))) {
            em->x05++;
            em_char_set(em, 0xB, 0, 0);
            em->adj_y = (em->x5AC - em->pos[1]) / 30.0f;
            if (!(em->adj_y < 0.0f)) {
                em->adj_y = -10.0f;
            }
            em->work08 = 0x1E;
        }
        break;
    case 2:
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (--em->work08 < 0) {
            em->x05++;
            em_char_set(em, 0x13, 0, 0);
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
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

void em_fly02_0056A340(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        break;
    case 1:
        em01_fly_adjy(em, 1);
        if (--em->work08 <= 0) {
            em->x05++;
            em01_act_set(em, 2, 1, 1);
        }
        senkai_target(em);
        if (em->work08 == 0x12C) {
            em01_to_fly(em, 0);
        }
        break;
    case 2:
        em01_fly_adjy(em, 1);
        senkai_target(em);
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

void em_fly03_0056A450(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em_rate_clear(em);
        em01_fly_adjy2_init(em, 2);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 50.0f) && em_frame_check2(em, 0, 114.0f) == 0) {
            senkai_target(em);
        }
        if (em_frame_check2(em, 0, 30.0f)) {
            em->x388 = 2;
            em01_fly_adjy2(em);
        }
        if (em->x194 == 0) {
            em->x05++;
        }
        break;
    case 2:
        if ((em01_fly_adjy2(em) & 0xFF) && em->pos[1] <= 800.0f + em->x5AC) {
            em->x05++;
            em_char_set(em, 0xB, 0, 0);
            em_rate_clear(em);
            em->adj_y = (em->x5AC - em->pos[1]) / 30.0f;
            if (!(em->adj_y < 0.0f)) {
                em->adj_y = -10.0f;
            }
        }
        break;
    case 3:
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
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
            w->x06 = 0x96;
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

void em_fly04_0056A6E0(EMW *em, EM01W *w) {
    int done = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em01_fly_adjy2_init(em, 3);
        em->adj_z = 20.0f;
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f)) {
            em->x388 = 2;
            done = em01_fly_adjy2(em);
        }
        if (done != 0 && em->pos[1] <= em->x5AC) {
            em_char_set(em, 0x13, 0, 0);
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            w->x06 = 0x96;
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

void em_fly05_0056A850(EMW *em, EM01W *w) {
    int done = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em01_fly_adjy2_init(em, 3);
        em->adj_z = -20.0f;
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f)) {
            em->x388 = 2;
            done = em01_fly_adjy2(em);
        }
        if (done != 0 && em->pos[1] <= em->x5AC) {
            em_char_set(em, 0x13, 0, 0);
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            w->x06 = 0x96;
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

void em_fly06_0056A9C0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em_char_set(em, 0xC, 0, 0);
        em->x05++;
        em->x07 = 0;
        em->x3F4 = 0;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z <= 50.0f) {
            em->adj_z = 50.0f;
        }
        w->x18 = 1;
        break;
    case 1:
        if (em->x07 == 0 && em->x194 == 0) {
            em->x07++;
            em_char_set(em, 0xE, 0, 0);
        }
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f || --em->work08 <= 0) {
            if (--w->x05 <= 0) {
                em01_to_fly(em, 1);
            } else {
                em->x883++;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em->work08 = (int)(1.5f * CalcDistanceXZ(em->pos, em->tgt_pos) / 50.0f);
                em_act_set(em, 2, 6);
            }
        }
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 1, 0);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

void em_fly07_0056ABE0(EMW *em, EM01W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        break;
    case 1:
        senkai_target(em);
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        em01_fly_adjy(em, 1);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 10.0f * em->adj_z) {
            em->x05++;
            em_rate_clear(em);
            em01_to_fly(em, 0);
        }
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

void em_fly08_0056AD20(EMW *em, EM01W *w) {
    u8 ar;
    FLYNEED *h = em_hungry_tbl[em->kind]; FLYNEED *t = em_thirst_tbl[em->kind];
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_rate_clear(em);
        em->adj_z = 50.0f;
        em_char_set(em, 0xC, 0, 0);
        NextStage_Dir_Set(em, em->tgt_pos);
        w->x18 = 1;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 1, 0);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (AreaFieldInCheck(em->stg, em->pos) == 0) {
            em->x05++;
        }
        break;
    case 2:
        em->x05++;
        if (t->x14 < em->thirst) {
            em->thirst -= t->x14;
        } else {
            em->thirst = 0;
        }
        if (h->x14 < em->hungry) {
            em->hungry -= h->x14;
        } else {
            em->hungry = 0;
        }
        break;
    case 3:
        em->x05++;
        em->work08 = 0x258;
        Em_Next_Stage_Pos();
        ar = em->x92F;
        if ((u16)em->x73A == ar || ar == 0xFF) {
            WyvernAreaMove((PLW *)em);
            em01_act_set(em, 2, 9, 1);
        } else {
            if (em->x8C3 == 0) {
                em->x73A = ar;
                em->x829 = ar & 0xFFFF;
                em->x827 = 3;
                cmd_target_kind_set(em, em->tgt_pos);
            }
            em01_act_set(em, 2, 0xD, 1);
            WyvernAreaMove((PLW *)em);
        }
        break;
    case 4:
        if (em->x8C3 == 0) {
            if (--em->work08 <= 0) {
                em->x05 = 3;
            }
        }
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

void em_fly09_0056AFC0(EMW *em, EM01W *w) {
    f32 d;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_z = 80.0f;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[1] = w->dang;
        em->x92F = 0xFF;
        w->x18 = 1;
        em_area_move_init(em);
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 5, 0);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        em->work08--;
        if (d <= 1000.0f || em->work08 < 0) {
            em->x05++;
            em->work08 = 0x258;
            em01_act_set(em, 2, 1, 1);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            if (--em->work08 <= 0) {
                em->work08 = 0x258;
                em01_act_set(em, 2, 1, 1);
            }
        }
        break;
    }
    FLY_FLOOR(em);
}
