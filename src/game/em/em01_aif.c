/* em01 AI, run 6: em_atk12_0056E6F0 .. em01_main_sub (game.bin 0x0056E6F0-0x00574AC8). Matching functions of em01_ai_nm.c (that file holds the
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

void em_atk12_0056E6F0(EMW *em, EM01W *w) {
    f32 v[3];
    int r;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 1;
        em->x6FD = 1;
        break;
    case 1:
        r = smell_search(em, 0x23, em->x700);
        get_joint_pos_em(em, 0x23, v);
        if (!(150.0f < flvecCalcDistance(v, em->x700))) {
            em->x05++;
            em_char_set(em, 0x23, 0, 0);
            break;
        }
        if (r == 0) {
            em->x3F4 = 0;
            em->x6FD = 0;
            em01_to_normal(em, 0, 0);
        }
        break;
    case 2:
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em_char_set(em, 0x30, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em->x3F4 = 0;
            em->x6FD = 0;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_atk13_0056E860(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x53, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

/* A file static in the original. */
void em_atk14(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x38, 0, 0);
        /* fallthrough */
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_atk15_0056E970(EMW *em, EM01W *w) {
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            int d;

            d = (u16)((u16)Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]);
            if (d <= 0x8000) {
                if (d <= 0x3F) {
                    em->ang[1] += d;
                } else {
                    em->ang[1] += 0x80;
                }
            } else if (d > 0xFFC0) {
                em->ang[1] += d;
            } else {
                em->ang[1] -= 0x80;
            }
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

void em_atk16_0056EAD0(EMW *em, EM01W *w) {
    int done = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x34, 0, 0);
        em01_fly_adjy2_init(em, 8);
        break;
    case 1:
        if (em_frame_check(em, 90.0f, 0)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 2, 0x2222, 0);
        }
        if (em_frame_check2(em, 0, 76.0f)) {
            em->x388 = 2;
            done = em01_fly_adjy2(em);
        }
        if (done != 0 && em->pos[1] <= em->x5AC) {
            em_char_set(em, 0x39, 0, 0);
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            ground_land_eff_set_0057A7E0(em);
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_atk17_0056EC80(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x31, 0, 0);
        em01_fly_adjy2_init(em, 0xD);
        break;
    case 1:
        em01_fly_adjy2(em);
        senkai_player(em);
        if (EMF(em, s32, 0x1E4) == 0) {
            if (--w->x1A <= 0) {
                em->x05++;
            }
        }
        break;
    case 2:
        em01_fly_adjy2(em);
        sound_call_00574D40(em, 0x78, 0x20, 0x23);
        senkai_player(em);
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 0, 0);
        }
        break;
    case 3:
        em01_fly_adjy2(em);
        senkai_player(em);
        if (em_frame_check(em, 6.0f, 0)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 2, 0x1E94, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 4, 0);
        }
        break;
    case 4:
        em01_fly_adjy2(em);
        senkai_player(em);
        if (em_frame_check(em, 6.0f, 1)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 2, 0x1E94, 0);
        }
        if (em_frame_check(em, 40.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xF, 2, 0x22);
        }
        break;
    case 5:
        em01_fly_adjy2(em);
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_atk18_0056EF60(EMW *em, EM01W *w) {
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM01_TURN(em);
            mot_miration_ret(em, v);
            w->dist = w->dist - v[2];
            if (w->dist <= 500.0f) {
                em->work08 = 1;
            }
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x5B, 0, 0);
            shell01_set(em, 0x24);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

/* A file static in the original. */
void em_atk19(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x23, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

/* A file static in the original. */
void em_atk20(EMW *em, EM01W *w) {
    int done = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x5C, 0, 0);
        em01_fly_adjy2_init(em, 0xA);
        break;
    case 1:
        if (em_frame_check2(em, 0, 94.0f)) {
            em->x388 = 2;
            done = em01_fly_adjy2(em);
        }
        if (done != 0 && em->pos[1] <= 372.0f + em->x5AC) {
            em->x05++;
            em_char_set(em, 0x5D, 0, 0);
            em01_fly_adjy2_init(em, 0xB);
        }
        break;
    case 2:
        if ((em01_fly_adjy2(em) & 0xFF) && em->pos[1] <= em->x5AC) {
            em->x05++;
            em_char_set(em, 0x5E, 0, 0);
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            ground_land_eff_set_0057A7E0(em);
            Em_set_quake_sub(em, 2);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

/* Variant of atk 8 that lands (ground contact after the pursuit). */
void em_atk21_0056F340(EMW *em, EM01W *w) {
    STAGE_DATA *sd;
    s8 r;
    PLW *pl;
    f32 p2[4];
    f32 p1[4];
    f32 p3[4];
    f32 adj;
    s16 t;

    sd = Stage_data_get(em->stg);
    t = w->x52;
    if (t != 0) {
        t = t - 1;
        w->x52 = t;
        if (t <= 0) {
            w->x52 = 0;
        }
    }
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        w->x52 = 0;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 3, 1);
        if (100.0f < em->adj_z) {
            em->adj_z -= 2.0f;
        } else if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) {
            em->x05++;
            em->work08 = 0x258;
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (--em->work08 <= 0) {
            kyusyu_senkai_ret_0057AB40(em, w);
        }
        break;
    case 2:
        if (em->x617 == -1) {
            kyusyu_senkai_ret_0057AB40(em, w);
        } else {
            if (80.0f < em->adj_z) {
                em->adj_z -= 1.0f;
            }
            r = em->x617;
            em_pl_pos_set(em, r, p1);
            pl = &player_work[r];
            World_calc2(pl->stg, p1, p2);
            w->dang = Em_Calc_angY(em->x754, p2);
            w->dang = w->dang - em->ang[1];
            em01_senkai_sub(em, 3, 1);
            if (w->dang <= 0x800 || w->dang >= 0xF800) {
                if (4000.0f >= CalcDistanceXZ(em->pos, p1)) {
                    kyusyu_senkai_ret_0057AB40(em, w);
                } else {
                    em->x05++;
                    w->turn = 0x100;
                    em_char_set(em, 0x2C, 0, 0);
                    em->work08 = 0x258;
                }
            }
            w->spd[0] = em->ang[0];
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
            speed_add(em, w->spd);
            if (--em->work08 <= 0 || !(em_target_pl_samestage_ck(em) & 0xFF)) {
                kyusyu_senkai_ret_0057AB40(em, w);
            }
        }
        break;
    case 3:
        if (em->ang[2] != 0) {
            if (em->ang[2] <= 0x8000) {
                em->ang[2] = em->ang[2] - w->bank_spd;
            } else {
                em->ang[2] = em->ang[2] + w->bank_spd;
            }
        }
        if (em_frame_check2(em, 0, 46.0f)) {
            xang_set_pl(em, 0, -200.0f);
            if (50.0f < em->adj_z) {
                em->adj_z -= 0.1f;
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x57, 0, 0);
            em->adj_z = 100.0f;
            em->work08 = 0x258;
        }
        senkai_player(em);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0xF;
        speed_add(em, w->spd);
        em->pos[1] -= 0.1f;
        if (--em->work08 <= 0 || !(em_target_pl_samestage_ck(em) & 0xFF)) {
            kyusyu_senkai_ret_0057AB40(em, w);
        }
        break;
    case 4:
        if (em->pos[1] <= 1000.0f + sd->floor_y) {
            em->x05++;
            em->work08 = 0x258;
        }
        if (kyusyu_char_set_0057A9F0(em)) {
            if (em->x05 == 4) {
                em->x05++;
                em->work08 = 0x258;
            }
        }
        if (em->char0 == 0x415) {
            xang_set_pl(em, 2, 0.0f);
        } else {
            xang_set_pl(em, 0, -200.0f);
        }
        senkai_player(em);
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        if (w->x52 == 0) {
            xang_calc_pl(em, w->spd, -3000.0f, 0.0f);
        }
        if (em->x74C != 0) {
            w->spd[0] = 0;
            w->x52 = 0x1E;
        }
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            adj = em->adj_y;
            if (adj < 0.0f) {
                em->pos[1] -= adj;
            }
            em->pos[1] += 30.0f;
        }
        if (--em->work08 <= 0 || !(em_target_pl_samestage_ck(em) & 0xFF)) {
            kyusyu_senkai_ret_0057AB40(em, w);
        }
        break;
    case 5:
        kyusyu_char_set_0057A9F0(em);
        if (em->char0 == 0x415 && em->x194 == 0) {
            em->x05 = 6;
            em->x3C0[0] = 0.0f;
            em->x3C0[2] = -10.0f;
            em->x3C0[1] = -10.0f;
        }
        senkai_player(em);
        if (w->x52 == 0) {
            xang_set_pl(em, 2, 100.0f);
            if (em->char0 == 0x415 && em_frame_check2(em, 0, 76.0f)) {
                xang_calc_pl(em, w->spd, 50.0f, 0.0f);
            } else if (em->x617 != -1) {
                em_pl_pos_set(em, em->x617, p3);
                if (!(200.0f + p3[1] < em->pos[1])) {
                    xang_calc_pl(em, w->spd, 50.0f, 0.0f);
                } else {
                    xang_calc_pl(em, w->spd, -2000.0f, 0.0f);
                }
            } else {
                xang_calc_pl(em, w->spd, -2000.0f, 0.0f);
            }
        }
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        if (em->x74C != 0) {
            w->spd[0] = 0;
            w->x52 = 0x1E;
        }
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            adj = em->adj_y;
            if (adj < 0.0f) {
                em->pos[1] -= adj;
            }
            em->pos[1] += 30.0f;
        }
        if (--em->work08 <= 0 || !(em_target_pl_samestage_ck(em) & 0xFF)) {
            kyusyu_senkai_ret_0057AB40(em, w);
        }
        break;
    case 6:
        if (em->adj_z < 0.0f) {
            em->adj_z = 0.0f;
            em->x3C0[2] = 0.0f;
        }
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x29, 0, 0);
        }
        break;
    case 7:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

/* A file static in the original. */
void em_atk22(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x31, 0, 0);
        em01_fly_adjy2_init(em, 0xD);
        break;
    case 1:
        em01_fly_adjy2(em);
        senkai_player(em);
        if (EMF(em, s32, 0x1E4) == 0) {
            if (--w->x1A <= 0) {
                em->x05++;
            }
        }
        break;
    case 2:
        em01_fly_adjy2(em);
        sound_call_00574D40(em, 0x78, 0x20, 0x23);
        senkai_player(em);
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 0, 0);
        }
        break;
    case 3:
    case 4:
        em01_fly_adjy2(em);
        senkai_player(em);
        if (em_frame_check(em, 6.0f, 1)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 2, 0x1E94, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 4, 0);
        }
        break;
    case 5:
        em01_fly_adjy2(em);
        senkai_player(em);
        if (em_frame_check(em, 6.0f, 1)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 2, 0x1E94, 0);
        }
        if (em_frame_check(em, 40.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xF, 2, 0x22);
        }
        break;
    case 6:
        em01_fly_adjy2(em);
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

/* A file static in the original. */
void em_atk23(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        break;
    case 1:
        if (em01_horm_main(em)) {
            em->x05++;
            em->x3F4 = 0;
            em_char_set(em, 0x32, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check2(em, 0, 256.0f) && em_frame_check2(em, 0, 316.0f) == 0 && !(*(u16 *)&game_w.x1E & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
        }
        if (em_frame_check(em, 78.0f, 0)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 0, 0x5B0, 0);
        }
        if (em_frame_check(em, 120.0f, 0)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 0, 0x4FA, 0xF1C8);
        }
        if (em_frame_check(em, 164.0f, 0)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 0, 0x444, 0x9F5);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->x3F4 = 0;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

/* A file static in the original. */
void em_atk24(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        break;
    case 1:
        if (em01_horm_main(em)) {
            em->x05++;
            em->x3F4 = 0;
            em_char_set(em, 0x2F, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check2(em, 0, 154.0f) && em_frame_check2(em, 0, 276.0f) == 0) {
            if (!(*(u16 *)&game_w.x1E & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            }
        } else if (em_frame_check(em, 78.0f, 0)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 0, 0xE39, 0);
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05++;
            if (--w->x1A <= 0) {
                em01_to_normal(em, 0, 0);
            } else {
                em01_horm_init(em);
                em_act_set(em, 3, 0x18);
            }
        }
        break;
    }
}

/* A file static in the original. */
void em_atk25(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        break;
    case 1:
        if (em01_horm_main(em)) {
            em->x05++;
            em->x3F4 = 0;
            em_char_set(em, 0x2F, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check2(em, 0, 154.0f) && em_frame_check2(em, 0, 276.0f) == 0) {
            if (!(*(u16 *)&game_w.x1E & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            }
        } else if (em_frame_check(em, 78.0f, 0)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 0, 0xF1C8, 0);
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05++;
            if (--w->x1A <= 0) {
                em01_to_normal(em, 0, 0);
            } else {
                em01_horm_init(em);
                em_act_set(em, 3, 0x19);
            }
        }
        break;
    }
}

DMG_SIMPLE(em_dmg00_00570530, 0x3C)

DMG_SIMPLE(em_dmg01_005705C0, 0x42)

DMG_SIMPLE(em_dmg02_00570650, 0x3F)

void em_dmg03_005706E0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x40, 0, 0);
        if (em->kind == 1) {
            Quest_enemy_hagi_set(em, 2);
        } else {
            Quest_enemy_hagi_set(em, 8);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_dmg04_005707A0(EMW *em, EM01W *w) {
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x41, 0, 0);
        em->x948 |= 1;
        em->x958 += 1;
        Em_Sleep_Flag_Ck(em);
        break;
    case 1:
        if (em_frame_check(em, 6.0f, 0) && (s8)em->ex[0xA3] != 0 && em->kind != 0x14) {
            em->ex[0xA3] = 0;
        }
        if (em_frame_check2(em, 0, 52.0f) && em_frame_check2(em, 0, 122.0f) == 0) {
            em->ang[1] -= (u16)(u32)(936.0f * em->act_spd);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->ang[1] -= 8;
            em01_act_set(em, 4, 0xF, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 4, 0xF, 3);
        }
        break;
    }
}

void em_dmg05_00570970(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x45, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0;
            em_char_set(em, 0x46, 0, 0);
        }
        break;
    case 2:
        em->work08++;
        if (em->work08 >= 0x78) {
            em->x05++;
            em_char_set(em, 0x47, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_dmg06_00570A90(EMW *em, EM01W *w) {
    em01_to_fly(em, 1);
}

void em_dmg07_00570AA0(EMW *em, EM01W *w) {
    em01_to_fly(em, 1);
}

void em_dmg08_00570AB0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4D, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[0] = 0.0f;
        em->x3C0[1] = -10.0f;
        em->x3C0[2] = 0.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x4E, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x47, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
            em_rate_clear(em);
        }
        break;
    }
}

void em_dmg09_00570C30(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4D, 0, 0);
        em->x88B = 0;
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[0] = 0.0f;
        em->x3C0[1] = -10.0f;
        em->x3C0[2] = 0.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x4E, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x47, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
            em_rate_clear(em);
        }
        break;
    }
}

DMG_SIMPLE(em_dmg10_00570DC0, 0x3D)

void em_dmg11_00570E50(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4C, 0, 0);
        Em_Mahi_Start(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em01_act_set(em, 0, 0x21, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, 0x21, 4);
        }
        break;
    }
}

void em_dmg12_00570F20(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x5F, 0, 0);
        em->x762 = 3;
        em->x8BD = 1;
        em->x9EA = 0x32;
        em->x88B = 1;
        Em_Sleep_Flag_Ck(em);
        break;
    case 1:
        if (em->x9EA < 5) {
            em->x9EA = 5;
        }
        if (em->x194 == 0) {
            em->x05++;
            em01_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x9EA < 5) {
            em->x9EA = 5;
        }
        if (em->x8C3 == 0) {
            em01_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

void em_dmg13_00571050(EMW *em, EM01W *w) {
    em->x9EA = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x60, 0, 0);
        em->x8BD = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            if (--em->x95A <= 0) {
                em->x05++;
                em_char_set(em, 0x60, 0, 0);
                em01_act_set(em, 2, 0x17, 4);
            } else {
                em_char_set(em, 0x60, 0, 0);
            }
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 2, 0x17, 4);
        }
        if (em->x194 == 0) {
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    }
}

void em_dmg14_00571180(EMW *em, EM01W *w) {
    em->x9EA = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_cmd_reset(em);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em01_act_set(em, 4, 0x11, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 4, 0x11, 3);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

void em_dmg15_00571290(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_cmd_reset(em);
        em_tail_off_sub(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_dmg16_00571310(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4A, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0;
            em_char_set(em, 0x4B, 0, 0);
        }
        break;
    case 2:
        em->work08++;
        if (em->work08 >= 0x78) {
            em->x05++;
            em_char_set(em, 0x48, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_dmg17_00571430(EMW *em, EM01W *w) {
    em->x9EA = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x60, 0, 0);
        Em_Mahi_End(em);
        em->x8BD = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            if (--em->x95A <= 0) {
                em->x05++;
                em01_act_set(em, 2, 0x17, 4);
            } else {
                em_char_set(em, 0x60, 0, 0);
            }
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 2, 0x17, 4);
        }
        if (em->x194 == 0) {
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    }
}

void em_dmg18_00571550(EMW *em, EM01W *w) {
    em->x9EA = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x62, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em01_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

void em_dmg19_00571650(EMW *em, EM01W *w) {
    em->x9EA = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x62, 0, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em01_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

/* Demo (event) flight: the monster picks up / follows the em at x944 (the partner), lands near it and roars. */
void em_demo00_00571750(EMW *em, EM01W *w) {
    EMW *t;
    STAGE_DATA *sd;
    f32 v[3];
    f32 inA[3];
    f32 outA[3];
    s32 angA[3];
    f32 inB[3];
    f32 outB[3];
    s32 angB[3];
    FLMAT matA;
    FLMAT matB;
    f32 adj;

    t = em->x944;
    sd = Stage_data_get(em->stg);
    if (t == 0) {
        SetVector(v, 10000.0f, 0.0f, 10000.0f);
    } else {
        SetVector(v, t->pos[0], t->pos[1], t->pos[2]);
    }
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em->adj_z = 100.0f;
        em->tgt_pos[0] = 3000.0f;
        em->tgt_pos[1] = 2000.0f;
        em->tgt_pos[2] = 9000.0f;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 7, 1);
        if (100.0f < em->adj_z) {
            em->adj_z -= 2.0f;
        } else if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 4000.0f) {
            em->x05++;
            em->work08 = 0x384;
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            adj = em->adj_y;
            if (adj < 0.0f) {
                em->pos[1] -= adj;
            }
            em->pos[1] += 30.0f;
        }
        break;
    case 2:
        if (80.0f < em->adj_z) {
            em->adj_z -= 1.0f;
        }
        w->dang = Em_Calc_angY(em->pos, v);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 3, 1);
        if (w->dang <= 0x800 || w->dang >= 0xF800) {
            if (4000.0f < CalcDistanceXZ(em->pos, v)) {
                em->x05++;
                w->turn = 0x100;
                em_char_set(em, 0x2C, 0, 0);
                em->work08 = 0x708;
            }
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            adj = em->adj_y;
            if (adj < 0.0f) {
                em->pos[1] -= adj;
            }
            em->pos[1] += 30.0f;
        }
        if (--em->work08 <= 0 && em->x05 == 2) {
            em->x05 = 0;
        }
        break;
    case 3:
        if (em->ang[2] != 0) {
            if (em->ang[2] <= 0x8000) {
                em->ang[2] = em->ang[2] - w->bank_spd;
            } else {
                em->ang[2] = em->ang[2] + w->bank_spd;
            }
        }
        if (em_frame_check2(em, 0, 46.0f)) {
            if (50.0f < em->adj_z) {
                em->adj_z -= 0.1f;
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x57, 0, 0);
            em->adj_z = 100.0f;
            em->work08 = 0x708;
        }
        SetVector(em->tgt_pos, v[0], v[1], v[2]);
        senkai_target(em);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0xF;
        speed_add(em, w->spd);
        em->pos[1] -= 1.0f;
        break;
    case 4:
        if (em->pos[1] <= 1000.0f + sd->floor_y) {
            em->x05++;
            em->work08 = 0x708;
        }
        if (kyusyu_char_set2_0057AAA0(em)) {
            if (em->x05 == 4) {
                em->x05++;
                em->work08 = 0x708;
            }
        }
        SetVector(em->tgt_pos, v[0], v[1], v[2]);
        senkai_target(em);
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, -3000.0f, 0.0f);
        speed_add(em, w->spd);
        break;
    case 5:
        kyusyu_char_set2_0057AAA0(em);
        SetVector(em->tgt_pos, v[0], v[1], v[2]);
        senkai_target(em);
        if (!(200.0f + em->tgt_pos[1] < em->pos[1])) {
            xang_calc_target(em, w->spd, 0.0f, 0.0f);
        } else {
            xang_calc_target(em, w->spd, -2000.0f, 0.0f);
        }
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (CalcDistanceXZ(em->pos, v) <= 600.0f ||
            (em->char0 == 0x415 && (em_frame_check2(em, 0, 52.0f) || em->x194 == 0))) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x29, 4, 0);
            if (t != 0) {
                angA[0] = 0;
                angA[1] = em->ang[1];
                angA[2] = 0;
                SetVector(inA, 0.0f, 0.0f, 600.0f);
                cpRotMatrixYXZ2(angA, &matA);
                flvecApplyMat33(outA, inA, (f32 *)&matA);
                t->pos[0] = em->pos[0] + outA[0];
                t->pos[2] = em->pos[2] + outA[2];
                if (t->mode != 6 || t->x15 != 1) {
                    em_act_set(t, 6, 1);
                    t->ang[1] = (em->ang[1] - 0x4000) & 0xFFFF;
                }
            }
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x23, 0, 0);
            if (t != 0) {
                em_act_set(t, 6, 2);
            }
        }
        if (t != 0) {
            angB[0] = 0;
            angB[1] = em->ang[1];
            angB[2] = 0;
            SetVector(inB, 0.0f, 0.0f, 600.0f);
            cpRotMatrixYXZ2(angB, &matB);
            flvecApplyMat33(outB, inB, (f32 *)&matB);
            t->pos[0] = em->pos[0] + outB[0];
            t->pos[2] = em->pos[2] + outB[2];
            t->x40E = 5;
        }
        break;
    case 7:
        if (em_frame_check(em, 60.0f, 0) || em_frame_check(em, 202.0f, 0)) {
            Eft13_set_em(em, 0x1C, 6);
        }
        if (em_frame_check(em, 52.0f, 0) || em_frame_check(em, 208.0f, 0)) {
            Eft13_set_em(em, 0x16, 6);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x4F, 0, 0);
            if (t != 0) {
                em_act_set(t, 6, 3);
            }
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3B, 0, 0);
        }
        break;
    case 9:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
            if (t != 0) {
                t->act_spd = 0.0f;
            }
        }
        break;
    case 10:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x14, 0, 0);
        }
        break;
    case 11:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

/* Demo: the monster arrives over the village/stage, lands and roars. */
void em_demo01_005720E0(EMW *em, EM01W *w) {
    f32 d;

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05 = 2;
        em->x3F4 = 0;
        em->x388 = 2;
        em->pos[0] = 0.0f;
        em->pos[1] = 3500.0f;
        em->pos[2] = 14500.0f;
        em->tgt_pos[0] = 13000.0f;
        em->tgt_pos[1] = 5700.0f;
        em->tgt_pos[2] = 23000.0f;
        em->ang[0] = 0;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->ang[2] = 0;
        em_char_set(em, 0xE, 0, 0);
        em_rate_clear(em);
        em->adj_z = 100.0f;
        w->turn = 0x100;
        w->spd[2] = 0;
        em->work08 = 0x12C;
        em->ex[0x90] = 0;
        break;
    case 1:
    case 2:
    case 3:
        if (em->x194 == 0) {
            if (em->char0 == 0x3F4) {
                em_char_set(em, 0xE, 0, 0);
            } else {
                em_char_set(em, 0xC, 0, 0);
            }
        }
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 0x1F, 1);
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 1000.0f) {
            em->x05++;
            switch (em->x05) {
            case 2:
                em->work08 = 0x12C;
                em->tgt_pos[0] = 13000.0f;
                em->tgt_pos[1] = 5700.0f;
                em->tgt_pos[2] = 23000.0f;
                break;
            case 3:
                em->work08 = 0x12C;
                em->tgt_pos[0] = 20800.0f;
                em->tgt_pos[1] = 8000.0f;
                em->tgt_pos[2] = 23000.0f;
                break;
            case 4:
                em->ex[0x90] = 1;
                em->pos[0] = 11800.0f;
                em->pos[1] = 2000.0f;
                em->pos[2] = 10330.0f;
                em->ang[0] = 0;
                em->ang[1] = 0xB000;
                em->ang[2] = 0;
                em_char_set(em, 0x5A, 0, 0);
                em01_fly_adjy2_init(em, 9);
                break;
            }
        }
        break;
    case 4:
        if (em01_fly_adjy2(em) & 0xFF) {
            em->x05++;
            em_rate_clear(em);
            em_char_set(em, 0xF, 0, 0);
            em->adj_y = -10.0f;
            em->tgt_pos[0] = 9550.0f;
            em->tgt_pos[1] = 0.0f;
            em->tgt_pos[2] = 9840.0f;
            em->adj_z = CalcDistanceXZ(em->pos, em->tgt_pos) / 167.0f;
        }
        break;
    case 5:
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
    case 6:
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
    case 7:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x36, 0, 0);
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 9:
        if (Event_flag_ck(0xD) == 1) {
            em->x05++;
            em->x9E1 = 0;
            em->x40C = 0;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

/* Demo: like demo00 but the partner is the em of kind 0xC found in em_work. */
void em_demo02_00572660(EMW *em, EM01W *w) {
    EMW *t;
    f32 v[3];
    f32 inA[3];
    f32 outA[3];
    s32 angA[3];
    f32 inB[3];
    f32 outB[3];
    s32 angB[3];
    FLMAT matA;
    FLMAT matB;
    s8 i;

    Stage_data_get(em->stg);
    for (i = 0, t = em_work; i < 0x14; i++, t++) {
        if (t->kind == 0xC && t->be_flag != 0 && t->type == 0) {
            goto found;
        }
    }
    t = 0;
found:
    if (t == 0) {
        SetVector(v, 10000.0f, 0.0f, 10000.0f);
    } else {
        t->x944 = em;
        SetVector(v, t->pos[0], t->pos[1], t->pos[2]);
    }
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em->adj_z = 100.0f;
        em->pos[0] = 20000.0f;
        em->pos[1] = 2000.0f;
        em->pos[2] = 12000.0f;
        em->tgt_pos[0] = 26500.0f;
        em->tgt_pos[1] = 2000.0f;
        em->tgt_pos[2] = 19000.0f;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em_char_set(em, 0xE, 0, 0);
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 7, 1);
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) {
            em->x05++;
            em->work08 = 0x384;
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    case 2:
        if (80.0f < em->adj_z) {
            em->adj_z -= 1.0f;
        }
        w->dang = Em_Calc_angY(em->pos, v);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 3, 1);
        if (w->dang <= 0x800 || w->dang >= 0xF800) {
            if (4000.0f < CalcDistanceXZ(em->pos, v)) {
                em->x05++;
                w->turn = 0x100;
                em_char_set(em, 0x2C, 0, 0);
                em->work08 = 0x708;
            }
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    case 3:
        if (em->ang[2] != 0) {
            if (em->ang[2] <= 0x8000) {
                em->ang[2] = em->ang[2] - w->bank_spd;
            } else {
                em->ang[2] = em->ang[2] + w->bank_spd;
            }
        }
        if (em_frame_check2(em, 0, 46.0f)) {
            if (50.0f < em->adj_z) {
                em->adj_z -= 0.1f;
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x57, 0, 0);
            em->adj_z = 100.0f;
            em->work08 = 0x708;
        }
        SetVector(em->tgt_pos, v[0], v[1], v[2]);
        em01_demo_senkai_target(em);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0xF;
        speed_add(em, w->spd);
        em->pos[1] -= 1.0f;
        break;
    case 4:
        if (em->pos[1] <= 1000.0f) {
            em->x05++;
            em->work08 = 0x708;
        }
        if (kyusyu_char_set2_0057AAA0(em)) {
            if (em->x05 == 4) {
                em->x05++;
                em->work08 = 0x708;
            }
        }
        SetVector(em->tgt_pos, v[0], v[1], v[2]);
        em01_demo_senkai_target(em);
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, -3000.0f, 0.0f);
        speed_add(em, w->spd);
        break;
    case 5:
        kyusyu_char_set2_0057AAA0(em);
        SetVector(em->tgt_pos, v[0], v[1], v[2]);
        em01_demo_senkai_target(em);
        if (!(200.0f + em->tgt_pos[1] < em->pos[1])) {
            xang_calc_target(em, w->spd, 0.0f, 0.0f);
        } else {
            xang_calc_target(em, w->spd, -2000.0f, 0.0f);
        }
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (CalcDistanceXZ(em->pos, v) <= 600.0f ||
            (em->char0 == 0x415 && (em_frame_check2(em, 0, 52.0f) || em->x194 == 0))) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x29, 4, 0);
            if (t != 0) {
                angA[0] = 0;
                angA[1] = em->ang[1];
                angA[2] = 0;
                SetVector(inA, 0.0f, 0.0f, 600.0f);
                cpRotMatrixYXZ2(angA, &matA);
                flvecApplyMat33(outA, inA, (f32 *)&matA);
                t->pos[0] = em->pos[0] + outA[0];
                t->pos[2] = em->pos[2] + outA[2];
                if (t->mode != 6 || t->x15 != 1) {
                    em_act_set(t, 6, 1);
                    t->ang[1] = (em->ang[1] - 0x4000) & 0xFFFF;
                }
            }
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x23, 0, 0);
            if (t != 0) {
                em_act_set(t, 6, 2);
            }
        }
        if (t != 0) {
            angB[0] = 0;
            angB[1] = em->ang[1];
            angB[2] = 0;
            SetVector(inB, 0.0f, 0.0f, 600.0f);
            cpRotMatrixYXZ2(angB, &matB);
            flvecApplyMat33(outB, inB, (f32 *)&matB);
            t->pos[0] = em->pos[0] + outB[0];
            t->pos[2] = em->pos[2] + outB[2];
            t->x40E = 5;
        }
        break;
    case 7:
        if (em_frame_check(em, 60.0f, 0) || em_frame_check(em, 202.0f, 0)) {
            Eft13_set_em(em, 0x1C, 6);
        }
        if (em_frame_check(em, 52.0f, 0) || em_frame_check(em, 208.0f, 0)) {
            Eft13_set_em(em, 0x16, 6);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x4F, 0xA, 0);
            if (t != 0) {
                em_act_set(t, 6, 3);
            }
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3B, 0, 0);
        }
        break;
    case 9:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
            if (t != 0) {
                t->act_spd = 0.0f;
            }
        }
        break;
    case 10:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x14, 0, 0);
        }
        break;
    case 11:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

void em_demo04_00572FA0(EMW *em, EM01W *w) {
    f32 v[3];

    em->x40C = 5;
    em->x9EA = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            Quest_enemy_capture();
        }
        break;
    case 2:
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.6f);
    if (++em->work08 % 135 == 0) {
        sound_call_sub_00574CD0(em, 0x57, 0x23);
    }
}

void em_die00_005730C0(EMW *em, EM01W *w) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x44, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        if (em->kind == 0xB) {
            wyvern_kill_cnt_up(User_data, 1);
        } else {
            wyvern_kill_cnt_up(User_data, 0);
        }
        break;
    case 1:
        if (em_frame_check(em, 212.0f, 0)) {
            Em_set_quake_sub(em, 2);
        }
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05++;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 2:
        Em_hagi_point_cnt_ck(em);
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em01_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_die01_00573290(EMW *em, EM01W *w) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x61, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        em->work08 = 0xE10;
        em->x9EA = 5;
        if (em->kind == 0xB) {
            wyvern_kill_cnt_up(User_data, 1);
        } else {
            wyvern_kill_cnt_up(User_data, 0);
        }
        break;
    case 1:
        em->x9EA = 5;
        em->work08 -= 1;
        if (em->x194 == 0) {
            em->x05++;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 2:
        Em_hagi_point_cnt_ck(em);
        if (em->work08 != 0 && em->x9EA != 0) {
            em->x9EA = 5;
            if (--em->work08 <= 0) {
                em->x9EA = 0;
            }
        }
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em01_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_die02_00573480(EMW *em, EM01W *w) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x4D, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x4E, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em_frame_check(em, 60.0f, 0)) {
            em->x05++;
            em->work08 = 0;
            em_char_set(em, 0x52, 0, 0);
            em->x388 = 3;
            Quest_enemy_die(em);
            if (em->kind == 0xB) {
                wyvern_kill_cnt_up(User_data, 1);
            } else {
                wyvern_kill_cnt_up(User_data, 0);
            }
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05++;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 4:
        Em_hagi_point_cnt_ck(em);
        break;
    case 5:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em01_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_move00_00573730(EMW *em, EM01W *w) {
    switch (em->x15) {
    case 0: em_act00_00567280(em, w); break;
    case 1: em_act01_00567350(em, w); break;
    case 2: em_act02_00567470(em, w); break;
    case 3: em_act03_00567530(em, w); break;
    case 4: em_act04_00567600(em, w); break;
    case 5: em_act05_005676C0(em, w); break;
    case 6: em_act06_005677B0(em, w); break;
    case 7: em_act07_005678A0(em, w); break;
    case 8: em_act08_00567970(em, w); break;
    case 9: em_act09_00567A90(em, w); break;
    case 10: em_act10_00567B30(em, w); break;
    case 11: em_act11_00567C40(em, w); break;
    case 12: em_act12_00567D30(em, w); break;
    case 13: em_act13_00567DB0(em, w); break;
    case 14: em_act14_00567EA0(em, w); break;
    case 15: em_act15_00568220(em, w); break;
    case 16: em_act16_00568350(em, w); break;
    case 17: em_act17_00568480(em, w); break;
    case 18: em_act18_00568500(em, w); break;
    case 19: em_act19_005686B0(em, w); break;
    case 20: em_act20_005688F0(em, w); break;
    case 21: em_act21_00568A00(em, w); break;
    case 22: em_act22_00568BB0(em, w); break;
    case 23: em_act23_00568C30(em, w); break;
    case 24: em_act24_00568D00(em, w); break;
    case 25: em_act25_00568DD0(em, w); break;
    case 26: em_act26_00568E50(em, w); break;
    case 27: em_act27_00568F90(em, w); break;
    case 28: em_act28_005690A0(em, w); break;
    case 29: em_act29_00569170(em, w); break;
    case 31: em_act31_005692B0(em, w); break;
    case 33: em_act33_005693F0(em, w); break;
    case 40: em_act40_00568830(em, w); break;
    }
}

void em_move01_00573980(EMW *em, EM01W *w) {
    switch (em->x15) {
    case 0: em_mv00_00569480(em, w); break;
    case 1: em_mv01_005695C0(em, w); break;
    case 2: em_mv02_005695D0(em, w); break;
    case 3: em_mv03_005695E0(em, w); break;
    case 4: em_mv04_005698F0(em, w); break;
    case 5: em_mv05_00569A50(em, w); break;
    case 6: em_mv06_00569D60(em, w); break;
    case 7: em_mv07_00569EC0(em, w); break;
    case 8: em_mv04_005698F0(em, w); break;
    }
}

void em_move02_00573A50(EMW *em, EM01W *w) {
    switch (em->x15) {
    case 0: em_fly00_0056A000(em, w); break;
    case 1: em_fly01_0056A100(em, w); break;
    case 2: em_fly02_0056A340(em, w); break;
    case 3: em_fly03_0056A450(em, w); break;
    case 4: em_fly04_0056A6E0(em, w); break;
    case 5: em_fly05_0056A850(em, w); break;
    case 6: em_fly06_0056A9C0(em, w); break;
    case 7: em_fly07_0056ABE0(em, w); break;
    case 8: em_fly08_0056AD20(em, w); break;
    case 9: em_fly09_0056AFC0(em, w); break;
    case 10: em_fly10_0056B190(em, w); break;
    case 11: em_fly11_0056B390(em, w); break;
    case 12: em_fly12_0056B5E0(em, w); break;
    case 13: em_fly13_0056B740(em, w); break;
    case 14: em_fly14_0056BAC0(em, w); break;
    case 15: em_fly15_0056BB80(em, w); break;
    case 16: em_fly16_0056BC40(em, w); break;
    case 17: em_fly17_0056BD50(em, w); break;
    case 18: em_fly18_0056BF90(em, w); break;
    case 19: em_fly19_0056C160(em, w); break;
    case 20: em_fly20_0056C340(em, w); break;
    case 21: em_fly21_0056C490(em, w); break;
    case 22: em_fly22_0056C5D0(em, w); break;
    case 23: em_fly23_0056C740(em, w); break;
    case 24: em_fly24_0056C980(em, w); break;
    }
}

void em_move03_00573C20(EMW *em, EM01W *w) {
    switch (em->x15) {
    case 0: em_atk00_0056CAA0(em, w); break;
    case 2: em_atk02_0056CB40(em, w); break;
    case 3: em_atk03_0056CDC0(em, w); break;
    case 4: em_atk04_0056CE50(em, w); break;
    case 5: em_atk05_0056D040(em, w); break;
    case 6: em_atk06_0056D0F0(em, w); break;
    case 7: em_atk07_0056D1E0(em, w); break;
    case 8: em_atk08_0056D300(em, w); break;
    case 9: em_atk09_0056DD30(em, w); break;
    case 10: em_atk10_0056DF20(em, w); break;
    case 11: em_atk11_0056E160(em, w); break;
    case 12: em_atk12_0056E6F0(em, w); break;
    case 13: em_atk13_0056E860(em, w); break;
    case 14: em_atk14(em, w); break;
    case 15: em_atk15_0056E970(em, w); break;
    case 16: em_atk16_0056EAD0(em, w); break;
    case 17: em_atk17_0056EC80(em, w); break;
    case 18: em_atk18_0056EF60(em, w); break;
    case 19: em_atk19(em, w); break;
    case 20: em_atk20(em, w); break;
    case 21: em_atk21_0056F340(em, w); break;
    case 22: em_atk22(em, w); break;
    case 23: em_atk23(em, w); break;
    case 24: em_atk24(em, w); break;
    case 25: em_atk25(em, w); break;
    }
}

void em_move04_00573DF0(EMW *em, EM01W *w) {
    switch (em->x15) {
    case 0: em_dmg00_00570530(em, w); break;
    case 1: em_dmg01_005705C0(em, w); break;
    case 2: em_dmg02_00570650(em, w); break;
    case 3: em_dmg03_005706E0(em, w); break;
    case 4: em_dmg04_005707A0(em, w); break;
    case 5: em_dmg05_00570970(em, w); break;
    case 6: em_dmg06_00570A90(em, w); break;
    case 7: em_dmg07_00570AA0(em, w); break;
    case 8: em_dmg08_00570AB0(em, w); break;
    case 9: em_dmg09_00570C30(em, w); break;
    case 10: em_dmg10_00570DC0(em, w); break;
    case 11: em_dmg11_00570E50(em, w); break;
    case 12: em_dmg12_00570F20(em, w); break;
    case 13: em_dmg13_00571050(em, w); break;
    case 14: em_dmg14_00571180(em, w); break;
    case 15: em_dmg15_00571290(em, w); break;
    case 16: em_dmg16_00571310(em, w); break;
    case 17: em_dmg17_00571430(em, w); break;
    case 18: em_dmg18_00571550(em, w); break;
    case 19: em_dmg19_00571650(em, w); break;
    }
}

void em_move05_00573F70(EMW *em, EM01W *w) {
    switch (em->x15) {
    case 0:
        em_die00_005730C0(em, w);
        break;
    case 1:
        em_die01_00573290(em, w);
        break;
    case 2:
        em_die02_00573480(em, w);
        break;
    }
}

void em_move06_00573FE0(EMW *em, EM01W *w) {
    switch (em->x15) {
    case 0: em_demo00_00571750(em, w); break;
    case 1: em_demo01_005720E0(em, w); break;
    case 2: em_demo02_00572660(em, w); break;
    case 3: em_demo00_00571750(em, w); break;
    case 4: em_demo04_00572FA0(em, w); break;
    case 5: em_demo02_00572660(em, w); break;
    }
}

void em01_main(EMW *em) {
    EM01W *w = (EM01W *)em->ex;
    u8 dmg[4];
    f32 rate;
    int r;
    int d;

    em_mode_timer_sub(em);
    em_no_floor_ck(em);
    if (em->kind == 1) {
        rate = 0.1f;
    } else {
        rate = 0.3f;
    }
    if (em->x8C2 != 1 && em->x8B6 == 0) {
        switch (quest_w.x08) {
        case 0x2C:
        case 0x2D:
        case 0x5E:
        case 0x5F:
            if (em->kind == 1) {
                em_hinshi_ck(em, rate);
                em_egg_ck(em);
                em_thirst_ck(em);
                em_sleep_ck(em);
            } else {
                em_egg_ck(em);
                em_thirst_ck(em);
                em_hungry_ck(em);
            }
            break;
        case 0x60:
            if (em->kind == 1) {
                em_hinshi_ck(em, rate);
                em_sleep_ck(em);
            } else {
                em_thirst_ck(em);
            }
            break;
        case 0x62:
            em_hinshi_ck(em, rate);
            em_egg_ck(em);
            break;
        case 0x64:
        case 0xAA:
            if (em->kind != 1) {
                em_thirst_ck(em);
            }
            break;
        default:
            em_hinshi_ck(em, rate);
            em_egg_ck(em);
            em_thirst_ck(em);
            em_hungry_ck(em);
            em_sleep_ck(em);
            break;
        }
    }
    if (w->x06 != 0) {
        w->x06--;
    }
    r = Em_Dmg_Sys(em, dmg);
    switch (r) {
    case 0:
    case 9:
    case 14:
        break;
    case 1:
    case 2:
        if (em->x388 == 2) {
            em01_act_set(em, 5, 2, 2);
        } else if (em->x9EA != 0) {
            em01_act_set(em, 5, 1, 2);
        } else {
            em01_act_set(em, 5, 0, 2);
        }
        break;
    case 3:
    case 4:
        if (em->x9EA == 0) {
            if (dmg[0] == 0) {
                em->x95A = 0x10;
            } else if (em->x8B6 == 0) {
                em->x95A = 0xA;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em01_act_set(em, 4, 0xC, 2);
        }
        break;
    case 5:
        if (em->mode != 4 || em->x15 != 8) {
            em01_act_set(em, 4, 8, 2);
        }
        break;
    case 6:
        if (em->x9EA != 0) {
            em_mahi_dmg_timer_set(em);
            em01_act_set(em, 4, 0xE, 2);
        } else if (!(em->mode == 4 && em->x15 == 0xB) && !(em->mode == 4 && em->x15 == 8)) {
            em_mahi_dmg_timer_set(em);
            em01_act_set(em, 4, 0xB, 2);
        }
        break;
    case 7:
        if (em->x9EA != 0) {
            em_sleep2_dmg_timer_set(em);
            if ((u8)em_hokaku_ck(em, 0.1f + ((em->kind == 1) ? 0.1f : 0.3f)) == 1) {
                em01_act_set(em, 6, 4, 4);
            } else {
                em01_act_set(em, 0, 0x1F, 2);
            }
        } else if (!(em->mode == 0 && em->x15 == 0x1B) && !(em->mode == 4 && em->x15 == 8)) {
            em_sleep2_dmg_timer_set(em);
            em01_act_set(em, 0, 0x1B, 2);
        }
        break;
    case 8:
        if (em->x9EA != 0) {
            em_sleep_dmg_timer_set(em);
            em01_act_set(em, 0, 0x1D, 2);
        } else if (!(em->mode == 0 && em->x15 == 0x14) && !(em->mode == 4 && em->x15 == 8)) {
            em_sleep_dmg_timer_set(em);
            em01_act_set(em, 0, 0x14, 2);
        }
        break;
    case 10:
        switch (em->x15) {
        case 18:
            em01_act_set(em, 0, 0x17, 2);
            em->x839 = 0;
            em_ikari_add(em, em->x8B0);
            break;
        case 20:
            em01_act_set(em, 0, 0x18, 2);
            em->x839 = 0;
            break;
        case 27:
            em01_act_set(em, 0, 0x1C, 2);
            em->x839 = 0;
            break;
        case 29:
            em01_act_set(em, 4, 0x12, 2);
            em->x839 = 0;
            break;
        case 31:
            em01_act_set(em, 4, 0x13, 2);
            em->x839 = 0;
            break;
        }
        break;
    case 11:
        em01_act_set(em, 4, 4, 2);
        break;
    case 12:
        if (em->x388 == 2) {
            em01_act_set(em, 4, 8, 2);
        } else {
            d = em->x38E;
            switch (d) {
            case 0:
            case 7:
                em01_act_set(em, 4, 0, 2);
                break;
            case 6:
                if (em->hagi[d].cnt >= 2) {
                    if (em->kind == 1) {
                        Quest_enemy_hagi_set(em, 1);
                    } else {
                        Quest_enemy_hagi_set(em, 4);
                    }
                }
                /* fallthrough */
            case 5:
                em01_act_set(em, 4, 2, 2);
                break;
            case 1:
            case 2:
                em01_act_set(em, 4, 3, 2);
                break;
            default:
                if (em->hagi[d & 0xFF].cnt >= 2) {
                    if (d != 3) {
                        em01_act_set(em, 4, 5, 2);
                    } else {
                        em01_act_set(em, 4, 0x10, 2);
                    }
                } else {
                    em01_act_set(em, 4, 1, 2);
                }
                break;
            }
        }
        break;
    case 13:
        if (em->x388 != 2) {
            em01_act_set(em, 4, 0, 2);
            em->x839 = 0;
        }
        break;
    }
    switch (quest_w.x08) {
    case 0x8A:
        if (em->stg == 0x25 && Event_flag_ck(0xD) == 0) {
            if (game_w.info_stop == 1 && em->mode != 6) {
                em01_act_set(em, 6, 1, 1);
            }
            break;
        }
        goto cmd;
    case 0x8B:
        if (em->stg == 0x21 && Event_flag_ck(0xE) == 0) {
            if (game_w.info_stop == 1 && em->mode != 6) {
                em01_act_set(em, 6, 2, 1);
            }
            break;
        }
        goto cmd;
    default:
    cmd:
        switch (em->x734) {
        case 3:
            if (em->x839 != 0) {
                em_cmd_ck(em);
                em->x839 = 0;
            }
            break;
        default:
            break;
        }
        break;
    }
    em01_main_sub(em, w);
    if (em->x6FF != 0) {
        em01_main_sub(em, w);
        em->x6FF = 0;
    }
    if (em->mode == 2 && w->x18 == 1) {
        em->x8BB = 2;
    }
}

void em01_main_sub(EMW *em, EM01W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->mode) {
    case 0: em_move00_00573730(em, w); break;
    case 1: em_move01_00573980(em, w); break;
    case 2: em_move02_00573A50(em, w); break;
    case 3: em_move03_00573C20(em, w); break;
    case 4: em_move04_00573DF0(em, w); break;
    case 5: em_move05_00573F70(em, w); break;
    case 6: em_move06_00573FE0(em, w); break;
    case 7: em_move06_00573FE0(em, w); break;
    }
    if (em->pos[0] <= 0.0f || em->pos[2] <= 0.0f) {
        em_dur_set(em, 0);
    }
}
