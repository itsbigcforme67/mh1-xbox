/* em01 AI, run 7 (with em01_uvmove 0x00574AD0 as a static before its callers, and em01_effect_move after ef_move_sub: one translation unit, as in the original): sound_call_sub_00574CD0 .. ef_move_sub_00574EE0 (game.bin 0x00574AD0-0x0057A7E0). Matching functions of em01_ai_nm.c (that file holds the
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
#define UVR(i) \
    do { \
        em->uv[i][0] = 0.0f; \
        em->uv[i][1] = 0.0f; \
        em->uvtm[i] = 0xFFFF; \
        em->uvty[i] = 0xFF; \
    } while (0)

static void em01_uvmove(EMW *em) {
    int i;

    for (i = 0; i < 4; i++) {
        if (em->uvtm[i] != 0xFFFF) {
            em->uvtm[i]++;
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

void sound_call_sub_00574CD0(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

void sound_call_00574D40(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, (f32)frame, 0)) {
        sound_call_sub_00574CD0(em, se, joint);
    }
}

void sound_call_parts_00574DA0(EMW *em, int frame, int se, int joint, u8 mode) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, mode)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}

void quake_call_00574E40(EMW *em, int frame, int v) {
    if (em_frame_check(em, (f32)frame, 0)) {
        Em_set_quake_sub(em, v);
    }
}

void move_default_00574E90(EMW *em) {
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

/* Sound and effect script per animation (sound_call(em, frame, se, joint) plays a sound at the joint once the
 * animation reaches the frame). Generated from the asm and checked with check.py. */
void ef_move_sub_00574EE0(EMW *em, EM01W *w) {
    f32 va[4];
    f32 vb[4];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
    case 0x3F8:
    case 0x406:
        break;
    case 0x3EB:
        sound_call_00574D40(em, 0x34, 1, 0x14);
        sound_call_00574D40(em, 0x74, 1, 0x1a);
        quake_call_00574E40(em, 0x34, 1);
        quake_call_00574E40(em, 0x74, 1);
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 2);
        }
        if (em_frame_check(em, 50.0f, 0)) {
            shell01_set(em, 3);
        }
        if (em_frame_check(em, 120.0f, 0)) {
            shell01_set(em, 4);
        }
        break;
    case 0x3EC:
        sound_call_00574D40(em, 0x28, 6, 0x14);
        sound_call_00574D40(em, 0x4c, 6, 0x1a);
        if (em_frame_check(em, 12.0f, 0)) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if (em_frame_check(em, 52.0f, 0)) {
            Eft20_set(1.0f, em, 3, 0);
        }
        if (em_frame_check(em, 92.0f, 0)) {
            Eft20_set(1.0f, em, 2, 0);
        }
        break;
    case 0x3ED:
        sound_call_00574D40(em, 0x19, 1, 0x14);
        sound_call_00574D40(em, 0x38, 1, 0x1a);
        quake_call_00574E40(em, 0x3c, 1);
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 5);
        }
        if (em_frame_check(em, 30.0f, 0)) {
            shell01_set(em, 6);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 2.0f)) {
                if (!(em_frame_check2(em, 0, 78.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, 1);
                    }
                }
            }
        }
        break;
    case 0x3EE:
        sound_call_00574D40(em, 0x19, 1, 0x1a);
        sound_call_00574D40(em, 0x38, 1, 0x14);
        quake_call_00574E40(em, 0x3c, 1);
        if (em_frame_check(em, 30.0f, 0)) {
            shell01_set(em, 7);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 8);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 2.0f)) {
                if (!(em_frame_check2(em, 0, 78.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, 0);
                    }
                }
            }
        }
        break;
    case 0x3EF:
        sound_call_00574D40(em, 6, 0x20, 0x23);
        sound_call_00574D40(em, 0xe, 1, 0x1a);
        sound_call_00574D40(em, 0x26, 1, 0x14);
        if (em_frame_check(em, 18.0f, 0)) {
            shell01_set(em, 9);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0xa);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 2.0f)) {
                if (!(em_frame_check2(em, 0, 18.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, 1);
                    }
                }
            }
            if (em_frame_check2(em, 0, 20.0f)) {
                if (!(em_frame_check2(em, 0, 70.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, 0);
                    }
                }
            }
        }
        break;
    case 0x3F0:
        sound_call_00574D40(em, 6, 0x20, 0x23);
        sound_call_00574D40(em, 0xe, 1, 0x14);
        sound_call_00574D40(em, 0x26, 1, 0x1a);
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0xb);
        }
        if (em_frame_check(em, 18.0f, 0)) {
            shell01_set(em, 0xc);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 2.0f)) {
                if (!(em_frame_check2(em, 0, 18.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, 0);
                    }
                }
            }
            if (em_frame_check2(em, 0, 20.0f)) {
                if (!(em_frame_check2(em, 0, 70.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, 1);
                    }
                }
            }
        }
        break;
    case 0x3F2:
        sound_call_00574D40(em, 6, 0x31, 0x23);
        sound_call_00574D40(em, 6, 4, 0x1a);
        sound_call_00574D40(em, 0x4c, 0, 0x14);
        sound_call_00574D40(em, 0x1c, 0x12, 0x14);
        if (em_frame_check(em, 42.0f, 0)) {
            shell01_set(em, 0x19);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0x1a);
        }
        if (em_frame_check(em, 74.0f, 0)) {
            shell01_set(em, 0x1a);
        }
        break;
    case 0x3F3:
        sound_call_00574D40(em, 0xe, 0xb, 6);
        sound_call_00574D40(em, 0x12, 0xb, 0xc);
        sound_call_00574D40(em, 0x46, 0xb, 6);
        sound_call_00574D40(em, 0x42, 0xb, 0xc);
        sound_call_00574D40(em, 0x7a, 0xb, 6);
        sound_call_00574D40(em, 0x7e, 0xb, 0xc);
        if (em_frame_check(em, 52.0f, 0) || em_frame_check(em, 104.0f, 0) || em_frame_check(em, 162.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft13_set_em_scl(em, 2, 5.0f, 7);
            }
        }
        break;
    case 0x3F4:
        sound_call_00574D40(em, 6, 0xc, 6);
        sound_call_00574D40(em, 4, 0xc, 0xc);
        sound_call_00574D40(em, 0x40, 0xc, 6);
        sound_call_00574D40(em, 0x3c, 0xc, 0xc);
        sound_call_00574D40(em, 4, 0x1a, 0x22);
        sound_call_00574D40(em, 6, 0x19, 0xc);
        sound_call_00574D40(em, 0xa, 0x19, 6);
        sound_call_00574D40(em, 0x24, 0x1a, 0x22);
        sound_call_00574D40(em, 0x26, 0x19, 0xc);
        sound_call_00574D40(em, 0x28, 0x19, 6);
        sound_call_00574D40(em, 0x46, 0x1a, 0x22);
        sound_call_00574D40(em, 0x48, 0x19, 0xc);
        sound_call_00574D40(em, 0x4c, 0x19, 6);
        break;
    case 0x3F5:
        sound_call_00574D40(em, 0xe, 0xf, 6);
        sound_call_00574D40(em, 0x12, 0xf, 0xc);
        sound_call_00574D40(em, 0x2c, 0xf, 6);
        sound_call_00574D40(em, 0x28, 0xf, 0xc);
        sound_call_00574D40(em, 0x42, 0xf, 6);
        sound_call_00574D40(em, 0x46, 0xf, 0xc);
        if (em_frame_check(em, 26.0f, 0) || em_frame_check(em, 52.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft13_set_em_scl(em, 1, 5.0f, 7);
            }
        }
        break;
    case 0x3F6:
    case 0x43D:
    case 0x43E:
    case 0x440:
    case 0x441:
        sound_call_00574D40(em, 0xa, 0x1a, 0x22);
        sound_call_00574D40(em, 0xe, 0x19, 0xc);
        sound_call_00574D40(em, 0x12, 0x19, 6);
        sound_call_00574D40(em, 0x2a, 0x19, 0x22);
        sound_call_00574D40(em, 0x2e, 0x19, 0xc);
        sound_call_00574D40(em, 0x32, 0x19, 6);
        if (em->x8B6) {
            if (!(*(u16 *)&game_w.x1E % 19)) {
                Eft20_set(1.0f, em, 0x19, (s16)(((u16)ran_suu(1) & 0x1) + 2));
            }
        }
        break;
    case 0x3F7:
        sound_call_00574D40(em, 4, 0xc, 6);
        sound_call_00574D40(em, 8, 0xc, 0xc);
        sound_call_00574D40(em, 0x3a, 0xb, 6);
        sound_call_00574D40(em, 0x3e, 0xb, 0xc);
        sound_call_00574D40(em, 0x8e, 0xb, 6);
        sound_call_00574D40(em, 0x8a, 0xb, 0xc);
        sound_call_00574D40(em, 0xd4, 0xb, 6);
        sound_call_00574D40(em, 0xd8, 0xb, 0xc);
        if (em_frame_check(em, 8.0f, 0) || em_frame_check(em, 86.0f, 0) || em_frame_check(em, 160.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6) {
            if ((em_frame_check2(em, 0, 70.0f) && !(em_frame_check(em, 84.0f, 0))) || (em_frame_check2(em, 0, 146.0f) && !(em_frame_check(em, 156.0f, 0))) || (em_frame_check2(em, 0, 220.0f) && !(em_frame_check(em, 234.0f, 0)))) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3F9:
        sound_call_00574D40(em, 6, 0x27, 0x23);
        sound_call_00574D40(em, 0x1c, 6, 0x14);
        sound_call_00574D40(em, 0x38, 6, 0x1a);
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0x18);
        }
        if (em->x8B6) {
            if (!(*(u16 *)&game_w.x1E % 10)) {
                Eft20_set(1.0f, em, 0x19, (s16)(((u16)ran_suu(1) & 0x1) + 2));
            }
        }
        if (em_frame_check(em, 12.0f, 0) || em_frame_check(em, 78.0f, 0)) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if (em_frame_check(em, 42.0f, 0)) {
            Eft20_set(1.0f, em, 3, 0);
        }
        break;
    case 0x3FA:
        sound_call_00574D40(em, 0x28, 7, 0x14);
        sound_call_00574D40(em, 0x1a, 0xd, 6);
        sound_call_00574D40(em, 0x1e, 0xd, 0xc);
        sound_call_00574D40(em, 0x52, 0xb, 6);
        sound_call_00574D40(em, 0x56, 0xb, 0xc);
        sound_call_00574D40(em, 0x88, 0xb, 6);
        sound_call_00574D40(em, 0x8c, 0xb, 0xc);
        sound_call_00574D40(em, 0xb6, 0xb, 6);
        sound_call_00574D40(em, 0xba, 0xb, 0xc);
        if (em_frame_check(em, 42.0f, 0) || em_frame_check(em, 102.0f, 0) || em_frame_check(em, 156.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6) {
            if (em_frame_check(em, 62.0f, 0)) {
                Eft20_set(1.0f, em, 0x19, 4);
                Eft20_set(1.0f, em, 0x19, 5);
            }
        }
        break;
    case 0x3FB:
        sound_call_00574D40(em, 2, 0xb, 6);
        sound_call_00574D40(em, 6, 0xb, 0xc);
        sound_call_00574D40(em, 0xc, 4, 0x1a);
        sound_call_00574D40(em, 4, 1, 0x14);
        sound_call_00574D40(em, 0x60, 0, 0x14);
        sound_call_00574D40(em, 0xe, 7, 9);
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0xd);
        }
        if (em_frame_check(em, 16.0f, 0)) {
            ground_land_eff_set_0057A7E0(em);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 18.0f)) {
                if (!(em_frame_check2(em, 0, 44.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x19, (s16)(((u16)ran_suu(1) & 0x1) + 2));
                    }
                }
            }
        }
        break;
    case 0x405:
        sound_call_00574D40(em, 0x1e, 0x13, 0);
        sound_call_00574D40(em, 2, 0x20, 0x23);
        sound_call_00574D40(em, 4, 0xe, 0xc);
        sound_call_00574D40(em, 8, 0xe, 6);
        break;
    case 0x407:
        sound_call_00574D40(em, 2, 0x2d, 0x23);
        sound_call_00574D40(em, 0xb6, 0x2d, 0x23);
        sound_call_00574D40(em, 0x16c, 0x2d, 0x23);
        va[1] = 10.0f;
        va[2] = 140.0f;
        va[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, va, 1.6f);
        break;
    case 0x408:
        sound_call_00574D40(em, 4, 0x2f, 0x23);
        sound_call_00574D40(em, 0x46, 0, 0x1a);
        sound_call_00574D40(em, 0x8e, 3, 0x14);
        sound_call_00574D40(em, 4, 0x17, 0);
        sound_call_00574D40(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_00574D40(em, 4, 0x2f, 0x23);
        sound_call_00574D40(em, 0x46, 0x2d, 0x23);
        sound_call_00574D40(em, 0x7e, 3, 0x1a);
        sound_call_00574D40(em, 0xb6, 3, 0x14);
        sound_call_00574D40(em, 0x38, 0x16, 0);
        break;
    case 0x40A:
        sound_call_00574D40(em, 0x24, 0x2f, 0x23);
        vb[1] = 10.0f;
        vb[2] = 140.0f;
        vb[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, vb, 1.6f);
        break;
    case 0x40B:
        sound_call_00574D40(em, 0x32, 2, 0x14);
        sound_call_00574D40(em, 0x3a, 6, 0x1a);
        sound_call_00574D40(em, 0x54, 1, 0x1a);
        sound_call_00574D40(em, 0x68, 4, 0x14);
        sound_call_00574D40(em, 0x7c, 0x11, 0x1a);
        sound_call_00574D40(em, 0xcc, 6, 0x1a);
        sound_call_00574D40(em, 0xd2, 2, 0x1a);
        sound_call_00574D40(em, 0xfe, 0x12, 0x14);
        sound_call_00574D40(em, 0x120, 2, 0x1a);
        sound_call_00574D40(em, 0x20, 0xf, 0xc);
        sound_call_00574D40(em, 0x24, 0xf, 6);
        sound_call_00574D40(em, 0x80, 0xf, 0xc);
        sound_call_00574D40(em, 0x84, 0xf, 6);
        sound_call_00574D40(em, 0x122, 0xe, 0xc);
        sound_call_00574D40(em, 0x126, 0xe, 6);
        sound_call_00574D40(em, 2, 0x15, 0);
        sound_call_00574D40(em, 2, 0x23, 0x23);
        sound_call_00574D40(em, 0x30, 0x21, 0x23);
        sound_call_00574D40(em, 0x80, 0x22, 0x23);
        sound_call_00574D40(em, 0xb8, 0x24, 0x23);
        sound_call_00574D40(em, 0x120, 0x22, 0x23);
        break;
    case 0x40C:
        sound_call_00574D40(em, 0x24, 0x24, 0x23);
        sound_call_00574D40(em, 0x14, 0, 0x1a);
        sound_call_00574D40(em, 0x28, 0, 0x14);
        sound_call_00574D40(em, 0x3c, 0x15, 0x22);
        sound_call_00574D40(em, 0x3c, 0xc, 0xc);
        sound_call_00574D40(em, 0xc, 0xc, 6);
        sound_call_00574D40(em, 0xa6, 0, 0x1a);
        if (em_frame_check(em, 50.0f, 0)) {
            shell01_set(em, 1);
        }
        if (em_frame_check(em, 46.0f, 0)) {
            shell01_set(em, 0x29);
        }
        break;
    case 0x40D:
        sound_call_00574D40(em, 0x1e, 0x20, 0x23);
        sound_call_00574D40(em, 0xc, 0xc, 0x1a);
        sound_call_00574D40(em, 0x1c, 2, 0x1a);
        sound_call_00574D40(em, 0x2e, 3, 0x14);
        sound_call_00574D40(em, 0x48, 1, 0x1a);
        break;
    case 0x40E:
        sound_call_00574D40(em, 4, 0x2f, 0x23);
        sound_call_00574D40(em, 0x48, 0x16, 0);
        sound_call_00574D40(em, 0xa4, 9, 0);
        sound_call_00574D40(em, 0x90, 3, 0xc);
        sound_call_00574D40(em, 0x88, 0xe, 6);
        sound_call_00574D40(em, 0xb2, 4, 0x22);
        if (em_frame_check(em, 148.0f, 0)) {
            shell01_set(em, 0xf);
        }
        if (em_frame_check(em, 156.0f, 0)) {
            Eft20_set(1.0f, em, 0, 0);
        }
        break;
    case 0x40F:
        sound_call_00574D40(em, 2, 0x2f, 0x23);
        sound_call_00574D40(em, 2, 0x17, 0);
        sound_call_00574D40(em, 0x40, 3, 0x1a);
        sound_call_00574D40(em, 0x66, 0x10, 0x1a);
        break;
    case 0x410:
        sound_call_00574D40(em, 0x2a, 0xd, 6);
        sound_call_00574D40(em, 0x2e, 0xd, 0xc);
        sound_call_00574D40(em, 0x66, 0xd, 6);
        sound_call_00574D40(em, 0x6a, 0xd, 0xc);
        sound_call_00574D40(em, 0x88, 0xa, 6);
        sound_call_00574D40(em, 0x8c, 0xa, 0xc);
        sound_call_00574D40(em, 2, 0x13, 0);
        sound_call_00574D40(em, 0x34, 4, 0x1a);
        sound_call_00574D40(em, 0x3a, 5, 0x1a);
        sound_call_00574D40(em, 0x38, 7, 0x1a);
        sound_call_00574D40(em, 0x70, 0x1a, 0x22);
        sound_call_00574D40(em, 0x74, 0x1b, 0xc);
        sound_call_00574D40(em, 0x78, 0x1b, 6);
        sound_call_00574D40(em, 0x8e, 0x1a, 0x22);
        sound_call_00574D40(em, 0x94, 0x1b, 0xc);
        sound_call_00574D40(em, 0x96, 0x1b, 6);
        sound_call_00574D40(em, 0xb0, 0x1a, 0x22);
        sound_call_00574D40(em, 0xb4, 0x1b, 0xc);
        sound_call_00574D40(em, 0xb8, 0x1b, 6);
        sound_call_00574D40(em, 0xb0, 0x1a, 0x22);
        sound_call_00574D40(em, 0xd6, 0x1b, 0xc);
        sound_call_00574D40(em, 0xda, 0x1b, 6);
        if (em_frame_check(em, 60.0f, 0)) {
            shell01_set(em, 0x22);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 142.0f)) {
                if (!(em_frame_check(em, 236.0f, 0))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                    }
                }
            }
        }
        break;
    case 0x411:
        sound_call_00574D40(em, 0xc, 0x27, 0x23);
        sound_call_00574D40(em, 2, 0xf, 6);
        sound_call_00574D40(em, 6, 0xf, 0xc);
        sound_call_00574D40(em, 0x16, 0xe, 6);
        sound_call_00574D40(em, 0x1a, 0xe, 0xc);
        sound_call_00574D40(em, 0x34, 0xa, 6);
        sound_call_00574D40(em, 0x38, 0xa, 0xc);
        sound_call_00574D40(em, 0x5c, 0xa, 6);
        sound_call_00574D40(em, 0x60, 0xa, 0xc);
        sound_call_00574D40(em, 0x12, 0x11, 0x1a);
        sound_call_00574D40(em, 0x16, 0x10, 0x14);
        sound_call_00574D40(em, 0x30, 6, 0x1a);
        sound_call_00574D40(em, 0x34, 8, 0x14);
        sound_call_00574D40(em, 0x5c, 6, 0x1a);
        sound_call_00574D40(em, 0x70, 6, 0x14);
        sound_call_00574D40(em, 0x8e, 6, 0x1a);
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0x23);
        }
        if (em_frame_check2(em, 0, 20.0f)) {
            if (!(em_frame_check2(em, 0, 50.0f))) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 2, 0);
                }
            }
        }
        if (em_frame_check(em, 102.0f, 0)) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if (em_frame_check2(em, 0, 22.0f)) {
            if (!(em_frame_check2(em, 0, 38.0f))) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 3, 0);
                }
            }
        }
        if (em_frame_check(em, 62.0f, 0)) {
            Eft20_set(1.0f, em, 3, 0);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 46.0f)) {
                if (!(em_frame_check2(em, 0, 134.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                    }
                }
            }
        }
        break;
    case 0x413:
        sound_call_00574D40(em, 8, 0x23, 0x23);
        sound_call_00574D40(em, 0xe, 0xf, 6);
        sound_call_00574D40(em, 0x1e, 0x14, 0x2a);
        sound_call_00574D40(em, 0x2a, 0, 0x1a);
        sound_call_00574D40(em, 0x48, 1, 0x14);
        quake_call_00574E40(em, 0x2c, 1);
        quake_call_00574E40(em, 0x49, 1);
        if (em_frame_check(em, 30.0f, 0)) {
            shell01_set(em, 0x11);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 6.0f)) {
                if (!(em_frame_check2(em, 0, 38.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, 1);
                    }
                }
            }
        }
        break;
    case 0x414:
        sound_call_00574D40(em, 0x1c, 0xb, 6);
        sound_call_00574D40(em, 0x20, 0xb, 0xc);
        sound_call_00574D40(em, 6, 0x1a, 0x22);
        sound_call_00574D40(em, 0xa, 0x1b, 0xc);
        sound_call_00574D40(em, 0xe, 0x1b, 6);
        sound_call_00574D40(em, 0x26, 0x1a, 0x22);
        sound_call_00574D40(em, 0x2a, 0x1b, 0xc);
        sound_call_00574D40(em, 0x32, 0x1b, 6);
        break;
    case 0x415:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 0x22, 0x5b, 0x23);
            sound_call_00574D40(em, 4, 0xd, 6);
            sound_call_00574D40(em, 8, 0xd, 0xc);
            sound_call_00574D40(em, 0x32, 0xd, 6);
            sound_call_00574D40(em, 0x36, 0xd, 0xc);
            sound_call_00574D40(em, 0x62, 0xb, 6);
            sound_call_00574D40(em, 0x66, 0xb, 0xc);
            sound_call_00574D40(em, 0x98, 0xb, 6);
            sound_call_00574D40(em, 0x9c, 0xb, 0xc);
            sound_call_00574D40(em, 4, 0x1c, 0x22);
            sound_call_00574D40(em, 4, 0x1d, 0xc);
            sound_call_00574D40(em, 8, 0x1d, 6);
            sound_call_00574D40(em, 0x22, 0x1e, 0x22);
            sound_call_00574D40(em, 0x22, 0x1d, 0xc);
            sound_call_00574D40(em, 0x26, 0x1d, 6);
            sound_call_00574D40(em, 0x40, 0x1e, 0x22);
            sound_call_00574D40(em, 0x40, 0x1d, 0xc);
            sound_call_00574D40(em, 0x44, 0x1d, 6);
            sound_call_00574D40(em, 0x5e, 0x1c, 0x22);
            sound_call_00574D40(em, 0x5e, 0x1b, 0xc);
            sound_call_00574D40(em, 0x62, 0x1b, 6);
            sound_call_00574D40(em, 0x7c, 0x1a, 0x22);
            sound_call_00574D40(em, 0x7c, 0x1d, 0xc);
            sound_call_00574D40(em, 0x80, 0x1d, 6);
            sound_call_00574D40(em, 0x9a, 0x1a, 0x22);
            sound_call_00574D40(em, 0x9a, 0x1d, 0xc);
            sound_call_00574D40(em, 0x9e, 0x1d, 6);
        } else {
            sound_call_00574D40(em, 0x22, 0x79, 0x23);
            sound_call_00574D40(em, 4, 0xd, 6);
            sound_call_00574D40(em, 8, 0xd, 0xc);
            sound_call_00574D40(em, 0x32, 0xd, 6);
            sound_call_00574D40(em, 0x36, 0xd, 0xc);
            sound_call_00574D40(em, 0x62, 0xb, 6);
            sound_call_00574D40(em, 0x66, 0xb, 0xc);
            sound_call_00574D40(em, 0x98, 0xb, 6);
            sound_call_00574D40(em, 0x9c, 0xb, 0xc);
            sound_call_00574D40(em, 4, 0x1c, 0x22);
            sound_call_00574D40(em, 4, 0x1d, 0xc);
            sound_call_00574D40(em, 8, 0x1d, 6);
            sound_call_00574D40(em, 0x22, 0x1e, 0x22);
            sound_call_00574D40(em, 0x22, 0x1d, 0xc);
            sound_call_00574D40(em, 0x26, 0x1d, 6);
            sound_call_00574D40(em, 0x40, 0x1e, 0x22);
            sound_call_00574D40(em, 0x40, 0x1d, 0xc);
            sound_call_00574D40(em, 0x44, 0x1d, 6);
            sound_call_00574D40(em, 0x5e, 0x1c, 0x22);
            sound_call_00574D40(em, 0x5e, 0x1b, 0xc);
            sound_call_00574D40(em, 0x62, 0x1b, 6);
            sound_call_00574D40(em, 0x7c, 0x1a, 0x22);
            sound_call_00574D40(em, 0x7c, 0x1d, 0xc);
            sound_call_00574D40(em, 0x80, 0x1d, 6);
            sound_call_00574D40(em, 0x9a, 0x1a, 0x22);
            sound_call_00574D40(em, 0x9a, 0x1d, 0xc);
            sound_call_00574D40(em, 0x9e, 0x1d, 6);
        }
        if (em_frame_check(em, 22.0f, 0)) {
            shell01_set(em, 0x14);
            shell01_set(em, 0x15);
        }
        if (em_frame_check(em, 20.0f, 0) || em_frame_check(em, 68.0f, 0) || em_frame_check(em, 114.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.0f, em, 9, 0);
                Eft20_set(1.0f, em, 0xe, 0);
            }
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 18.0f)) {
                if (!(em_frame_check2(em, 0, 110.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                    }
                }
            }
        }
        break;
    case 0x416:
        sound_call_00574D40(em, 6, 0x18, 0x23);
        sound_call_00574D40(em, 6, 0x29, 0x23);
        sound_call_00574D40(em, 0xa, 0xf, 6);
        sound_call_00574D40(em, 0xe, 0xf, 0xc);
        break;
    case 0x417:
        sound_call_00574D40(em, 2, 0x12, 0);
        sound_call_00574D40(em, 0x50, 0x18, 0x23);
        sound_call_00574D40(em, 0x14, 0x28, 0x23);
        sound_call_00574D40(em, 0x4a, 0x29, 0x23);
        sound_call_00574D40(em, 0x4c, 0xd, 6);
        sound_call_00574D40(em, 0x48, 0xd, 0xc);
        sound_call_00574D40(em, 0xb4, 0x13, 6);
        sound_call_00574D40(em, 0xbb, 0x13, 0xc);
        sound_call_00574D40(em, 0xf0, 0x12, 0);
        break;
    case 0x418:
        sound_call_00574D40(em, 2, 0x22, 0x23);
        sound_call_00574D40(em, 0x22, 0x22, 0x23);
        sound_call_00574D40(em, 0x40, 0x22, 0x23);
        sound_call_00574D40(em, 0x72, 0x20, 0x23);
        sound_call_00574D40(em, 0x10, 1, 0x14);
        break;
    case 0x419:
        sound_call_00574D40(em, 0x20, 0xf, 6);
        sound_call_00574D40(em, 0x24, 0xf, 0xc);
        sound_call_00574D40(em, 0x74, 0xb, 6);
        sound_call_00574D40(em, 0x78, 0xb, 0xc);
        sound_call_00574D40(em, 0xbe, 0xb, 6);
        sound_call_00574D40(em, 0xc2, 0xb, 0xc);
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 72.0f)) {
                if (!(em_frame_check2(em, 0, 208.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                    }
                }
            }
        }
        break;
    case 0x41A:
        sound_call_00574D40(em, 0x4e, 0x29, 0x23);
        sound_call_00574D40(em, 0x4e, 0x18, 0x23);
        sound_call_00574D40(em, 0x78, 0x18, 0x23);
        sound_call_00574D40(em, 0xa4, 0x18, 0x23);
        sound_call_00574D40(em, 4, 0x12, 0x1a);
        sound_call_00574D40(em, 4, 0x13, 0x22);
        sound_call_00574D40(em, 0x40, 0xf, 6);
        sound_call_00574D40(em, 0x40, 0xf, 0xc);
        sound_call_00574D40(em, 0xfa, 0x20, 0x23);
        sound_call_00574D40(em, 0x12a, 0x12, 0x14);
        sound_call_00574D40(em, 0x11e, 0x13, 0);
        break;
    case 0x41B:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x13, 0x2a);
            sound_call_00574D40(em, 0x26, 0x10, 0x1a);
            sound_call_00574D40(em, 0x38, 0x12, 0x1a);
            sound_call_00574D40(em, 0x58, 6, 0x1a);
            sound_call_00574D40(em, 0xa0, 0x56, 0x23);
            sound_call_00574D40(em, 0xee, 0x16, 0);
        } else {
            sound_call_00574D40(em, 4, 0x13, 0x2a);
            sound_call_00574D40(em, 0x26, 0x10, 0x1a);
            sound_call_00574D40(em, 0x38, 0x12, 0x1a);
            sound_call_00574D40(em, 0x58, 6, 0x1a);
            sound_call_00574D40(em, 0xa0, 0x74, 0x23);
            sound_call_00574D40(em, 0xee, 0x16, 0);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0x1b);
        }
        if (em_frame_check(em, 168.0f, 0)) {
            shell01_set(em, 0x1b);
        }
        if (em_frame_check(em, 56.0f, 0)) {
            shell01_set(em, 0x1c);
        }
        if (em_frame_check(em, 54.0f, 0)) {
            Eft20_set(0.8f, em, 2, 2);
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 154.0f, 244.0f) || em_frame_check3(em, 0, 254.0f, 330.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x41C:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 0x16, 0x28, 0x23);
            sound_call_00574D40(em, 0x48, 0x29, 0x23);
            sound_call_00574D40(em, 0x52, 0x18, 0x23);
            sound_call_00574D40(em, 6, 0x14, 0x22);
            sound_call_00574D40(em, 0x16, 0, 0x1a);
            sound_call_00574D40(em, 0x3a, 0xf, 6);
            sound_call_00574D40(em, 0x3e, 0xf, 0xc);
            sound_call_00574D40(em, 0x6a, 0xb, 6);
            sound_call_00574D40(em, 0x6e, 0xb, 0xc);
            sound_call_00574D40(em, 0xac, 0xb, 6);
            sound_call_00574D40(em, 0xb0, 0xb, 0xc);
        } else {
            sound_call_00574D40(em, 0x16, 0x28, 0x23);
            sound_call_00574D40(em, 0x48, 0x29, 0x23);
            sound_call_00574D40(em, 6, 0x14, 0x22);
            sound_call_00574D40(em, 0x16, 0, 0x1a);
            sound_call_00574D40(em, 0x3a, 0xf, 6);
            sound_call_00574D40(em, 0x3e, 0xf, 0xc);
            sound_call_00574D40(em, 0x6a, 0xb, 6);
            sound_call_00574D40(em, 0x6e, 0xb, 0xc);
            sound_call_00574D40(em, 0xac, 0xb, 6);
            sound_call_00574D40(em, 0xb0, 0xb, 0xc);
        }
        if (em_frame_check(em, 72.0f, 0)) {
            Eft20_set(1.0f, em, 4, 0);
        }
        if (em_frame_check(em, 80.0f, 0)) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if (em_frame_check(em, 84.0f, 0)) {
            Eft20_set(1.0f, em, 4, 1);
        }
        break;
    case 0x41D:
        sound_call_00574D40(em, 0x50, 0x21, 0x23);
        sound_call_00574D40(em, 0x58, 0x30, 0x14);
        sound_call_00574D40(em, 0xc, 0xf, 6);
        sound_call_00574D40(em, 8, 0xf, 0xc);
        sound_call_00574D40(em, 0x20, 0xf, 6);
        sound_call_00574D40(em, 0x24, 0xf, 0xc);
        sound_call_00574D40(em, 0x42, 0xf, 6);
        sound_call_00574D40(em, 0x3e, 0xf, 0xc);
        sound_call_00574D40(em, 0x5c, 0xf, 6);
        sound_call_00574D40(em, 0x58, 0xf, 0xc);
        sound_call_00574D40(em, 0x70, 0xf, 6);
        sound_call_00574D40(em, 0x74, 0xf, 0xc);
        sound_call_00574D40(em, 0x92, 0xf, 6);
        sound_call_00574D40(em, 0x96, 0xf, 0xc);
        sound_call_00574D40(em, 0xbc, 0xb, 6);
        sound_call_00574D40(em, 0xb8, 0xb, 0xc);
        if (em_frame_check(em, 90.0f, 0)) {
            shell01_set(em, 0x16);
        }
        if (em_frame_check(em, 108.0f, 0)) {
            shell01_set(em, 0x17);
        }
        break;
    case 0x41E:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 2, 0x12, 0);
            sound_call_00574D40(em, 0x1c, 4, 0x1a);
            sound_call_00574D40(em, 0x38, 4, 0x14);
            sound_call_00574D40(em, 0x1c, 5, 0x1a);
            sound_call_00574D40(em, 0x78, 4, 0x14);
            sound_call_00574D40(em, 0x108, 4, 0x1a);
            sound_call_00574D40(em, 4, 0x14, 0x23);
            sound_call_00574D40(em, 0x40, 0x28, 0x23);
            sound_call_00574D40(em, 0x84, 0x27, 0x23);
            sound_call_00574D40(em, 0x84, 0x51, 0x23);
            sound_call_00574D40(em, 0xa, 0xc, 0xc);
            sound_call_00574D40(em, 0x10, 0xc, 6);
        } else {
            sound_call_00574D40(em, 2, 0x12, 0);
            sound_call_00574D40(em, 0x1c, 4, 0x1a);
            sound_call_00574D40(em, 0x38, 4, 0x14);
            sound_call_00574D40(em, 0x1c, 5, 0x1a);
            sound_call_00574D40(em, 0x78, 4, 0x14);
            sound_call_00574D40(em, 0x108, 4, 0x1a);
            sound_call_00574D40(em, 4, 0x14, 0x23);
            sound_call_00574D40(em, 0x40, 0x28, 0x23);
            sound_call_00574D40(em, 0x84, 0x27, 0x23);
            sound_call_00574D40(em, 0x84, 0x6f, 0x23);
            sound_call_00574D40(em, 0xa, 0xc, 0xc);
            sound_call_00574D40(em, 0x10, 0xc, 6);
        }
        if (em_frame_check(em, 102.0f, 0)) {
            shell01_set(em, 0x12);
        }
        if (em_frame_check(em, 110.0f, 0)) {
            shell01_set(em, 0x13);
            Eft15_set3(em, 5, 1.0f, 4);
        }
        if (em->x8B6) {
            if ((em_frame_check2(em, 0, 20.0f) && !(em_frame_check2(em, 0, 36.0f))) || (em_frame_check2(em, 0, 46.0f) && !(em_frame_check2(em, 0, 168.0f))) || (em_frame_check2(em, 0, 172.0f) && !(em_frame_check2(em, 0, 192.0f))) || (em_frame_check2(em, 0, 196.0f) && !(em_frame_check2(em, 0, 216.0f))) || (em_frame_check2(em, 0, 230.0f) && !(em_frame_check2(em, 0, 238.0f)))) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x421:
        sound_call_00574D40(em, 0xc, 0x12, 0x14);
        sound_call_00574D40(em, 0xa, 0x28, 0x1a);
        sound_call_00574D40(em, 0x2e, 8, 0x14);
        sound_call_00574D40(em, 2, 0xd, 6);
        sound_call_00574D40(em, 4, 0xd, 0xc);
        if (em_frame_check(em, 12.0f, 0)) {
            Eft20_set(1.0f, em, 4, 0);
        }
        if (em_frame_check(em, 30.0f, 0)) {
            Eft20_set(1.0f, em, 5, 0);
        }
        if (em_frame_check(em, 28.0f, 0)) {
            shell01_set(em, 0x1d);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0x25);
        }
        if (em_frame_check(em, 36.0f, 0)) {
            shell01_set(em, 0x28);
        }
        break;
    case 0x423:
        sound_call_00574D40(em, 0xc, 0x16, 0);
        sound_call_00574D40(em, 6, 0x22, 0x23);
        sound_call_00574D40(em, 0x68, 3, 0x1a);
        break;
    case 0x424:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 6, 0x53, 0x23);
            sound_call_00574D40(em, 0x48, 0x13, 0x2a);
            sound_call_00574D40(em, 0x30, 6, 0x1a);
            sound_call_00574D40(em, 0x30, 0x11, 0x14);
            sound_call_00574D40(em, 0x30, 3, 0x14);
            sound_call_00574D40(em, 0x74, 3, 0x14);
            sound_call_00574D40(em, 0x94, 3, 0x1a);
            sound_call_00574D40(em, 0xc4, 3, 0x1a);
            sound_call_00574D40(em, 0x34, 0xc, 6);
            sound_call_00574D40(em, 0x38, 0xd, 0xc);
        } else {
            sound_call_00574D40(em, 6, 0x71, 0x23);
            sound_call_00574D40(em, 0x48, 0x13, 0x2a);
            sound_call_00574D40(em, 0x30, 6, 0x1a);
            sound_call_00574D40(em, 0x30, 0x11, 0x14);
            sound_call_00574D40(em, 0x30, 3, 0x14);
            sound_call_00574D40(em, 0x74, 3, 0x14);
            sound_call_00574D40(em, 0x94, 3, 0x1a);
            sound_call_00574D40(em, 0xc4, 3, 0x1a);
            sound_call_00574D40(em, 0x34, 0xc, 6);
            sound_call_00574D40(em, 0x38, 0xd, 0xc);
        }
        if (em_frame_check(em, 80.0f, 0)) {
            Eft20_set(0.5f, em, 1, 0x80);
        }
        if (em_frame_check(em, 44.0f, 0)) {
            Eft13_set_em_scl(em, 0x1b, 1.0f, 7);
        }
        if (em_frame_check(em, 62.0f, 0)) {
            Eft13_set_em_scl(em, 0x16, 1.0f, 7);
        }
        break;
    case 0x425:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 6, 0x53, 0x23);
            sound_call_00574D40(em, 0xa, 0x12, 0x1a);
            sound_call_00574D40(em, 0x26, 6, 0x1a);
            sound_call_00574D40(em, 0x4e, 4, 0x14);
            sound_call_00574D40(em, 0x6e, 3, 0x14);
            sound_call_00574D40(em, 0xac, 3, 0x1a);
            sound_call_00574D40(em, 0x5a, 0x15, 0);
        } else {
            sound_call_00574D40(em, 6, 0x71, 0x23);
            sound_call_00574D40(em, 0xa, 0x12, 0x1a);
            sound_call_00574D40(em, 0x26, 6, 0x1a);
            sound_call_00574D40(em, 0x4e, 4, 0x14);
            sound_call_00574D40(em, 0x6e, 3, 0x14);
            sound_call_00574D40(em, 0xac, 3, 0x1a);
            sound_call_00574D40(em, 0x5a, 0x15, 0);
        }
        if (em_frame_check(em, 24.0f, 0)) {
            Eft13_set_em_scl(em, 0x1a, 1.0f, 7);
        }
        break;
    case 0x426:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x4f, 0x23);
            sound_call_00574D40(em, 0x56, 0x1f, 0x23);
            sound_call_00574D40(em, 0x10, 3, 0x14);
            sound_call_00574D40(em, 4, 0xd, 6);
            sound_call_00574D40(em, 8, 0xd, 0xc);
            sound_call_00574D40(em, 0x62, 0x13, 0x2a);
            sound_call_00574D40(em, 0x7e, 3, 0x1a);
        } else {
            sound_call_00574D40(em, 4, 0x6d, 0x23);
            sound_call_00574D40(em, 0x56, 0x1f, 0x23);
            sound_call_00574D40(em, 0x10, 3, 0x14);
            sound_call_00574D40(em, 4, 0xd, 6);
            sound_call_00574D40(em, 8, 0xd, 0xc);
            sound_call_00574D40(em, 0x62, 0x13, 0x2a);
            sound_call_00574D40(em, 0x7e, 3, 0x1a);
        }
        break;
    case 0x427:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x52, 0x23);
            sound_call_00574D40(em, 4, 0x13, 0);
            sound_call_00574D40(em, 0x4c, 0x1f, 0x23);
            sound_call_00574D40(em, 0xe, 0, 0x1a);
        } else {
            sound_call_00574D40(em, 4, 0x70, 0x23);
            sound_call_00574D40(em, 4, 0x13, 0);
            sound_call_00574D40(em, 0x4c, 0x1f, 0x23);
            sound_call_00574D40(em, 0xe, 0, 0x1a);
        }
        break;
    case 0x428:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x52, 0x23);
            sound_call_00574D40(em, 4, 0x13, 0);
            sound_call_00574D40(em, 0x44, 0x20, 0x23);
            sound_call_00574D40(em, 0x7a, 0, 0x1a);
        } else {
            sound_call_00574D40(em, 4, 0x70, 0x23);
            sound_call_00574D40(em, 4, 0x13, 0);
            sound_call_00574D40(em, 0x44, 0x20, 0x23);
            sound_call_00574D40(em, 0x7a, 0, 0x1a);
        }
        break;
    case 0x429:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x54, 0x23);
            sound_call_00574D40(em, 0xa8, 0x2a, 0x23);
            sound_call_00574D40(em, 4, 0x27, 0x2c);
            sound_call_00574D40(em, 0xfc, 0x20, 0x23);
            sound_call_00574D40(em, 0x130, 0x27, 0x23);
            sound_call_00574D40(em, 0x3e, 4, 0x2c);
            sound_call_00574D40(em, 0x12, 0x12, 0x14);
            sound_call_00574D40(em, 0x2c, 6, 0x1a);
            sound_call_00574D40(em, 0x3a, 0x10, 0x1a);
            sound_call_00574D40(em, 0xd2, 3, 0x1a);
            sound_call_00574D40(em, 0x142, 3, 0x1a);
            sound_call_00574D40(em, 0x3e, 9, 0);
            sound_call_00574D40(em, 0x4a, 9, 0);
            sound_call_00574D40(em, 0x64, 0x12, 0);
            sound_call_00574D40(em, 0xa2, 0x12, 0);
            sound_call_00574D40(em, 0xe, 0xc, 6);
            sound_call_00574D40(em, 0x12, 0xc, 0xc);
            sound_call_00574D40(em, 0x11e, 0xd, 6);
            sound_call_00574D40(em, 0x122, 0xd, 0xc);
        } else {
            sound_call_00574D40(em, 4, 0x72, 0x23);
            sound_call_00574D40(em, 0xa8, 0x2a, 0x23);
            sound_call_00574D40(em, 4, 0x27, 0x2c);
            sound_call_00574D40(em, 0xfc, 0x20, 0x23);
            sound_call_00574D40(em, 0x130, 0x27, 0x23);
            sound_call_00574D40(em, 0x3e, 4, 0x2c);
            sound_call_00574D40(em, 0x12, 0x12, 0x14);
            sound_call_00574D40(em, 0x2c, 6, 0x1a);
            sound_call_00574D40(em, 0x3a, 0x10, 0x1a);
            sound_call_00574D40(em, 0xd2, 3, 0x1a);
            sound_call_00574D40(em, 0x142, 3, 0x1a);
            sound_call_00574D40(em, 0x3e, 9, 0);
            sound_call_00574D40(em, 0x4a, 9, 0);
            sound_call_00574D40(em, 0x64, 0x12, 0);
            sound_call_00574D40(em, 0xa2, 0x12, 0);
            sound_call_00574D40(em, 0xe, 0xc, 6);
            sound_call_00574D40(em, 0x12, 0xc, 0xc);
            sound_call_00574D40(em, 0x11e, 0xd, 6);
            sound_call_00574D40(em, 0x122, 0xd, 0xc);
        }
        if (em_frame_check(em, 20.0f, 0)) {
            Eft20_set(1.0f, em, 3, 0x80);
        }
        if (em_frame_check(em, 128.0f, 0) || em_frame_check(em, 164.0f, 0)) {
            Eft20_set(0.6f, em, 3, 0x80);
        }
        if (em_frame_check(em, 60.0f, 0)) {
            Eft20_set(1.0f, em, 1, 0x80);
        }
        break;
    case 0x42A:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x52, 0x23);
            sound_call_00574D40(em, 4, 0x13, 0);
        } else {
            sound_call_00574D40(em, 4, 0x70, 0x23);
            sound_call_00574D40(em, 4, 0x13, 0);
        }
        break;
    case 0x42B:
        sound_call_00574D40(em, 0x46, 0x2d, 0x23);
        sound_call_00574D40(em, 0x46, 0x17, 0);
        break;
    case 0x42C:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 0x1c, 2, 0x1a);
            sound_call_00574D40(em, 0x30, 4, 0x14);
            sound_call_00574D40(em, 0xdc, 0x10, 0x1a);
            sound_call_00574D40(em, 0x1e, 0x16, 0);
            sound_call_00574D40(em, 0x108, 9, 0);
            sound_call_00574D40(em, 0x108, 7, 0);
            sound_call_00574D40(em, 0x114, 0x17, 0);
            sound_call_00574D40(em, 0x180, 0x16, 0);
            sound_call_00574D40(em, 0x18, 0x52, 0x23);
            sound_call_00574D40(em, 0x8c, 0x28, 0x23);
            sound_call_00574D40(em, 0xb4, 0x2c, 0x23);
            sound_call_00574D40(em, 0x13a, 0x2f, 0x23);
            sound_call_00574D40(em, 0x174, 0xf, 6);
            sound_call_00574D40(em, 0x176, 0xe, 0xc);
        } else {
            sound_call_00574D40(em, 0x1c, 2, 0x1a);
            sound_call_00574D40(em, 0x30, 4, 0x14);
            sound_call_00574D40(em, 0xdc, 0x10, 0x1a);
            sound_call_00574D40(em, 0x1e, 0x16, 0);
            sound_call_00574D40(em, 0x108, 9, 0);
            sound_call_00574D40(em, 0x108, 7, 0);
            sound_call_00574D40(em, 0x114, 0x17, 0);
            sound_call_00574D40(em, 0x180, 0x16, 0);
            sound_call_00574D40(em, 0x18, 0x70, 0x23);
            sound_call_00574D40(em, 0x8c, 0x28, 0x23);
            sound_call_00574D40(em, 0xb4, 0x2c, 0x23);
            sound_call_00574D40(em, 0x13a, 0x2f, 0x23);
            sound_call_00574D40(em, 0x174, 0xf, 6);
            sound_call_00574D40(em, 0x176, 0xe, 0xc);
        }
        if (em_frame_check(em, 268.0f, 0)) {
            Eft20_set(0.9f, em, 0, 0);
        }
        break;
    case 0x42D:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x52, 0);
            sound_call_00574D40(em, 4, 0x13, 0x2a);
            sound_call_00574D40(em, 0x18, 9, 0);
            sound_call_00574D40(em, 0x18, 0x11, 0);
        } else {
            sound_call_00574D40(em, 4, 0x70, 0);
            sound_call_00574D40(em, 4, 0x13, 0x2a);
            sound_call_00574D40(em, 0x18, 9, 0);
            sound_call_00574D40(em, 0x18, 0x11, 0);
        }
        quake_call_00574E40(em, 0x1a, 2);
        if (em_frame_check(em, 26.0f, 0)) {
            Eft20_set(1.0f, em, 0x12, 0x80);
        }
        break;
    case 0x42E:
    case 0x433:
        sound_call_00574D40(em, 8, 0x16, 0);
        sound_call_00574D40(em, 0x30, 0x31, 0x23);
        break;
    case 0x42F:
        sound_call_00574D40(em, 4, 0x16, 0x23);
        sound_call_00574D40(em, 0x1e, 0x20, 0x23);
        sound_call_00574D40(em, 0x30, 1, 0x14);
        sound_call_00574D40(em, 0x46, 1, 0x1a);
        break;
    case 0x430:
        sound_call_00574D40(em, 4, 0x16, 0x23);
        sound_call_00574D40(em, 0x1e, 0x20, 0x23);
        sound_call_00574D40(em, 0x30, 1, 0x1a);
        sound_call_00574D40(em, 0x46, 1, 0x14);
        break;
    case 0x431:
        sound_call_00574D40(em, 4, 0x1e, 0x22);
        sound_call_00574D40(em, 4, 0x1d, 6);
        sound_call_00574D40(em, 8, 0x1d, 0xc);
        sound_call_00574D40(em, 0xa, 0x2a, 0x23);
        sound_call_00574D40(em, 0x4c, 0x20, 0x23);
        sound_call_00574D40(em, 0x18, 0x12, 0);
        sound_call_00574D40(em, 0x1e, 0x10, 0);
        sound_call_00574D40(em, 0x1e, 9, 0);
        sound_call_00574D40(em, 0x22, 7, 0);
        quake_call_00574E40(em, 0x1a, 2);
        if (em_frame_check2(em, 0, 28.0f)) {
            if (!(em_frame_check2(em, 0, 52.0f))) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0xa, 0x80);
                }
            }
        }
        break;
    case 0x432:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x52, 0);
            sound_call_00574D40(em, 4, 0x13, 0x2a);
            sound_call_00574D40(em, 0x18, 9, 0);
            sound_call_00574D40(em, 0x18, 0x11, 0);
        } else {
            sound_call_00574D40(em, 4, 0x70, 0);
            sound_call_00574D40(em, 4, 0x13, 0x2a);
            sound_call_00574D40(em, 0x18, 9, 0);
            sound_call_00574D40(em, 0x18, 0x11, 0);
        }
        quake_call_00574E40(em, 0x1a, 2);
        if (em_frame_check(em, 26.0f, 0)) {
            Eft20_set(1.0f, em, 0x12, 0x81);
        }
        break;
    case 0x434:
        sound_call_00574D40(em, 0x10, 0x5a, 0x23);
        sound_call_00574D40(em, 0x10, 1, 0x1a);
        em_mahi_eff_set(em, 2);
        break;
    case 0x435:
        sound_call_00574D40(em, 4, 0x2b, 0x23);
        sound_call_00574D40(em, 0x14, 0xb, 0xc);
        sound_call_00574D40(em, 0x3a, 0x14, 0);
        sound_call_00574D40(em, 0x58, 0x1c, 0x22);
        sound_call_00574D40(em, 0x58, 0x1b, 6);
        sound_call_00574D40(em, 0x5c, 0x1b, 0xc);
        break;
    case 0x436:
        sound_call_00574D40(em, 0xa, 9, 0);
        sound_call_00574D40(em, 0xa, 7, 0x2a);
        sound_call_00574D40(em, 0xc, 0x11, 0);
        sound_call_00574D40(em, 0x10, 0x2c, 0x23);
        if (em_frame_check(em, 8.0f, 0)) {
            Eft20_set(1.0f, em, 0xb, 0);
        }
        break;
    case 0x437:
        sound_call_00574D40(em, 0x26, 1, 0x1a);
        sound_call_00574D40(em, 0xc, 0x24, 0x23);
        sound_call_00574D40(em, 0x24, 0x22, 0x23);
        sound_call_00574D40(em, 0x66, 0x22, 0x23);
        sound_call_00574D40(em, 0xa4, 0x22, 0x23);
        if (em_frame_check(em, 48.0f, 0) || em_frame_check(em, 106.0f, 0) || em_frame_check(em, 174.0f, 0)) {
            shell01_set(em, 0x10);
        }
        break;
    case 0x438:
        sound_call_00574D40(em, 2, 0x16, 0x22);
        sound_call_00574D40(em, 0x64, 0x17, 0x22);
        sound_call_00574D40(em, 0x26, 0x2d, 0x23);
        sound_call_00574D40(em, 0xc8, 0x20, 0x23);
        sound_call_00574D40(em, 0x1c, 3, 0x1a);
        sound_call_00574D40(em, 0x140, 3, 0x1a);
        break;
    case 0x439:
        sound_call_00574D40(em, 2, 0x16, 0x22);
        sound_call_00574D40(em, 0x64, 0x17, 0x22);
        sound_call_00574D40(em, 0x26, 0x2d, 0x23);
        sound_call_00574D40(em, 0xc8, 0x20, 0x23);
        sound_call_00574D40(em, 0x1c, 3, 0x14);
        sound_call_00574D40(em, 0x140, 3, 0x14);
        break;
    case 0x43A:
        sound_call_00574D40(em, 4, 0x16, 0);
        sound_call_00574D40(em, 0x32, 0x2c, 0x23);
        break;
    case 0x43B:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x16, 0);
            sound_call_00574D40(em, 4, 0x17, 0);
            sound_call_00574D40(em, 0xa, 0x10, 0x1a);
            sound_call_00574D40(em, 0xa, 0, 0x1a);
            sound_call_00574D40(em, 0x1c, 0, 0x14);
            sound_call_00574D40(em, 0x34, 0, 0x1a);
            sound_call_00574D40(em, 0x58, 0, 0x14);
            sound_call_00574D40(em, 0x80, 0, 0x1a);
            sound_call_00574D40(em, 0xc6, 0, 0x14);
            sound_call_00574D40(em, 4, 0x53, 0x23);
            sound_call_00574D40(em, 0xa8, 0x27, 0x23);
            sound_call_00574D40(em, 0xa2, 0xd, 6);
            sound_call_00574D40(em, 0xa6, 0xd, 0xc);
        } else {
            sound_call_00574D40(em, 4, 0x16, 0);
            sound_call_00574D40(em, 4, 0x17, 0);
            sound_call_00574D40(em, 0xa, 0x10, 0x1a);
            sound_call_00574D40(em, 0xa, 0, 0x1a);
            sound_call_00574D40(em, 0x1c, 0, 0x14);
            sound_call_00574D40(em, 0x34, 0, 0x1a);
            sound_call_00574D40(em, 0x58, 0, 0x14);
            sound_call_00574D40(em, 0x80, 0, 0x1a);
            sound_call_00574D40(em, 0xc6, 0, 0x14);
            sound_call_00574D40(em, 4, 0x71, 0x23);
            sound_call_00574D40(em, 0xa8, 0x27, 0x23);
            sound_call_00574D40(em, 0xa2, 0xd, 6);
            sound_call_00574D40(em, 0xa6, 0xd, 0xc);
        }
        break;
    case 0x43C:
        sound_call_00574D40(em, 0x30, 0x2a, 0x23);
        sound_call_00574D40(em, 0x16, 0xd, 6);
        sound_call_00574D40(em, 0x1a, 0xd, 0xc);
        sound_call_00574D40(em, 0x34, 0xa, 6);
        sound_call_00574D40(em, 0x38, 0xe, 0xc);
        sound_call_00574D40(em, 0x4e, 0xe, 6);
        sound_call_00574D40(em, 0x4a, 0xa, 0xc);
        sound_call_00574D40(em, 0x58, 0xc, 6);
        sound_call_00574D40(em, 0x5c, 0xc, 0xc);
        sound_call_00574D40(em, 0x20, 5, 0x1a);
        sound_call_00574D40(em, 0x50, 1, 0x1a);
        sound_call_00574D40(em, 0x56, 3, 0x14);
        if (em_frame_check(em, 38.0f, 0) || em_frame_check(em, 60.0f, 0) || em_frame_check(em, 78.0f, 0)) {
            Eft13_set_em_scl(em, 2, 5.0f, 7);
        }
        break;
    case 0x43F:
        sound_call_00574D40(em, 4, 0x1a, 0x22);
        sound_call_00574D40(em, 8, 0x1b, 0xc);
        sound_call_00574D40(em, 4, 0x1b, 6);
        sound_call_00574D40(em, 0x22, 0x1c, 0x22);
        sound_call_00574D40(em, 0x40, 0x1c, 0x22);
        break;
    case 0x442:
        sound_call_00574D40(em, 0x30, 0x29, 0x23);
        sound_call_00574D40(em, 0x20, 0xd, 6);
        sound_call_00574D40(em, 0x24, 0xd, 0xc);
        sound_call_00574D40(em, 0x4a, 0xd, 6);
        sound_call_00574D40(em, 0x4e, 0xd, 0xc);
        sound_call_00574D40(em, 0x7c, 0xb, 6);
        sound_call_00574D40(em, 0x80, 0xb, 0xc);
        if (em_frame_check(em, 50.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.0f, em, 9, 0);
                Eft20_set(1.0f, em, 0xe, 0);
            }
        }
        if (em_frame_check(em, 102.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.0f, em, 9, 0);
                Eft20_set(1.0f, em, 0xe, 0);
            }
        }
        break;
    case 0x443:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 0x10, 6, 0x14);
            sound_call_00574D40(em, 0x38, 6, 0x1a);
            sound_call_00574D40(em, 0xb0, 3, 0x14);
            sound_call_00574D40(em, 0xd6, 3, 0x1a);
            sound_call_00574D40(em, 0x108, 1, 0x14);
            sound_call_00574D40(em, 0x130, 1, 0x1a);
            sound_call_00574D40(em, 4, 0x5b, 0x23);
            sound_call_00574D40(em, 0x64, 9, 0);
            sound_call_00574D40(em, 0x5c, 0x10, 0);
            sound_call_00574D40(em, 0x8c, 0x11, 0);
            sound_call_00574D40(em, 0x10, 0xf, 6);
            sound_call_00574D40(em, 0x14, 0xf, 0xc);
            sound_call_00574D40(em, 0x114, 0xc, 6);
            sound_call_00574D40(em, 0x118, 0xc, 0xc);
        } else {
            sound_call_00574D40(em, 0x10, 6, 0x14);
            sound_call_00574D40(em, 0x38, 6, 0x1a);
            sound_call_00574D40(em, 0xb0, 3, 0x14);
            sound_call_00574D40(em, 0xd6, 3, 0x1a);
            sound_call_00574D40(em, 0x108, 1, 0x14);
            sound_call_00574D40(em, 0x130, 1, 0x1a);
            sound_call_00574D40(em, 4, 0x79, 0x23);
            sound_call_00574D40(em, 0x64, 9, 0);
            sound_call_00574D40(em, 0x5c, 0x10, 0);
            sound_call_00574D40(em, 0x8c, 0x11, 0);
            sound_call_00574D40(em, 0x10, 0xf, 6);
            sound_call_00574D40(em, 0x14, 0xf, 0xc);
            sound_call_00574D40(em, 0x114, 0xc, 6);
            sound_call_00574D40(em, 0x118, 0xc, 0xc);
        }
        if (em_frame_check(em, 34.0f, 0)) {
            Eft20_set(1.0f, em, 3, 0);
        }
        if (em_frame_check(em, 70.0f, 0)) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if (em_frame_check2(em, 0, 96.0f)) {
            if (!(em_frame_check2(em, 0, 120.0f))) {
                if (!(*(u16 *)&game_w.x1E % 6)) {
                    Eft20_set(1.0f, em, 0x12, 0);
                }
            }
        }
        if (em->x8B6) {
            if (!(*(u16 *)&game_w.x1E % 19)) {
                Eft20_set(1.0f, em, 0x19, (s16)(((u16)ran_suu(1) & 0x1) + 2));
            }
        }
        break;
    case 0x444:
        sound_call_00574D40(em, 0x1e, 1, 0x14);
        sound_call_00574D40(em, 0x34, 1, 0x1a);
        sound_call_00574D40(em, 4, 0x16, 0);
        sound_call_00574D40(em, 0x46, 0xd, 6);
        sound_call_00574D40(em, 0x4a, 0xd, 0xc);
        sound_call_00574D40(em, 0x6a, 0x14, 0);
        sound_call_00574D40(em, 0x6a, 0x13, 0);
        sound_call_00574D40(em, 0x54, 0x27, 0x23);
        sound_call_00574D40(em, 0xa0, 0xf, 6);
        sound_call_00574D40(em, 0xa4, 0xf, 0xc);
        sound_call_00574D40(em, 0xd2, 0xc, 6);
        sound_call_00574D40(em, 0xd6, 0xc, 0xc);
        if (em_frame_check(em, 98.0f, 0)) {
            shell01_set(em, 0x1e);
        }
        if (em_frame_check(em, 104.0f, 0)) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if (em_frame_check(em, 200.0f, 0)) {
            Eft20_set(1.0f, em, 1, 0);
        }
        break;
    case 0x445:
        sound_call_00574D40(em, 8, 0xa, 6);
        sound_call_00574D40(em, 0xc, 0xa, 0xc);
        break;
    case 0x446:
        sound_call_00574D40(em, 8, 0x20, 0x23);
        sound_call_00574D40(em, 8, 8, 0);
        sound_call_00574D40(em, 0x22, 0x16, 0);
        sound_call_00574D40(em, 0x68, 5, 0x1a);
        sound_call_00574D40(em, 0x6c, 1, 0x14);
        if (em_frame_check(em, 2.0f, 0)) {
            shell01_set(em, 0x1f);
        }
        break;
    case 0x447:
        if (em->kind == 0xb) {
            sound_call_00574D40(em, 4, 0x10, 0);
            sound_call_00574D40(em, 0xa, 0x12, 0);
            sound_call_00574D40(em, 0x14, 9, 0);
            sound_call_00574D40(em, 4, 0x27, 0x23);
            sound_call_00574D40(em, 4, 0x52, 0x23);
        } else {
            sound_call_00574D40(em, 4, 0x10, 0);
            sound_call_00574D40(em, 0xa, 0x12, 0);
            sound_call_00574D40(em, 0x14, 9, 0);
            sound_call_00574D40(em, 4, 0x27, 0x23);
            sound_call_00574D40(em, 4, 0x70, 0x23);
        }
        if (em_frame_check(em, 10.0f, 0)) {
            Eft20_set(1.0f, em, 0xb, 0x80);
        }
        if (em->x8B6) {
            if (em_frame_check2(em, 0, 4.0f)) {
                if (!(em_frame_check2(em, 0, 60.0f))) {
                    if (!(*(u16 *)&game_w.x1E & 0x3)) {
                        Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                    }
                }
            }
        }
        break;
    case 0x448:
        sound_call_00574D40(em, 0x16, 0x12, 0);
        sound_call_00574D40(em, 0x48, 0x12, 0);
        sound_call_00574D40(em, 4, 0xc, 6);
        sound_call_00574D40(em, 8, 0xc, 0xc);
        sound_call_00574D40(em, 0x30, 0x13, 0);
        sound_call_00574D40(em, 4, 0x28, 0x23);
        sound_call_00574D40(em, 0x28, 0x2b, 0x23);
        sound_call_00574D40(em, 4, 0x16, 0);
        if (em->x8B6) {
            if ((em_frame_check2(em, 0, 12.0f) && !(em_frame_check2(em, 0, 24.0f))) || (em_frame_check2(em, 0, 46.0f) && !(em_frame_check2(em, 0, 90.0f)))) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x449:
        sound_call_00574D40(em, 0x22, 0x16, 6);
        sound_call_00574D40(em, 0x26, 0x16, 0xc);
        sound_call_00574D40(em, 0x62, 4, 0x22);
        sound_call_00574D40(em, 0x6c, 3, 0x22);
        sound_call_00574D40(em, 4, 0x2a, 0x23);
        sound_call_00574D40(em, 4, 0x2b, 0x23);
        break;
    case 0x44A:
        sound_call_00574D40(em, 4, 0x16, 6);
        sound_call_00574D40(em, 8, 0x16, 0xc);
        sound_call_00574D40(em, 0x6c, 3, 0x22);
        sound_call_00574D40(em, 4, 0x28, 0x23);
        sound_call_00574D40(em, 0x46, 0x20, 0x23);
        break;
    default:
        move_default_00574E90(em);
        break;
    }
}

void em01_effect_move(EMW *em) {
    EM01W *w = (EM01W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_00574EE0(em, w);
        break;
    }
    em01_uvmove(em);
}
