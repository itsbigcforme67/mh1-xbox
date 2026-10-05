/* em01 AI (part 1) - game.bin 0x00566630-0x0057AB5C: monster kind 1 (and 6/8/11/14/15/17/21/22/26): init, action
 * steps em_act00-40, walk (mv), fly, attack (atk), damage (dmg), demo, death, move states, main, uvmove and the
 * per-animation sound/effect script (ef_move_sub). The setters are in em01.c. The whole file is in this
 * _nm file, matching runs are split off into em01_ai*.c. Field meanings are guesses. */
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
static void sound_call_sub_00574CD0(EMW *em, int se, int joint);
static void sound_call_00574D40(EMW *em, int frame, int se, int joint);
static void sound_call_parts_00574DA0(EMW *em, int frame, int se, int joint, u8 mode);
static void quake_call_00574E40(EMW *em, int frame, int v);
static void move_default_00574E90(EMW *em);
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
static void ground_land_eff_set_0057A7E0(EMW *em);
void get_joint_pos_em(EMW *, int, f32 *);
void eft11_set(EMW *, f32 *, int);
int em_pl_pos_set(EMW *, u8, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
static void takeoff_eff_set_0057A890(EMW *em);
static void takeon_eff_set_0057A900(EMW *em);
static void hover_eff_set2_0057A9A0(EMW *em);
static int kyusyu_char_set_0057A9F0(EMW *em);
static int kyusyu_char_set2_0057AAA0(EMW *em);
static void kyusyu_senkai_ret_0057AB40(EMW *em, EM01W *w);
static void ef_move_sub_00574EE0(EMW *em, EM01W *w);
static void em01_uvmove(EMW *em);
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

void em01_local_init(EMW *em) {
    Eft19_set(em, 0x22, 0);
    eft01_set((PLW *)em, 0);
}

void em01_init(EMW *em) {
    EM01W *w = (EM01W *)em->ex;
    u8 f;
    int k;

    if (quest_w.x08 == 0) {
        em->ang[1] = 0x8000;
        switch (game_w.stage) {
        case 0:
            em->pos[0] = 8000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 6000.0f;
            break;
        case 15:
            em->pos[0] = 7900.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 1050.0f;
            em->pos[2] = 13900.0f;
            break;
        case 18:
            em->pos[0] = 10000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 200.0f;
            em->pos[2] = 8000.0f;
            break;
        case 22:
            em->pos[0] = 9900.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 300.0f;
            em->pos[2] = 10350.0f;
            break;
        case 24:
            em->pos[0] = 10000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 5700.0f;
            em->ang[1] = 0x4000;
            break;
        case 27:
            em->pos[0] = 14300.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 7100.0f;
            break;
        case 37:
            em->pos[0] = 9750.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 300.0f;
            em->pos[2] = 7750.0f;
            break;
        case 40:
            em->pos[0] = 10500.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9500.0f;
            break;
        default:
            em->pos[0] = 5000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f;
            break;
        }
    }
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    em01_act_set(em, 0, 1, 0);
    w->x06 = 0;
    if (em->kind == 1) {
        em->x792 = em->x302 = em_hp_vital_set2(em, 0x4B0, 0x514);
    } else {
        em->x792 = em->x302 = em_hp_vital_set2(em, 0x5DC, 0x1F4);
    }
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->stay_tm = em01_stay_timer_tbl[em->stg];
    em->runaway_tm = em01_runaway_timer_tbl[em->stg];
    w->dang = 0x4000;
    w->pitch_spd = 0x100;
    w->turn = 0x200;
    w->bank_spd = 0x100;
    w->bank_max = 0x2000;
    w->turn_left = 0;
    em->x734 = 3;
    EMF(em, u8, 0x735) = 0;
    w->x48 = 0;
    w->x49 = 0;
    w->x4A = 0;
    f = em->x948 & 1;
    em->x948 = f;
    if (f == 0) {
        k = em->kind;
        if (k != 0x14) {
        switch (k) {
        case 1: case 6: case 8: case 0xB: case 0xF: case 0xE:
        case 0x11: case 0x15: case 0x16: case 0x1A:
            em->ex[0xA3] = 0;
            eft09_set(em, k);
            break;
        }
        }
    }
}

static u16 *em_act_search2_00566C90(EMW *em, u16 *tbl) {
    EM01W *w = (EM01W *)em->ex;
    u16 *p = &tbl[w->x19++ * 2];

    if (*p == 0xFFFF) {
        p = tbl;
        w->x19 = 1;
    }
    return p;
}

static void act_dist_select_00566CD0(EMW *em) {
    u8 type = em->x734;
    u8 idx = EMF(em, u8, 0x735);
    EM01W *w = (EM01W *)em->ex;
    u16 *row;
    int a;

    switch (type) {
    case 0:
        if (em->x8C3 == 0) {
            em01_act_set(em, 0, em_act_search(em01_act_add[idx]), 1);
        }
        break;
    case 1:
        if (em->x8C3 == 0) {
            row = em_act_search2_00566C90(em, em01_rail_add[idx]);
            a = row[0];
            if (a == 1 && row[1] == 0) {
                w->has_tgt = 1;
            }
            em01_act_set(em, a, row[1], 1);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            row = em_act_search2_00566C90(em, em01_rail_half_add[idx]);
            a = row[0];
            if (a == 1 && row[1] == 0) {
                w->has_tgt = 1;
            }
            em01_act_set(em, a, row[1], 1);
        }
        break;
    case 3:
        em->x839 = 1;
        if (em->x388 == 0) {
            em01_act_set(em, 0, 1, 0);
        } else {
            em01_act_set(em, 2, 2, 0);
        }
        break;
    }
}

void em01_to_normal(em, a, b) EMW *em; s16 a; s16 b; {
    if (em->x734 != 0) {
        act_dist_select_00566CD0(em);
        return;
    }
    if (em->x734 == 3) {
        em->act_spd = 1.0f;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x839 = 1;
        if (em->x302 < (s16)(em->x792 * ((em->kind == 1) ? 0.1f : 0.3f))) {
            em01_act_set(em, 0, 1, 0);
        } else if (em->x888 == 0) {
            em01_act_set(em, 0, 1, 0);
        } else {
            em01_act_set(em, 0, 0x11, 0);
        }
        return;
    }
    if (em->char0 != 0x3E9) {
        em_char_set(em, 1, a, b);
    }
    if (em->x2DE != 0x44D) {
        em_char_set(em, 1, a, b);
    }
    if (em->x2E0 != 0x4B1) {
        em_char_set(em, 1, a, b);
    }
    em->x388 = 0;
    em->x3F4 = 0;
    em01_act_set(em, 0, 1, 0);
}

void em01_to_fly(em, mode) EMW *em; int mode; {
    if (em->x734 != 3) {
        act_dist_select_00566CD0(em);
        return;
    }
    em->x839 = 1;
    em->act_spd = 1.0f;
    switch ((u8)mode) {
    case 0:
        em_act_set(em, 2, 0xE);
        break;
    case 1:
        em_act_set(em, 2, 0xF);
        break;
    }
}

void em01_frame_reset(em, i) EMW *em; int i; {
    if (((s32 (*)[20])&em->x194)[i][0] == 0) {
        switch (i) {
        case 0:
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
            break;
        case 1:
            em_char_set2(em, 0x4B1, 0xA, 0);
            break;
        case 2:
            em_char_set2(em, 0x579, 0xA, 0);
            break;
        }
    }
}

void em01_reset_char_set(em, i) EMW *em; int i; {
    switch (i) {
    case 0:
        if (em->char0 == 0x405) {
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
        }
        if (em->char0 == 0x41B) {
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
        }
        break;
    case 1:
        if (em->x2DE == 0x4CD) {
            em_char_set2(em, 0x4B1, 0xA, 0);
        }
        if (em->x2DE == 0x4E3) {
            em_char_set2(em, 0x4B1, 0xA, 0, 1);
        }
        break;
    case 2:
        if (em->x2E0 == 0x595) {
            em_char_set2(em, 0x579, 0xA, 0);
        }
        if (em->x2E0 == 0x5AB) {
            em_char_set2(em, 0x579, 0xA, 0, 2);
        }
        break;
    }
}

static void em_act00_00567280(EMW *em, EM01W *w) {
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

static void em_act01_00567350(EMW *em, EM01W *w) {
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

static void em_act02_00567470(EMW *em, EM01W *w) {
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

static void em_act03_00567530(EMW *em, EM01W *w) {
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

static void em_act04_00567600(EMW *em, EM01W *w) {
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

static void em_act05_005676C0(EMW *em, EM01W *w) {
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

static void em_act06_005677B0(EMW *em, EM01W *w) {
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

static void em_act07_005678A0(EMW *em, EM01W *w) {
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

static void sound_call_sub_00574CD0(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

static void sound_call_00574D40(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, (f32)frame, 0)) {
        sound_call_sub_00574CD0(em, se, joint);
    }
}

static void sound_call_parts_00574DA0(EMW *em, int frame, int se, int joint, u8 mode) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, mode)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}

static void quake_call_00574E40(EMW *em, int frame, int v) {
    if (em_frame_check(em, (f32)frame, 0)) {
        Em_set_quake_sub(em, v);
    }
}

static void em_act08_00567970(EMW *em, EM01W *w) {
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

static void em_act09_00567A90(EMW *em, EM01W *w) {
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

static void em_act10_00567B30(EMW *em, EM01W *w) {
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

static void em_act11_00567C40(EMW *em, EM01W *w) {
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

static void em_act12_00567D30(EMW *em, EM01W *w) {
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

static void em_act13_00567DB0(EMW *em, EM01W *w) {
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

static void em_act14_00567EA0(EMW *em, EM01W *w) {
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

static void em_act15_00568220(EMW *em, EM01W *w) {
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

static void em_act16_00568350(EMW *em, EM01W *w) {
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

static void em_act17_00568480(EMW *em, EM01W *w) {
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

static void em_act18_00568500(EMW *em, EM01W *w) {
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

static void em_act19_005686B0(EMW *em, EM01W *w) {
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

static void em_act40_00568830(EMW *em, EM01W *w) {
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

static void em_act20_005688F0(EMW *em, EM01W *w) {
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

static void em_act21_00568A00(EMW *em, EM01W *w) {
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

static void em_act22_00568BB0(EMW *em, EM01W *w) {
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

static void em_act23_00568C30(EMW *em, EM01W *w) {
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

static void em_act24_00568D00(EMW *em, EM01W *w) {
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

static void em_act25_00568DD0(EMW *em, EM01W *w) {
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

static void em_act26_00568E50(EMW *em, EM01W *w) {
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

static void em_act27_00568F90(EMW *em, EM01W *w) {
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

static void em_act28_005690A0(EMW *em, EM01W *w) {
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

static void em_act29_00569170(EMW *em, EM01W *w) {
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

static void em_act31_005692B0(EMW *em, EM01W *w) {
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

static void em_act33_005693F0(EMW *em, EM01W *w) {
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

static void em_mv00_00569480(EMW *em, EM01W *w) {
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

static void em_mv01_005695C0(EMW *em, EM01W *w) {
    em01_to_normal(em, 0, 0);
}

static void em_mv02_005695D0(EMW *em, EM01W *w) {
    em01_to_normal(em, 0, 0);
}

/* Walk-turn toward the target; mode 0 turns left (anims 5/6), mode 1 right (7/8). */
static void em_mv03_005695E0(EMW *em, EM01W *w) {
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

static void em_mv04_005698F0(EMW *em, EM01W *w) {
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

static void em_mv05_00569A50(EMW *em, EM01W *w) {
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
            em_char_set(em, 7, 0, 0);
        } else {
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            spd = (u32)((16384.0f / (em->x1A8 / 2.0f)) * em->act_spd);
            d = (w->dang - (u16)em->ang[1]) & 0xFFFF;
            if (em->x194 == 0 || em_frame_check2(em, 0, 48.0f)) {
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
                        em_char_set(em, 7, 0, 0);
                    } else {
                        em_char_set(em, 8, 0, 0);
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

static void em_mv06_00569D60(EMW *em, EM01W *w) {
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

static void em_mv07_00569EC0(EMW *em, EM01W *w) {
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

static void em_fly00_0056A000(EMW *em, EM01W *w) {
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

static void em_fly01_0056A100(EMW *em, EM01W *w) {
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

static void em_fly02_0056A340(EMW *em, EM01W *w) {
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

static void em_fly03_0056A450(EMW *em, EM01W *w) {
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

static void em_fly04_0056A6E0(EMW *em, EM01W *w) {
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

static void em_fly05_0056A850(EMW *em, EM01W *w) {
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

static void em_fly06_0056A9C0(EMW *em, EM01W *w) {
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

static void em_fly07_0056ABE0(EMW *em, EM01W *w) {
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

static void em_fly08_0056AD20(EMW *em, EM01W *w) {
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

#define FLY_FLOOR(em)                    \
    if ((em)->pos[1] < (em)->x5AC) {     \
        (em)->pos[1] = (em)->x5AC;       \
    }

static void em_fly09_0056AFC0(EMW *em, EM01W *w) {
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

static void em_fly10_0056B190(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em->x388 = 2;
        em_rate_clear(em);
        NextStage_Dir_Set(em, em->tgt_pos);
        w->x18 = 1;
        break;
    case 1:
        senkai_target(em);
        em01_fly_adjy(em, 1);
        em->pos[1] = em->pos[1] + 100.0f;
        if (!(em->pos[1] < em->tgt_pos[1])) {
            em->x05++;
        }
        break;
    case 2:
        em->x05++;
        em->adj_z = 50.0f;
        em_char_set(em, 0xC, 0, 0);
        /* fallthrough */
    case 3:
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
    case 4:
        em->x05++;
        Em_Next_Stage_Pos();
        WyvernAreaMove((PLW *)em);
        if (em->stg == 0xF) {
            em->stg = 0x13;
        } else {
            em->stg = 0xF;
        }
        em01_to_fly(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly11_0056B390(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0xC, 0, 0);
        if (!(em->adj_z <= 100.0f)) {
            em->adj_z = 100.0f;
        }
        w->x18 = 1;
        break;
    case 1:
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) {
            if (!(em->adj_z <= 50.0f)) {
                em->adj_z = 50.0f;
            }
            em->x05++;
            em->work08 = 0x12C;
            em01_act_set(em, 2, 1, 1);
        } else {
            w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
            w->dang = w->dang - em->ang[1];
            em01_senkai_sub(em, 1, 0);
            w->spd[0] = em->ang[0];
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
            speed_add(em, w->spd);
            if (--em->work08 <= 0) {
                em->x05++;
                em->work08 = 0x12C;
                em01_act_set(em, 2, 1, 1);
            }
        }
        break;
    case 2:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 1, 0);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (--em->work08 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em01_act_set(em, 2, 1, 1);
            }
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly12_0056B5E0(EMW *em, EM01W *w) {
    int done = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x34, 0, 0);
        em01_fly_adjy2_init(em, 0xE);
        break;
    case 1:
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

static void em_fly13_0056B740(EMW *em, EM01W *w) {
    STAGE_DATA *sd;
    EM_POSP p;
    f32 z;
    u8 ar;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_z = 80.0f;
        sd = Stage_data_get(em->stg);
        em->tgt_pos[1] = sd->floor_y;
        p = gp_ptr_ck(em, em->area->x0);
        if (p == 0) {
            em->tgt_pos[0] = sd->width / 2.0f;
            em->tgt_pos[2] = sd->depth / 2.0f;
        } else {
            em->tgt_pos[0] = p[0][0];
            em->tgt_pos[2] = p[0][2];
        }
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[1] = w->dang;
        w->x18 = 1;
        em->work08 = 0x384;
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
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f || --em->work08 <= 0) {
            em->x05++;
            em->work08 = 0x384;
            NextStage_Dir_Set(em, em->tgt_pos);
        }
        break;
    case 2:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub(em, 1, 0);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (AreaFieldInCheck(em->stg, em->pos) == 0 || --em->work08 <= 0) {
            em->x05++;
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
    FLY_FLOOR(em);
}

static void em_fly14_0056BAC0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly15_0056BB80(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xC, 0, 0);
        em_rate_clear_g(em);
        em->adj_y = 0.0f;
        em->rate_x = 0.0f;
        w->x18 = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_fly(em, 1);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly16_0056BC40(EMW *em, EM01W *w) {
    int t;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xF, 0, 0);
        w->turn = 0x100;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        w->x18 = 0;
        em->work08 = 0x96;
        break;
    case 1:
        em01_fly_adjy(em, 1);
        t = senkai_target(em) & 0xFF;
        if (--em->work08 <= 0 || t != 0) {
            em->x05++;
            em01_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
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

static void ground_land_eff_set_0057A7E0(EMW *em) {
    VEC3 v;
    f32 y;

    if (game_w.stage == 0) {
        get_joint_pos_em(em, 0x14, &v.x);
        v.y = em->x5AC;
        if (v.y <= 46.0f) {
            eft11_set(em, &v.x, 1);
            get_joint_pos_em(em, 0x1A, &v.x);
            eft11_set(em, &v.x, 1);
        }
    } else {
        Eft20_set(1.0f, em, 0xB, 0);
    }
}

static void takeoff_eff_set_0057A890(EMW *em) {
    f32 v[3];
    f32 y;

    if (game_w.stage == 0) {
        get_joint_pos_em(em, 0, v);
        v[1] = em->x5AC;
        if (v[1] <= 46.0f) {
            eft11_set(em, v, 1);
        }
    }
}

static void takeon_eff_set_0057A900(EMW *em) {
    f32 v[3];
    f32 y;

    if (game_w.stage == 0 && *(u16 *)&game_w.x1E % 10 == 0) {
        get_joint_pos_em(em, 0, v);
        y = em->x5AC;
        if (v[1] - y < 400.0f && y <= 46.0f) {
            eft11_set(em, v, 1);
        }
    }
}

static void hover_eff_set2_0057A9A0(EMW *em) {
    if (*(u16 *)&game_w.x1E % 5 == 0) {
        Eft20_set(1.0f, em, 0xA, 0);
    }
}

static int kyusyu_char_set_0057A9F0(EMW *em) {
    f32 v[3];
    f32 d;

    if (em->x617 == -1) {
        return 0;
    }
    em_pl_pos_set(em, em->x617, v);
    d = flvecCalcDistance(em->pos, v);
    if (em->char0 == 0x415) {
        return 1;
    }
    if (d <= 4.5f + 30.0f * em->adj_z) {
        em_char_set(em, 0x2D, 0xA, 0);
        return 1;
    }
    return 0;
}

static int kyusyu_char_set2_0057AAA0(EMW *em) {
    f32 d;

    d = flvecCalcDistance(em->pos, em->tgt_pos);
    if (em->char0 == 0x415) {
        return 1;
    }
    if (d <= 2.4199998f + 22.0f * em->adj_z) {
        em_char_set(em, 0x2D, 0xA, 0);
        return 1;
    }
    return 0;
}

static void kyusyu_senkai_ret_0057AB40(EMW *em, EM01W *w) {
    em->x05 = 0x63;
    em01_to_fly(em, 1);
}

/* A file static in the original; data tables point at it. */
void dummy_em_prog_0057AB50(void) {
}

static void em_fly17_0056BD50(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z < 50.0f) {
            em->x07 = 0;
            em->adj_z = 50.0f;
            em_char_set(em, 0xC, 0, 0);
        } else {
            em->x07 = 1;
            if (em->char0 != 0x3F4 && em->char0 != 0x3F6) {
                em_char_set(em, 0xC, 0, 0);
            }
        }
        w->x18 = 1;
        break;
    case 1:
        if (em->x07 == 0 && em->x194 == 0) {
            em->x07++;
            em_char_set(em, 0xE, 0, 0);
        }
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f || --em->work08 <= 0) {
            em->x883++;
            em->x883 &= 3;
            target_kind_set(em, em->tgt_pos);
            em->work08 = (int)(1.5f * CalcDistanceXZ(em->pos, em->tgt_pos) / 50.0f) + 0x96;
            em->x05++;
            em_act_set(em, 2, 0x11);
        }
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub2(em, 1, 0);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly18_0056BF90(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x5A, 0, 0);
        em01_fly_adjy2_init(em, 9);
        w->x18 = 0;
        break;
    case 1:
        if (em->ang[0] != 0) {
            if (em->ang[0] < 0x8000) {
                em->ang[0] = em->ang[0] - w->pitch_spd;
            } else {
                em->ang[0] = em->ang[0] + w->pitch_spd;
            }
            em->ang[0] = (u16)em->ang[0];
        }
        if (em->ang[2] != 0) {
            if (em->ang[2] < 0x8000) {
                em->ang[2] = em->ang[2] - w->bank_spd;
            } else {
                em->ang[2] = em->ang[2] + w->bank_spd;
            }
            em->ang[2] = (u16)em->ang[2];
        }
        if (em01_fly_adjy2(em) & 0xFF) {
            em->x05++;
            em_rate_clear(em);
            em_char_set(em, 0xF, 0, 0);
            em->work08 = 0x12C;
            em01_act_set(em, 2, 1, 1);
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->work08 = 0x12C;
            em_char_set(em, 0xF, 0, 0);
            if (em->x8C3 == 0) {
                em01_act_set(em, 2, 1, 1);
            }
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly19_0056C160(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z < 50.0f) {
            em->x07 = 0;
            em->adj_z = 50.0f;
            em_char_set(em, 0xC, 0, 0);
        } else {
            em->x07 = 1;
            if (em->char0 != 0x3F4 && em->char0 != 0x3F6) {
                em_char_set(em, 0xC, 0, 0);
            }
        }
        w->x18 = 1;
        break;
    case 1:
        if (em->x07 == 0 && em->x194 == 0) {
            em->x07++;
            em_char_set(em, 0xE, 0, 0);
        }
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f || --em->work08 <= 0) {
            em->x05++;
            em01_to_fly(em, 1);
        }
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        em01_senkai_sub3(em, 1, 0);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly20_0056C340(EMW *em, EM01W *w) {
    int done = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x54, 0, 0);
        em01_fly_adjy2_init(em, 0xC);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 44.0f)) {
            em->x388 = 2;
            done = em01_fly_adjy2(em);
        }
        if (done != 0 && em->pos[1] <= em->x5AC) {
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

static void em_fly21_0056C490(EMW *em, EM01W *w) {
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
        w->spd[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        d = flvecCalcDistance(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 10.0f * em->adj_z) {
            em->x05++;
            em_rate_clear(em);
            em01_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly22_0056C5D0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x5A, 0, 0);
        em01_fly_adjy2_init(em, 9);
        w->x18 = 0;
        break;
    case 1:
        if (em->ang[0] != 0) {
            if (em->ang[0] < 0x8000) {
                em->ang[0] = em->ang[0] - w->pitch_spd;
            } else {
                em->ang[0] = em->ang[0] + w->pitch_spd;
            }
            em->ang[0] = (u16)em->ang[0];
        }
        if (em->ang[2] != 0) {
            if (em->ang[2] < 0x8000) {
                em->ang[2] = em->ang[2] - w->bank_spd;
            } else {
                em->ang[2] = em->ang[2] + w->bank_spd;
            }
            em->ang[2] = (u16)em->ang[2];
        }
        if (em01_fly_adjy2(em) & 0xFF) {
            em->x05++;
            em_rate_clear(em);
            em01_to_fly(em, 0);
        }
        break;
    case 2:
        em01_fly_adjy2(em);
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly23_0056C740(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x762 = 0;
        em_char_set(em, 0x12, 0xA, 0x1E);
        em01_fly_adjy2_init(em, 2);
        em->x388 = 2;
        em->x9EA = 0;
        em->x959 = 0;
        em->x8BD = 1;
        break;
    case 1:
        em01_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05++;
        }
        break;
    case 2:
        if ((em01_fly_adjy2(em) & 0xFF) && em->pos[1] <= 800.0f + em->x5AC) {
            em->x05++;
            em_char_set(em, 0xB, 0, 0);
            em_rate_clear(em);
            FLY_FLOOR(em);
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
    FLY_FLOOR(em);
}

static void em_fly24_0056C980(EMW *em, EM01W *w) {
    f32 v[4]; /* never filled: the original passes this uninitialized local (Capcom bug) */

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 20.0f;
        break;
    case 1:
        w->spd[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
        speed_add(em, w->spd);
        if (--em->work08 <= 0 || !(em->pos[1] <= 1000.0f + em->tgt_pos[1])) {
            em->x05++;
            em_rate_clear(em);
            em01_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_atk00_0056CAA0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        if (em01_horm_main(em)) {
            em->x05++;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x24, 0, 0);
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

static void em_atk02_0056CB40(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        switch (em->x06) {
        case 0:
            em->x06++;
            em->x388 = 0;
            /* fallthrough */
        case 1:
            if (em01_horm_main(em)) {
                em->x3F4 = 0;
                em->x05++;
                em_char_set(em, 0x28, 0, 0);
                em01_fly_adjy2_init(em, 5);
            }
            break;
        }
        break;
    case 1:
        if (em_frame_check(em, 58.0f, 0)) {
            takeoff_eff_set_0057A890(em);
        }
        if (em_frame_check2(em, 0, 60.0f)) {
            em->x388 = 2;
            em01_fly_adjy2(em);
            em->x05++;
        }
        break;
    case 2:
        w->dist -= em->adj_z;
        em01_fly_adjy2(em);
        if (w->dist <= 0.0f) {
            em->x05++;
            em->x3C0[1] = -10.0f;
        }
        if (w->dist <= 500.0f) {
            hover_eff_set2_0057A9A0(em);
        }
        break;
    case 3:
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
        if (w->dist <= 500.0f) {
            hover_eff_set2_0057A9A0(em);
        }
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x29, 0, 0);
            takeon_eff_set_0057A900(em);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_atk03_0056CDC0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 1;
        em_char_set(em, 0x24, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk04_0056CE50(EMW *em, EM01W *w) {
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
            Shell08_set_ang(em, 0x22, 0, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05++;
            if (--w->x1A <= 0) {
                em01_to_normal(em, 0, 0);
            } else {
                em01_horm_init(em);
                em_act_set(em, 3, 4);
            }
        }
        break;
    }
}

static void em_atk05_0056D040(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x23, 0, 0);
        em_action_timer_calc(em, 0);
        em->x3F4 = 1;
        shell01_set(em, 1);
        break;
    case 1:
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk06_0056D0F0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x2B, 0, 0);
        em_action_timer_calc(em, 0);
        break;
    case 1:
        em->ang[1] -= 0x200;
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em_char_set(em, 0x2B, 0, 0);
        }
        break;
    case 2:
        em->ang[1] -= 0x200;
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em01_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk07_0056D1E0(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        switch (em->x06) {
        case 0:
            em->x06++;
            em->x388 = 0;
            em->x3F4 = 0;
            /* fallthrough */
        case 1:
            if (em01_horm_main(em)) {
                em->x05++;
                em->x3F4 = 0;
                em_char_set(em, 0x1F, 0, 0);
            }
            break;
        }
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f)) {
            em->x05++;
            em_char_set(em, 0x30, 0, 0);
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

static void em_atk09_0056DD30(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x31, 0, 0);
        em01_fly_adjy2_init(em, 0xD);
        break;
    case 1:
        sound_call_00574D40(em, 0x78, 0x20, 0x23);
        senkai_player(em);
        em01_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 0, 0);
        }
        break;
    case 2:
        sound_call_00574D40(em, 0x78, 0x20, 0x23);
        senkai_player(em);
        em01_fly_adjy2(em);
        if (em_frame_check(em, 6.0f, 0)) {
            Eft17_set(em, 0x24, 1, 0);
            Eft17_set(em, 0x24, 2, 0);
            Shell08_set_ang(em, 0x22, 0, 1, 0x1E94, 0);
        }
        if (em_frame_check(em, 40.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xF, 2, 0x22);
        }
        break;
    case 3:
        em01_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05++;
            em01_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_atk10_0056DF20(EMW *em, EM01W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x31, 0, 0);
        em01_fly_adjy2_init(em, 0xD);
        break;
    case 1:
        senkai_player(em);
        em01_fly_adjy2(em);
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
        if (em_frame_check(em, 40.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xF, 2, 0x22);
        }
        break;
    case 4:
        em01_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05++;
            em01_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_atk11_0056E160(EMW *em, EM01W *w) {
    f32 v[4];
    f32 d;
    f32 f1;
    f32 f3;
    int t;

    switch (em->x05) {
    case 0:
        em->x388 = 2;
        SetVector(w->tp, em->pos[0], em->pos[1], em->pos[2]);
        senkai_player(em);
        em->x3F4 = 0;
        if (w->dang <= 0x100 || w->dang >= 0xFF00) {
            em->x05 = 2;
            em_char_set(em, 0x35, 0, 0);
            em_rate_clear(em);
            em->work08 = 0x2D;
            break;
        }
        em->x05++;
        em->work08 = 0x78;
        if (em->char0 != 0x3F7) {
            em_char_set(em, 0xF, 0, 0);
        }
        break;
    case 1:
        em01_fly_adjy(em, 1);
        if (--em->work08 <= 0) {
            em->x05 = 0x62;
            em->work08 = 0x12C;
            em01_act_set(em, 2, 1, 1);
            break;
        }
        senkai_player(em);
        if (w->dang <= 0x100 || w->dang >= 0xFF00) {
            em->x05 = 2;
            em_char_set(em, 0x35, 0, 0);
            em_rate_clear(em);
            em->work08 = 0x2D;
        }
        break;
    case 2:
        em->x05++;
        senkai_player(em);
        break;
    case 3:
        senkai_player(em);
        d = CalcDistanceXZ(em->pos, em->tgt_pos) - 300.0f;
        EM_F3AC(em) = d;
        if (d < 0.0f) {
            EM_F3AC(em) = 0.0f;
        }
        em->adj_z = 100.0f;
        em->x3C0[2] = 10.0f;
        t = (int)((flSqrt(2.0f * em->x3C0[2] * EM_F3AC(em) + em->adj_z * em->adj_z) - em->adj_z) / em->x3C0[2]) + 1;
        if (t >= --em->work08) {
            em->x05++;
        }
        break;
    case 4:
        senkai_player(em);
        if (em->x1C4 == 0) {
            if (!em_frame_check2(em, 0, 90.0f)) {
                if (em->work08 < 5 && em->work08 != 0) {
                    em->x3C0[2] = 0.0f;
                    d = CalcDistanceXZ(em->pos, em->tgt_pos) - 300.0f;
                    EM_F3AC(em) = d;
                    if (d < 0.0f) {
                        EM_F3AC(em) = 0.0f;
                    }
                    t = em->work08;
                    em->work08 = t - 1;
                    f1 = EM_F3AC(em) / (f32)t;
                    f3 = em->adj_z;
                    if (!(1.2f * f3 < f1) && f1 <= 0.8f * f3) {
                        em->adj_z = f1;
                    } else if (f3 < f1) {
                        em->adj_z *= 1.2f;
                    } else {
                        em->adj_z *= 0.8f;
                    }
                }
                w->spd[1] = em->ang[1];
                w->spd[2] = 0;
                xang_calc_pl(em, w->spd, -200.0f, -300.0f);
                speed_add_g(em, w->spd);
                em_pl_pos_set(em, em->x617, v);
                if (em->pos[1] < v[1]) {
                    em->pos[1] = v[1];
                }
                FLY_FLOOR(em);
            }
            if (em_frame_check2(em, 0, 120.0f)) {
                em->x05++;
                em_rate_clear(em);
                em->work08 = 0x37;
            }
        }
        break;
    case 5:
        d = flvecCalcDistance(em->pos, w->tp);
        w->spd[0] = 0;
        w->spd[1] = Em_Calc_angY(em->pos, w->tp) & 0xFFFF;
        w->spd[2] = 0;
        if (em->work08 != 0) {
            em->adj_z = d / (f32)em->work08;
            em->adj_y = (w->tp[1] - em->pos[1]) / (f32)em->work08--;
            speed_add(em, w->spd);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_rate_clear(em);
            em01_to_fly(em, 0);
        }
        break;
    case 0x62:
    case 0x63:
        if (--em->work08 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em01_act_set(em, 2, 1, 1);
            }
        }
        break;
    }
}

static void em_atk12_0056E6F0(EMW *em, EM01W *w) {
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

static void em_atk13_0056E860(EMW *em, EM01W *w) {
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

static void em_atk15_0056E970(EMW *em, EM01W *w) {
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

static void em_atk16_0056EAD0(EMW *em, EM01W *w) {
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

static void em_atk18_0056EF60(EMW *em, EM01W *w) {
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

static void em_atk17_0056EC80(EMW *em, EM01W *w) {
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

/* kyusyu attack: the monster follows a player in the air (guess: "kyusyu" = suck/drain). */
static void em_atk08_0056D300(EMW *em, EM01W *w) {
    STAGE_DATA *sd;
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
            pl = &player_work[em->x617];
            if (!(Pl_stg_ck_tw(em, pl) & 0xFF) && pl->be_flag != 0) {
                kyusyu_senkai_ret_0057AB40(em, w);
            } else {
                em_pl_pos_set(em, em->x617, p1);
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
            em->work08 = 0x258;
        }
        pl = &player_work[em->x617];
        if (!(Pl_stg_ck_tw(em, pl) & 0xFF) && pl->be_flag != 0) {
            kyusyu_senkai_ret_0057AB40(em, w);
        } else {
            senkai_player(em);
            w->spd[0] = em->ang[0];
            w->spd[1] = em->ang[1];
            w->spd[2] = 0xF;
            speed_add(em, w->spd);
            em->pos[1] -= 0.1f;
            if (--em->work08 <= 0 || !(em_target_pl_samestage_ck(em) & 0xFF)) {
                kyusyu_senkai_ret_0057AB40(em, w);
            }
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
            em_char_set(em, 0xE, 0, 0);
            ground_point_search(em);
        }
        senkai_player(em);
        if (w->x52 == 0) {
            xang_set_pl(em, 2, 100.0f);
            if (em->char0 == 0x415 && em_frame_check2(em, 0, 76.0f)) {
                xang_calc_pl(em, w->spd, 300.0f, 0.0f);
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
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        xang_set_pl(em, 1, 0.0f);
        em01_senkai_sub(em, 3, 1);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (1300.0f + sd->floor_y <= em->pos[1]) {
            em->x05++;
            em->work08 = 0x12C;
        }
        break;
    case 7:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = w->dang - em->ang[1];
        xang_set_pl(em, 2, 0.0f);
        em01_senkai_sub(em, 3, 1);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) {
            ground_point_search(em);
        }
        if ((em->ang[0] == 0 && em->ang[2] == 0) || --em->work08 <= 0) {
            em->ang[0] = 0;
            em->ang[2] = 0;
            kyusyu_senkai_ret_0057AB40(em, w);
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

/* Variant of atk 8 that lands (ground contact after the pursuit). */
static void em_atk21_0056F340(EMW *em, EM01W *w) {
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

DMG_SIMPLE(em_dmg00_00570530, 0x3C)
DMG_SIMPLE(em_dmg01_005705C0, 0x42)
DMG_SIMPLE(em_dmg02_00570650, 0x3F)

static void em_dmg03_005706E0(EMW *em, EM01W *w) {
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

static void em_dmg04_005707A0(EMW *em, EM01W *w) {
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

static void em_dmg05_00570970(EMW *em, EM01W *w) {
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

static void em_dmg06_00570A90(EMW *em, EM01W *w) {
    em01_to_fly(em, 1);
}

static void em_dmg07_00570AA0(EMW *em, EM01W *w) {
    em01_to_fly(em, 1);
}

static void em_dmg08_00570AB0(EMW *em, EM01W *w) {
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

static void em_dmg09_00570C30(EMW *em, EM01W *w) {
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

static void em_dmg11_00570E50(EMW *em, EM01W *w) {
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

static void em_dmg12_00570F20(EMW *em, EM01W *w) {
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

static void em_dmg13_00571050(EMW *em, EM01W *w) {
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

static void em_dmg14_00571180(EMW *em, EM01W *w) {
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

static void em_dmg15_00571290(EMW *em, EM01W *w) {
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

static void em_dmg16_00571310(EMW *em, EM01W *w) {
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

static void em_dmg17_00571430(EMW *em, EM01W *w) {
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

static void em_dmg18_00571550(EMW *em, EM01W *w) {
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

static void em_dmg19_00571650(EMW *em, EM01W *w) {
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

static void em_demo04_00572FA0(EMW *em, EM01W *w) {
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

static void em_die00_005730C0(EMW *em, EM01W *w) {
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

static void em_die01_00573290(EMW *em, EM01W *w) {
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

static void em_die02_00573480(EMW *em, EM01W *w) {
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

static void em_move05_00573F70(EMW *em, EM01W *w) {
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

/* Demo (event) flight: the monster picks up / follows the em at x944 (the partner), lands near it and roars. */
static void em_demo00_00571750(EMW *em, EM01W *w) {
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
static void em_demo01_005720E0(EMW *em, EM01W *w) {
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
static void em_demo02_00572660(EMW *em, EM01W *w) {
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

static void em_move00_00573730(EMW *em, EM01W *w) {
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

static void em_move01_00573980(EMW *em, EM01W *w) {
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

static void em_move02_00573A50(EMW *em, EM01W *w) {
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

static void em_move03_00573C20(EMW *em, EM01W *w) {
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

static void em_move04_00573DF0(EMW *em, EM01W *w) {
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

static void em_move06_00573FE0(EMW *em, EM01W *w) {
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

#define UV_RESET(i) \
    do { \
        uv[i][0] = 0.0f; \
        uv[i][1] = 0.0f; \
        tm[i] = 0xFFFF; \
        ty[i] = 0xFF; \
    } while (0)

#define UVR(i) \
    do { \
        em->uv[i][0] = 0.0f; \
        em->uv[i][1] = 0.0f; \
        em->uvtm[i] = 0xFFFF; \
        em->uvty[i] = 0xFF; \
    } while (0)

void em01_uvmove(EMW *em) {
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

static void move_default_00574E90(EMW *em) {
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
static void ef_move_sub_00574EE0(EMW *em, EM01W *w) {
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
