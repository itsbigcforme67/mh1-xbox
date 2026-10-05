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
    u8 _pad01[4];
    u8 x05;             /* 0x05 fly mode (4 circling, 1 fly 19) */
    s16 x06;            /* 0x06 (150 set after a landing) */
    u8 x08;             /* 0x08 cleared by every em01_act_set */
    u8 _pad09[7];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    u8 x18;             /* 0x18 1 while flying (set by fly 6 and 8) */
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
} EM01W;

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
static void kyusyu_senkai_ret_0057AB40(EMW *em);
static void ef_move_sub_00574EE0(EMW *em);
static void em01_uvmove(EMW *em);
void em01_senkai_sub2(EMW *, int, int);
void em01_senkai_sub3(EMW *, int, int);
int em01_horm_main(EMW *);
void em01_horm_init(EMW *);
void Eft17_set(EMW *, int, int, int);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
int ran_suu(int);
void em_action_timer_calc(EMW *, int);
f32 flSqrt(f32);
void SetVector(f32 *, f32, f32, f32);
void senkai_player(EMW *);
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
            EMF(em, u8, 0x4E7) = 0;
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
    if (*(s32 *)((u8 *)em + i * 0x50 + 0x194) == 0) {
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
            if ((u32)(t - 1) < 2U || t == 3) {
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
        if (d < 0xE39U || d >= 0xF1C8U) {
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
                    return;
                }
                if (d < 0xE39U || d >= 0xF1C8U) {
                    pl_flag_set((PLW *)em, 0x20000);
                    em_char_set(em, 3, 0, 0);
                    return;
                }
                pl_flag_clr((PLW *)em, 0x20000);
                if (d >= 0x8000U) {
                    em_char_set(em, 6, 0, 0);
                } else {
                    em_char_set(em, 5, 0, 0);
                }
                return;
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
        if (d < 0xE39U || d >= 0xF1C8U) {
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
            if (em->x194 == 0 || em_frame_check2(em, 0, 110.0f)) {
                if ((u32)((d + spd) & 0xFFFF) < spd * 2) {
                    em->x05++;
                    pl_flag_clr((PLW *)em, 0x20000);
                    em01_to_normal(em, 0, 0);
                    return;
                }
                if (d < 0xE39U || d >= 0xF1C8U) {
                    pl_flag_set((PLW *)em, 0x20000);
                    em_char_set(em, 3, 0, 0);
                    return;
                }
                pl_flag_clr((PLW *)em, 0x20000);
                if (d >= 0x8000U) {
                    em_char_set(em, 7, 0, 0);
                } else {
                    em_char_set(em, 8, 0, 0);
                }
                return;
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
    int t;
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
        t = em->work08 - 1;
        em->work08 = t;
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f || t < 0) {
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
        ef_move_sub_00574EE0(em);
        break;
    }
    em01_uvmove(em);
}

static void ground_land_eff_set_0057A7E0(EMW *em) {
    VEC3 v;
    f32 y;

    if (game_w.stage == 0) {
        get_joint_pos_em(em, 0x14, &v.x);
        y = em->x5AC;
        v.y = y;
        if (y <= 46.0f) {
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
        y = em->x5AC;
        v[1] = y;
        if (y <= 46.0f) {
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

static void kyusyu_senkai_ret_0057AB40(EMW *em) {
    ((EM01W *)em->ex)->x05 = 0x63;
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
