/* em16 - game.bin 0x005D0600-0x005D8198: per-monster AI for monster kind 16 (also 13 and 30). Sibling of
 * em27 (see em27_nm.c): em16_init / em16_init2 (reset after a revival), em16_to_normal, action steps
 * em_act00-12, move states (turning/walking, fly), attacks em_atk00-12, damage reactions, death (em_die00-09),
 * demo, revival, em16_main (damage system), em16_uvmove, the sound/effect script. The action setters and
 * fly_adjy2 are in em16.c. Field meanings are guesses. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"

/* Per-monster work at EMW+0x444 (em16.c has the same start). */
typedef struct EM16W {
    u8 eff;             /* 0x00 em16_effect_move step */
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
    u8 _pad30[2];
    s8 x32;             /* 0x32 */
    u8 _pad33;
    u8 yobi_stg;        /* 0x34 stage */
    u8 _pad35[3];
    s32 x38;            /* 0x38 */
    s32 x3C;            /* 0x3C */
    f32 x40;            /* 0x40 saved 0x3BC */
    s32 x44;            /* 0x44 */
    s32 x48;            /* 0x48 */
    s32 x4C;            /* 0x4C */
    f32 home[3];        /* 0x50 position of the start */
    s32 home_ang;       /* 0x5C */
    u8 x60;             /* 0x60 */
} EM16W;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void em_dur_init(EMW *);
void em_boss_work_set(EMW *);
void em_status_init(EMW *);
void em16_act_set(EMW *em, int kind, u16 no, u16 arg);
void em16_to_normal();
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
u8 em16_fly_adjy2(EMW *);
void em16_fly_adjy2_init(EMW *, u8);
void shell15_set(EMW *, int);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);
void em_cmd_reset(EMW *);
void Em_Mahi_Start(EMW *);
void em_mahi_eff_set(EMW *, int);
f32 CalcDistanceXZ(f32 *, f32 *);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void shell13_set(EMW *, int);

void em16_local_init(EMW *em) {
    eft01_set((PLW *)em, 0);
}

void em16_init(EMW *em) {
    EM16W *w = (EM16W *)em->ex;

    if (quest_w.x08 == 0) {
        switch (game_w.stage) {
        case 15:
            em->pos[0] = 9500.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9200.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        case 13:
            em->pos[0] = 5000.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        case 37:
            em->pos[0] = 10000.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f + 150.0f * (f32)(em->x13 & 7);
            em->ang[1] = 0;
            break;
        case 38:
            em->pos[0] = 8400.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f + 150.0f * (f32)(em->x13 & 7);
            em->ang[1] = 0x3000;
            break;
        default:
            em->pos[0] = 2400.0f + 150.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 3600.0f;
            break;
        }
    }
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    em_act_set(em, 0, 0xC);
    switch (em->kind) {
    case 16:
        em->x792 = em->x302 = em_hp_vital_set(em, 0x3C);
        break;
    case 13:
        em->x792 = em->x302 = em_hp_vital_set(em, 0x50);
        break;
    case 30:
        em->x792 = em->x302 = em_hp_vital_set(em, 0x78);
        break;
    }
    em->x839 = 0;
    em->x88B = 1;
    w->yobi_st = 0;
    w->home[0] = em->pos[0];
    w->home[1] = em->pos[1];
    w->home[2] = em->pos[2];
    w->home_ang = em->ang[1];
    em_dur_init(em);
    em->x8C3 = 0;
    w->x60 = 0;
    switch (em->x95B) {
    case 1:
        em->x9E1 = 5;
        em_boss_work_set(em);
        em_act_set(em, 0, 0xA);
        break;
    }
}

void em16_init2(EMW *em) {
    EM16W *w = (EM16W *)em->ex;

    em_status_init(em);
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    em_act_set(em, 0, 0xC);
    switch (em->kind) {
    case 16:
        em->x302 = 0x3C;
        em->x792 = 0x3C;
        break;
    case 13:
        em->x302 = 0x50;
        em->x792 = 0x50;
        break;
    case 30:
        em->x302 = 0x78;
        em->x792 = 0x78;
        break;
    }
    em->x88B = 1;
    w->yobi_st = 0;
    em_dur_init(em);
    em->x8C3 = 0;
    w->x60 = 0;
    switch (em->x95B) {
    case 1:
        em->x9E1 = 5;
        em_boss_work_set(em);
        em_act_set(em, 0, 0xA);
        break;
    }
    em->pos[0] = w->home[0];
    em->pos[1] = w->home[1];
    em->pos[2] = w->home[2];
    em->x5A0[0] = em->pos[0];
    em->x5A0[1] = em->pos[1];
    em->x5A0[2] = em->pos[2];
    em->ang[1] = w->home_ang;
    em->x839 = 0;
}

void em16_to_normal(em) EMW *em; {
    em->act_spd = 1.0f;
    em->x388 = 0;
    em->x3F4 = 0;
    em->x839 = 1;
    if (em->x302 < (s16)(0.3f * (f32)em->x792)) {
        em16_act_set(em, 0, 1, 0);
    } else if (em->x888 == 0) {
        em16_act_set(em, 0, 1, 0);
    } else {
        em16_act_set(em, 0, 5, 0);
    }
}

static void em_act00_005D0BB0(EMW *em, EM16W *w) {
    em->act_spd = 1.2f;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = (em->x39A & 3) * 30 + 0x1E;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x839 = 1;
        }
        break;
    }
}

static void em_act01_005D0C60(EMW *em, EM16W *w) {
    em->act_spd = 1.2f;
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x839 = 1;
        }
        break;
    }
}

static void em_act02_005D0CE0(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x32, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act03_005D0D50(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x33, 0, 0);
        break;
    case 1:
        if (em_frame_check2(em, 0, 40.0f) != 0) {
            SetVector(w->yobi, em->pos[0], em->pos[1], em->pos[2]);
            w->yobi_r = 1000.0f;
            w->yobi_stg = em->stg;
            w->x32 = 0;
            push_em_yobi(w->yobi);
            w->yobi_st = 1;
        }
        if (em->x194 == 0) {
            if (w->yobi_st != 0) {
                pull_em_yobi(w->yobi);
                w->yobi_st = 0;
            }
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act04_005D0E50(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x34, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act05_005D0EC0(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xB, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act06_005D0F30(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x27, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act07_005D0FA0(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x2B, 0, 0);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2C, 0, 0);
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em_act_set(em, 0, 8);
        }
        break;
    }
}

static void em_act08_005D1070(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x27, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act09_005D1100(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x412) {
            em_char_set(em, 0x2A, 0, 0);
        }
        break;
    case 1:
        if (em->x94E == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act10_005D1180(EMW *em, EM16W *w) {
    em->act_spd = 1.2f;
    em->x9E1 = 5;
    em->x40E = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3F4) {
            em_char_set(em, 0xC, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x839 = 1;
        }
        break;
    }
}

static void em_act11_005D1210(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xB, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act12_005D1290(EMW *em, EM16W *w) {
    em->act_spd = 1.2f;
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 0xC, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x839 = 1;
        }
        break;
    }
}

static void em_mv00_005D1430(EMW *em, EM16W *w) {
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        if ((u32)(u16)(w->tgt_ang - em->ang[1]) >= 0x8000) {
            em_char_set(em, 6, 0, 0);
        } else {
            em_char_set(em, 7, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            d = (u16)(w->tgt_ang - (u16)em->ang[1]);
            if (em->x194 == 0) {
                if ((u32)(u16)(d + 0x71C) < 0xE38) {
                    em16_to_normal(em, 0, 0);
                } else if (d >= 0x8000) {
                    em_char_set(em, 6, 0, 0);
                } else {
                    em_char_set(em, 7, 0, 0);
                }
            } else if (em_frame_check2(em, 0, 24.0f) != 0 && em_frame_check2(em, 0, 42.0f) == 0) {
                if ((u32)(u16)(d + 0x71C) < 0xE38) {
                    em->ang[1] = w->tgt_ang;
                } else if (d < 0x8000) {
                    em->ang[1] = (u16)(em->ang[1] + 0x71C);
                } else {
                    em->ang[1] = (u16)(em->ang[1] - 0x71C);
                }
            }
        }
        break;
    }
}

#define EM16_TURN2(em, L, H) \
    do { \
        int d = (u16)((u16)Em_Calc_angY((em)->pos, (em)->tgt_pos) - (em)->ang[1]); \
        if (d <= 0x8000) { \
            if (d < L) { \
                (em)->ang[1] += d; \
            } else { \
                (em)->ang[1] += L; \
            } \
        } else if (d > H) { \
            (em)->ang[1] += d; \
        } else { \
            (em)->ang[1] -= L; \
        } \
    } while (0)
#define EM16_TURN(em, L) EM16_TURN2(em, L, 0x10000 - L)

static void em_mv01_005D1610(EMW *em, EM16W *w) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM16_TURN(em, 0x40);
        }
        mot_miration_ret(em, v);
        w->dist -= v[2];
        if (w->dist <= 0.0f) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv02_005D1750(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 8, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv03_005D17C0(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 9, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv04_005D1830(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xA, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv05_005D18A0(EMW *em, EM16W *w) {
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        if ((u32)(u16)(w->tgt_ang - em->ang[1]) >= 0x8000) {
            em_char_set(em, 6, 0, 0);
        } else {
            em_char_set(em, 7, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            d = (u16)(w->tgt_ang - (u16)em->ang[1]);
            if (em->x194 == 0) {
                if ((u32)(u16)(d + 0x71C) < 0xE38) {
                    em16_to_normal(em, 0, 0);
                } else if (d >= 0x8000) {
                    em_char_set(em, 6, 0, 0);
                } else {
                    em_char_set(em, 7, 0, 0);
                }
            } else if (em_frame_check2(em, 0, 24.0f) != 0 && em_frame_check2(em, 0, 42.0f) == 0) {
                if ((u32)(u16)(d + 0x71C) < 0xE38) {
                    em->ang[1] = w->tgt_ang;
                } else if (d < 0x8000) {
                    em->ang[1] = (u16)(em->ang[1] + 0x71C);
                } else {
                    em->ang[1] = (u16)(em->ang[1] - 0x71C);
                }
            }
        }
        break;
    }
}

static void em_fly00_005D1B20(EMW *em, EM16W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        em16_fly_adjy2_init(em, 2);
        break;
    case 1:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x388 = 2;
            landed = em16_fly_adjy2(em) & 0xFF;
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_fly01_005D1C40(EMW *em, EM16W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 4, 0, 0);
        em16_fly_adjy2_init(em, 3);
        break;
    case 1:
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            landed = em16_fly_adjy2(em) & 0xFF;
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_fly02_005D1D60(EMW *em, EM16W *w) {
    em->x9E1 = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        em->x388 = 0;
        em_rate_clear(em);
        em->adj_y = 60.0f;
        em->adj_z = 50.0f;
        em->x3C0[1] = -4.0f;
        em->x3C0[2] = -1.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x388 = 2;
            w->spd[1] = em->ang[1];
            speed_add_g(em, w->spd);
        }
        if (em->adj_y < -50.0f) {
            em->adj_y = -50.0f;
            em->x3C0[1] = 0.0f;
        }
        if (em->adj_z < 20.0f) {
            em->adj_z = 20.0f;
            em->x3C0[2] = 0.0f;
        }
        if (em->adj_y < 0.0f && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_move02_005D1F20(EMW *em, EM16W *w) {
    switch (em->x15) {
    case 0: em_fly00_005D1B20(em, w); break;
    case 1: em_fly01_005D1C40(em, w); break;
    case 2: em_fly02_005D1D60(em, w); break;
    }
}

static void em_atk00_005D1F90(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            if (em->kind == 0x1E) {
                shell13_set(em, 0xB);
            } else {
                shell13_set(em, 1);
            }
        }
        if (w->has_tgt != 0 && em_frame_check2(em, 0, 68.0f) == 0) {
            EM16_TURN(em, 0x80);
        }
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk01_005D2100(EMW *em, EM16W *w) {
    em16_to_normal(em, 0, 0);
}

static void em_atk02_005D2110(EMW *em, EM16W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        em->adj_y = 75.0f;
        em->x3C0[1] = -10.0f;
        em->adj_z = 50.0f;
        em->x3C0[2] = -1.5f;
        w->x38 = 0;
        w->x3C = 0;
        w->x40 = 0.0f;
        w->x44 = 0;
        w->x48 = 0;
        w->x4C = 0;
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            switch (em->kind) {
            case 16:
                shell13_set(em, 3);
                break;
            case 13:
                shell13_set(em, 0xC);
                break;
            case 30:
                shell13_set(em, 0xD);
                break;
            }
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[0] = 0;
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
            landed = 1;
            if (em->x74C & 0xF000000F) {
                w->x40 = em->adj_z;
                em->adj_z = 0.0f;
            }
            speed_add_g(em, w->spd);
            if (w->x40 != 0.0f) {
                em->adj_z = w->x40;
                w->x40 = 0.0f;
            }
            if (em->adj_z < 10.0f) {
                em->adj_z = 10.0f;
                em->x3C0[2] = 0.0f;
            }
        }
        if (w->has_tgt != 0 && em->x388 == 2) {
            EM16_TURN(em, 0x100);
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk03_005D2400(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 16.0f, 0) != 0) {
            switch (em->kind) {
            case 16:
                shell13_set(em, 5);
                break;
            case 13:
                shell13_set(em, 5);
                break;
            case 30:
                shell13_set(em, 0xE);
                break;
            }
        }
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk04_005D2500(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1A, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 80.0f, 0) != 0) {
            Shell08_set_ang(em, 0xF, 9, 0, 0xF8E5, 0);
        }
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk05_005D25B0(EMW *em, EM16W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        em->adj_y = 75.0f;
        em->x3C0[1] = -10.0f;
        em->adj_z = 100.0f;
        em->x3C0[2] = -0.75f;
        w->x38 = 0;
        w->x3C = 0;
        w->x40 = 0.0f;
        w->x44 = 0;
        w->x48 = 0;
        w->x4C = 0;
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            switch (em->kind) {
            case 16:
                shell13_set(em, 5);
                break;
            case 13:
                shell13_set(em, 0xC);
                break;
            case 30:
                shell13_set(em, 0xF);
                break;
            }
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[0] = 0;
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
            landed = 1;
            if (em->x74C & 0xF000000F) {
                w->x40 = em->adj_z;
                em->adj_z = 0.0f;
            }
            speed_add_g(em, w->spd);
            if (w->x40 != 0.0f) {
                em->adj_z = w->x40;
                w->x40 = 0.0f;
            }
            if (em->adj_z < 10.0f) {
                em->adj_z = 10.0f;
                em->x3C0[2] = 0.0f;
            }
        }
        if (w->has_tgt != 0 && em->x388 == 2) {
            EM16_TURN(em, 0x100);
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk06_005D28A0(EMW *em, EM16W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        em->adj_y = 150.0f;
        em->x3C0[1] = -20.0f;
        em->adj_z = 25.0f;
        em->x3C0[2] = -3.0f;
        w->x38 = 0;
        w->x3C = 0;
        w->x40 = 0.0f;
        w->x44 = 0;
        w->x48 = 0;
        w->x4C = 0;
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            switch (em->kind) {
            case 16:
                shell13_set(em, 5);
                break;
            case 13:
                shell13_set(em, 0xC);
                break;
            case 30:
                shell13_set(em, 0x10);
                break;
            }
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[0] = 0;
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
            landed = 1;
            if (em->x74C & 0xF000000F) {
                w->x40 = em->adj_z;
                em->adj_z = 0.0f;
            }
            speed_add_g(em, w->spd);
            if (w->x40 != 0.0f) {
                em->adj_z = w->x40;
                w->x40 = 0.0f;
            }
            if (em->adj_z < 10.0f) {
                em->adj_z = 10.0f;
                em->x3C0[2] = 0.0f;
            }
        }
        if (w->has_tgt != 0 && em->x388 == 2) {
            EM16_TURN(em, 0x100);
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk07_005D2B90(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell13_set(em, 7);
        }
        if (w->has_tgt != 0) {
            EM16_TURN(em, 0x80);
        }
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk08_005D2CC0(EMW *em, EM16W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        em->adj_y = 75.0f;
        em->x3C0[1] = -10.0f;
        em->adj_z = 50.0f;
        em->x3C0[2] = -1.5f;
        w->x38 = 0;
        w->x3C = 0;
        w->x40 = 0.0f;
        w->x44 = 0;
        w->x48 = 0;
        w->x4C = 0;
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell13_set(em, 9);
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[0] = 0;
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
            landed = 1;
            if (em->x74C & 0xF000000F) {
                w->x40 = em->adj_z;
                em->adj_z = 0.0f;
            }
            speed_add_g(em, w->spd);
            if (w->x40 != 0.0f) {
                em->adj_z = w->x40;
                w->x40 = 0.0f;
            }
            if (em->adj_z < 10.0f) {
                em->adj_z = 10.0f;
                em->x3C0[2] = 0.0f;
            }
        }
        if (w->has_tgt != 0 && em->x388 == 2) {
            EM16_TURN(em, 0x100);
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk09_005D2F50(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 16.0f, 0) != 0) {
            shell13_set(em, 0xA);
        }
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk10_005D2FF0(EMW *em, EM16W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        em->adj_y = 75.0f;
        em->x3C0[1] = -10.0f;
        em->adj_z = 100.0f;
        em->x3C0[2] = -0.75f;
        w->x38 = 0;
        w->x3C = 0;
        w->x40 = 0.0f;
        w->x44 = 0;
        w->x48 = 0;
        w->x4C = 0;
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell13_set(em, 9);
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[0] = 0;
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
            landed = 1;
            if (em->x74C & 0xF000000F) {
                w->x40 = em->adj_z;
                em->adj_z = 0.0f;
            }
            speed_add_g(em, w->spd);
            if (w->x40 != 0.0f) {
                em->adj_z = w->x40;
                w->x40 = 0.0f;
            }
            if (em->adj_z < 10.0f) {
                em->adj_z = 10.0f;
                em->x3C0[2] = 0.0f;
            }
        }
        if (w->has_tgt != 0 && em->x388 == 2) {
            EM16_TURN(em, 0x100);
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk11_005D3280(EMW *em, EM16W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        em->adj_y = 150.0f;
        em->x3C0[1] = -20.0f;
        em->adj_z = 25.0f;
        em->x3C0[2] = -3.0f;
        w->x38 = 0;
        w->x3C = 0;
        w->x40 = 0.0f;
        w->x44 = 0;
        w->x48 = 0;
        w->x4C = 0;
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell13_set(em, 9);
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[0] = 0;
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
            landed = 1;
            if (em->x74C & 0xF000000F) {
                w->x40 = em->adj_z;
                em->adj_z = 0.0f;
            }
            speed_add_g(em, w->spd);
            if (w->x40 != 0.0f) {
                em->adj_z = w->x40;
                w->x40 = 0.0f;
            }
            if (em->adj_z < 10.0f) {
                em->adj_z = 10.0f;
                em->x3C0[2] = 0.0f;
            }
        }
        if (w->has_tgt != 0 && em->x388 == 2) {
            EM16_TURN(em, 0x100);
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk12_005D3510(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell13_set(em, 0x11);
        }
        if (w->has_tgt != 0 && em_frame_check2(em, 0, 68.0f) == 0) {
            EM16_TURN(em, 0x80);
        }
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}
