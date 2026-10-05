/* em01 AI, run 2: em_act00_00567280 .. em_mv02_005695D0 (game.bin 0x00567280-0x005695DC). Matching functions of em01_ai_nm.c (that file holds the
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

void em_act00_00567280(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        if (em->char0 != 0x3E9) {
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
        }
        if (em->x2DE != 0x4B1) {
            em_char_set2(em, 0x4B1, 0xA, 0, 1);
        }
        if (em->x2E0 != 0x579) {
            em_char_set2(em, 0x579, 0xA, 0, 2);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x839 = 1;
        }
        break;
    }
}

void em_act01_00567350(EMW *em, EM01W *w) {
    u8 t;
    u16 a;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1D, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            t = em->x734;
            if (t == 1 || t == 2 || t == 3) {
                if (em->x194 == 0) {
                    em->x05++;
                    act_dist_select_00566CD0(em);
                }
            } else {
                a = em_act_search(em01_act_add[EMF(em, u8, 0x735)]);
                if (a != 1) {
                    em01_act_set(em, 0, a, 0);
                }
            }
        }
        break;
    }
}

void em_act02_00567470(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x17, 0, 0);
        em01_reset_char_set(em, 0);
        em01_reset_char_set(em, 1);
        break;
    case 1:
        if (EMF(em, s32, 0x234) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 0);
    em01_frame_reset(em, 1);
}

void em_act03_00567530(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x17, 0, 0);
        em01_reset_char_set(em, 0);
        em01_reset_char_set(em, 1);
        break;
    case 1:
        if (EMF(em, s32, 0x234) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 0);
    em01_frame_reset(em, 1);
    sound_call_parts_00574DA0(em, 0x4E, 0x17, 0x2A, 2);
}

void em_act04_00567600(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        em01_reset_char_set(em, 1);
        em01_reset_char_set(em, 2);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 1);
    em01_frame_reset(em, 2);
}

void em_act05_005676C0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x18, 0, 0);
        em_char_set2(em, 0x3E9, 0xA, 0, 0);
        em_char_set2(em, 0x579, 0xA, 0, 2);
        break;
    case 1:
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 0);
    em01_frame_reset(em, 2);
    sound_call_parts_00574DA0(em, 0x20, 0x2F, 0x23, 1);
}

void em_act06_005677B0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        em01_reset_char_set(em, 0);
        em01_reset_char_set(em, 2);
        break;
    case 1:
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 0);
    em01_frame_reset(em, 2);
    sound_call_parts_00574DA0(em, 0x20, 0x20, 0x23, 1);
    sound_call_parts_00574DA0(em, 0xA0, 0x17, 0x23, 1);
}

void em_act07_005678A0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1A, 0, 0);
        em01_reset_char_set(em, 0);
        em01_reset_char_set(em, 2);
        break;
    case 1:
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 0);
    em01_frame_reset(em, 2);
    sound_call_parts_00574DA0(em, 4, 0x2D, 0x23, 1);
}

void em_act08_00567970(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1B, 0, 0);
        em01_reset_char_set(em, 0);
        em01_reset_char_set(em, 1);
        break;
    case 1:
        if (em_frame_check(em, 26.0f, 2)) {
            shell01_set(em, 0x29);
        }
        if (em_frame_check(em, 90.0f, 2)) {
            shell01_set(em, 0x29);
        }
        if (EMF(em, s32, 0x234) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 0);
    em01_frame_reset(em, 1);
    sound_call_parts_00574DA0(em, 0x10, 0x16, 0x2A, 2);
}

void em_act09_00567A90(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        if (em->x39A & 1) {
            em_char_set(em, 0x50, 0, 0);
        } else {
            em_char_set(em, 0x51, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act10_00567B30(EMW *em, EM01W *w) {
    u16 a;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x36, 0, 0);
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        a = w->dang;
        if ((u32)(u16)((u16)(a - (u16)em->ang[1]) + 0x200) < 0x400U) {
            em->ang[1] = a;
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

void em_act11_00567C40(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1C, 0, 0);
        em01_reset_char_set(em, 0);
        em01_reset_char_set(em, 2);
        break;
    case 1:
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 0);
    em01_frame_reset(em, 2);
    sound_call_parts_00574DA0(em, 0x10, 0x21, 0x23, 1);
    sound_call_parts_00574DA0(em, 0x72, 0x1F, 0x23, 1);
}

void em_act12_00567D30(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1D, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act13_00567DB0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x14, 0, 0);
        em01_reset_char_set(em, 0);
        em01_reset_char_set(em, 2);
        break;
    case 1:
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em01_frame_reset(em, 0);
    em01_frame_reset(em, 2);
    sound_call_parts_00574DA0(em, 2, 0x20, 0x23, 1);
    sound_call_parts_00574DA0(em, 0x32, 0x1F, 0x23, 1);
}

void em_act14_00567EA0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4F, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1);
        em_range_set(em, 1);
        break;
    case 1:
        if (em_frame_check(em, 56.0f, 0) || em_frame_check(em, 108.0f, 0) || em_frame_check(em, 126.0f, 0) ||
            em_frame_check(em, 138.0f, 0) || em_frame_check(em, 170.0f, 0)) {
            sound_call_sub_00574CD0(em, 0x26, 0x23);
            Eft20_set(5.0f, em, 7, 0);
        }
        if (em_frame_check(em, 210.0f, 0) || em_frame_check(em, 218.0f, 0)) {
            Eft20_set(2.5f, em, 7, 0);
        }
        if (em_frame_check(em, 52.0f, 0)) {
            Eft20_set(1.0f, em, 8, 0);
        }
        if (em_frame_check(em, 106.0f, 0) || em_frame_check(em, 130.0f, 0)) {
            Eft20_set(1.0f, em, 6, 0);
        }
        if (em_frame_check(em, 124.0f, 0)) {
            Eft20_set(1.0f, em, 6, 1);
        }
        if (em_frame_check(em, 170.0f, 0)) {
            Eft20_set(1.0f, em, 0xF, 0);
        }
        if (em_frame_check(em, 192.0f, 0)) {
            Eft20_set(1.0f, em, 0x10, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3B, 0, 0);
            em_hp_add(em, (s16)(0.05f * (f32)em->x792));
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->x762 = 0;
            em_search_data_set(em, 0);
            em_range_set(em, 0);
            em_thirst_end(em);
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    em_thirst_add(em, 0xDE);
}

void em_act15_00568220(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x50, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05++;
            em_char_set(em, 0x51, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 4:
        if (em->x1C4 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act16_00568350(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x51, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05++;
            em_char_set(em, 0x50, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 4:
        if (em->x1C4 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act17_00568480(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x33, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act18_00568500(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x20, 0, 0);
        em->x88B = 0;
        em_range_set(em, 1);
        Em_Suimin_Start(em);
        if (em->kind == 1) {
            em->work08 = 0x3138;
        } else {
            em->work08 = 0x2328;
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x1F, 0, 0);
        }
        break;
    case 2:
        if (em->kind == 1) {
            em_sleep_hp_add(em, 1, (s16)(0.6f * (f32)em->x792), 3);
        } else {
            em_sleep_hp_add(em, 1, (s16)(0.7f * (f32)em->x792), 3);
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em_hinshi_end(em);
            em01_act_set(em, 0, 0x17, 4);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, 0x17, 4);
        }
        break;
    }
}

void em_act19_005686B0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4F, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1);
        em_range_set(em, 1);
        break;
    case 1:
        if (em_frame_check(em, 200.0f, 0)) {
            em->x05++;
            if (em->x8C3 == 0) {
                em->x827 = 7;
                em->x828 = em->x951;
                em->x829 = em->x952;
                EMF(em, u8, 0xA00) = 1;
                em_niku_eat_set(em);
                em_hungry_add(em, 0x2710);
                cmd_target_kind_set(em, em->tgt_pos);
            }
            em01_act_set(em, 0, 0x28, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em->x827 = 7;
            em->x828 = em->x951;
            em->x829 = em->x952;
            cmd_target_kind_set(em, em->tgt_pos);
            EMF(em, u8, 0xA00) = 1;
            em_niku_eat_set(em);
            em_hungry_add(em, 0x2710);
            em01_act_set(em, 0, 0x28, 4);
        }
        break;
    }
}

void em_act40_00568830(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x762 = 0;
        em_search_data_set(em, 0);
        em_range_set(em, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3B, 0, 0);
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

void em_act20_005688F0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x26, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em01_act_set(em, 0, 0x18, 4);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

void em_act21_00568A00(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x27, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        if (em->kind == 1) {
            em->work08 = 0x3138;
        } else {
            em->work08 = 0x2328;
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        if (em->kind == 1) {
            em_sleep_hp_add(em, 1, (s16)(0.6f * (f32)em->x792), 3);
        } else {
            em_sleep_hp_add(em, 1, (s16)(0.7f * (f32)em->x792), 3);
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em_hinshi_end(em);
            em01_act_set(em, 0, 0x18, 4);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

void em_act22_00568BB0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x50, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act23_00568C30(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x21, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        em_suimin_end(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

void em_act24_00568D00(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x25, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

void em_act25_00568DD0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act26_00568E50(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4F, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1);
        em_range_set(em, 1);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3B, 0, 0);
            em_hp_add(em, (s16)(0.05f * (f32)em->x792));
            em_hungry_add(em, em->hungry_max);
            em_hungry_end(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->x762 = 0;
            em_search_data_set(em, 0);
            em_range_set(em, 0);
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act27_00568F90(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x26, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em01_act_set(em, 0, 0x1C, 4);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, 0x1C, 4);
        }
        break;
    }
}

void em_act28_005690A0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x25, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

void em_act29_00569170(EMW *em, EM01W *w) {
    f32 v[3];

    em->x9EA = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em01_act_set(em, 4, 0x12, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 4, 0x12, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.6f);
    if (em->work08 % 135 == 0) {
        sound_call_sub_00574CD0(em, 0x57, 0x23);
    }
}

void em_act31_005692B0(EMW *em, EM01W *w) {
    f32 v[3];

    em->x9EA = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em01_act_set(em, 4, 0x13, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em01_act_set(em, 4, 0x13, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.6f);
    if (em->work08 % 135 == 0) {
        sound_call_sub_00574CD0(em, 0x57, 0x23);
    }
}

void em_act33_005693F0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_mv00_00569480(EMW *em, EM01W *w) {
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM01_TURN(em);
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_mv01_005695C0(EMW *em, EM01W *w) {
    em01_to_normal(em, 0, 0);
}

void em_mv02_005695D0(EMW *em, EM01W *w) {
    em01_to_normal(em, 0, 0);
}

/* Walk-turn toward the target; mode 0 turns left (anims 5/6), mode 1 right (7/8). */
void em_mv03_005695E0(EMW *em, EM01W *w) {
    u32 spd;
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        d = (w->dang - em->ang[1]) & 0xFFFF;
        if (d <= 0xE38U || d >= 0xF1C8U) {
            pl_flag_set((PLW *)em, 0x20000);
            em_char_set(em, 3, 0, 0);
        } else if (d >= 0x8000U) {
            em_char_set(em, 6, 0, 0);
        } else {
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            spd = (u32)((16384.0f / (em->x1A8 / 2.0f)) * em->act_spd);
            d = (w->dang - (u16)em->ang[1]) & 0xFFFF;
            if (em->x194 == 0 || em_frame_check2(em, 0, 110.0f)) {
                if ((u32)((d + spd) & 0xFFFF) < spd * 2) {
                    em->x05++;
                    pl_flag_clr((PLW *)em, 0x20000);
                    em01_to_normal(em, 0, 0);
                } else if (d <= 0xE38U || d >= 0xF1C8U) {
                    pl_flag_set((PLW *)em, 0x20000);
                    em_char_set(em, 3, 0, 0);
                } else {
                    pl_flag_clr((PLW *)em, 0x20000);
                    if (d >= 0x8000U) {
                        em_char_set(em, 6, 0, 0);
                    } else {
                        em_char_set(em, 5, 0, 0);
                    }
                }
                break;
            }
            if ((u32)((d + spd) & 0xFFFF) < spd * 2) {
                em->ang[1] = (u16)w->dang;
            } else if (d < 0x8000U) {
                em->ang[1] = (em->ang[1] + spd) & 0xFFFF;
            } else {
                em->ang[1] = (em->ang[1] - spd) & 0xFFFF;
            }
        }
        break;
    }
}
