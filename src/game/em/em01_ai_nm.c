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

/* em01's part of the per-monster work at EMW+0x444 (em01.c has the same start). */
typedef struct EM01W {
    u8 _pad00[5];
    u8 x05;             /* 0x05 fly mode (4 circling, 1 fly 19) */
    u8 _pad06[2];
    u8 x08;             /* 0x08 cleared by every em01_act_set */
    u8 _pad09[7];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    u8 _pad18;
    u8 x19;             /* 0x19 row counter of em_act_search2 */
    u8 x1A;             /* 0x1A attack variant */
    u8 _pad1B;
    s32 turn_left;      /* 0x1C */
    s32 bank_max;       /* 0x20 bank limit for senkai_sub */
    u8 _pad24[0x30 - 0x24];
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
    *(s16 *)&w->_pad06[0] = 0;
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
