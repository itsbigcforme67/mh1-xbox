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
extern f32 st02_pos_tbl[][6];
extern f32 st09_pos_tbl[][6];
extern f32 st18_pos_tbl[][6];
extern f32 st31_pos_tbl[][6];
extern f32 st38_pos_tbl_00663520[][6];
extern f32 st39_pos_tbl_006635A0[][6];
extern f32 st46_pos_tbl[][6];
extern f32 st49_pos_tbl_00663690[][6];
extern u16 st51_ang_tbl[3];
extern f32 st51_pos_tbl_00663720[][3];
extern u16 st53_ang_tbl[4];
extern f32 st53_pos_tbl_00663750[][3];
extern f32 st58_pos_tbl[][6];
extern f32 st63_pos_tbl[][6];
extern f32 st66_pos_tbl[][6];
extern f32 st69_pos_tbl[][6];
extern f32 st71_pos_tbl[][6];
void shell13_set(EMW *, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);
int Code_Make(int, int, int, int);
void Eft13_set_em(EMW *, int, int);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
int em_mode_timer_sub(EMW *);
int Em_Yobi_Ck(EMW *, f32 *);
int em_cancel_act_ck(EMW *, u8);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void em_cmd_ck(EMW *);
void em16_init2(EMW *);
int Event_flag_ck(int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em16_init(EMW *);
void Quest_enemy_escape(EMW *);

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

void em_move00_005D1320(EMW *em, EM16W *w) {
    switch (em->x15) {
    case 0: em_act00_005D0BB0(em, w); break;
    case 1: em_act01_005D0C60(em, w); break;
    case 2: em_act02_005D0CE0(em, w); break;
    case 3: em_act03_005D0D50(em, w); break;
    case 4: em_act04_005D0E50(em, w); break;
    case 5: em_act05_005D0EC0(em, w); break;
    case 6: em_act06_005D0F30(em, w); break;
    case 7: em_act07_005D0FA0(em, w); break;
    case 8: em_act08_005D1070(em, w); break;
    case 9: em_act09_005D1100(em, w); break;
    case 10: em_act10_005D1180(em, w); break;
    case 11: em_act11_005D1210(em, w); break;
    case 12: em_act12_005D1290(em, w); break;
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

void em_move01_005D1A80(EMW *em, EM16W *w) {
    switch (em->x15) {
    case 0: em_mv00_005D1430(em, w); break;
    case 1: em_mv01_005D1610(em, w); break;
    case 2: em_mv02_005D1750(em, w); break;
    case 3: em_mv03_005D17C0(em, w); break;
    case 4: em_mv04_005D1830(em, w); break;
    case 5: em_mv05_005D18A0(em, w); break;
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

void em_move03_005D3660(EMW *em, EM16W *w) {
    switch (em->x15) {
    case 0: em_atk00_005D1F90(em, w); break;
    case 1: em_atk01_005D2100(em, w); break;
    case 2: em_atk02_005D2110(em, w); break;
    case 3: em_atk03_005D2400(em, w); break;
    case 4: em_atk04_005D2500(em, w); break;
    case 5: em_atk05_005D25B0(em, w); break;
    case 6: em_atk06_005D28A0(em, w); break;
    case 7: em_atk07_005D2B90(em, w); break;
    case 8: em_atk08_005D2CC0(em, w); break;
    case 9: em_atk09_005D2F50(em, w); break;
    case 10: em_atk10_005D2FF0(em, w); break;
    case 11: em_atk11_005D3280(em, w); break;
    case 12: em_atk12_005D3510(em, w); break;
    }
}

static void em_dmg00_005D3770(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1E, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg01_005D37F0(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1F, 0, 0);
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x388 = 2;
            speed_add_g(em, w->spd);
            if (em->pos[1] <= em->x5AC) {
                em->x05++;
                em->x388 = 0;
                em_char_set(em, 0x20, 0, 0);
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em16_act_set(em, 0, 6, 0);
        }
        break;
    }
}

static void em_dmg02_005D3930(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x21, 0, 0);
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] + 0x8000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x388 = 2;
            speed_add_g(em, w->spd);
            if (em->pos[1] <= em->x5AC) {
                em->x05++;
                em->x388 = 0;
                em_char_set(em, 0x22, 0, 0);
            }
        }
        break;
    case 2:
        if (em_frame_check2(em, 0, 26.0f) == 0) {
            em->ang[1] -= 0x555;
        }
        if (em->x194 == 0) {
            em16_act_set(em, 0, 6, 0);
        }
        break;
    }
}

static void em_dmg03_005D3AB0(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x23, 0, 0);
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] - 0x4000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x388 = 2;
            speed_add_g(em, w->spd);
            if (em->pos[1] <= em->x5AC) {
                em->x05++;
                em->x388 = 0;
                em_char_set(em, 0x24, 0, 0);
            }
        }
        break;
    case 2:
        if (em_frame_check2(em, 0, 26.0f) == 0) {
            em->ang[1] -= 0x2AA;
        }
        if (em->x194 == 0) {
            em16_act_set(em, 0, 6, 0);
        }
        break;
    }
}

static void em_dmg04_005D3C30(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x25, 0, 0);
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] + 0x4000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x388 = 2;
            speed_add_g(em, w->spd);
            if (em->pos[1] <= em->x5AC) {
                em->x05++;
                em->x388 = 0;
                em_char_set(em, 0x26, 0, 0);
            }
        }
        break;
    case 2:
        if (em_frame_check2(em, 0, 26.0f) == 0) {
            em->ang[1] += 0x2AA;
        }
        if (em->x194 == 0) {
            em16_act_set(em, 0, 6, 0);
        }
        break;
    }
}

static void em_dmg05_005D3DB0(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x2A, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg06_005D3E30(EMW *em, EM16W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x2F, 0, 0);
        Em_Mahi_Start(em);
        em_cmd_reset(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_act_set(em, 0, 0xB);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

void em_move04_005D3EE0(EMW *em, EM16W *w) {
    switch (em->x15) {
    case 0: em_dmg00_005D3770(em, w); break;
    case 1: em_dmg01_005D37F0(em, w); break;
    case 2: em_dmg02_005D3930(em, w); break;
    case 3: em_dmg03_005D3AB0(em, w); break;
    case 4: em_dmg04_005D3C30(em, w); break;
    case 5: em_dmg05_005D3DB0(em, w); break;
    case 6: em_dmg06_005D3E30(em, w); break;
    }
}

static void em_die00_005D3F90(EMW *em, EM16W *w) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        w->x60 = 1;
        Quest_enemy_die(em);
        em_char_set(em, 0x1F, 0, 0);
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        em->x388 = 2;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x20, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 0, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x29, 0, 0);
            em->work08 = 0x12C;
            Em_hagi_point_set(em, 0);
        }
        break;
    case 4:
        Em_hagi_point_cnt_ck(em);
        if (--em->work08 <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 5:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
        }
        break;
    }
}

static void em_die01_005D41B0(EMW *em, EM16W *w) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        w->x60 = 1;
        Quest_enemy_die(em);
        em_char_set(em, 0x21, 0, 0);
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] + 0x8000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        em->x388 = 2;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 0, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x29, 0, 0);
            em->work08 = 0x12C;
            Em_hagi_point_set(em, 0);
        }
        break;
    case 4:
        Em_hagi_point_cnt_ck(em);
        if (--em->work08 <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 5:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
        }
        break;
    }
}

static void em_die02_005D43D0(EMW *em, EM16W *w) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        w->x60 = 1;
        Quest_enemy_die(em);
        em_char_set(em, 0x23, 0, 0);
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] - 0x4000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        em->x388 = 2;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x24, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 0, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x29, 0, 0);
            em->work08 = 0x12C;
            Em_hagi_point_set(em, 0);
        }
        break;
    case 4:
        Em_hagi_point_cnt_ck(em);
        if (--em->work08 <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 5:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
        }
        break;
    }
}

static void em_die03_005D45F0(EMW *em, EM16W *w) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        w->x60 = 1;
        Quest_enemy_die(em);
        em_char_set(em, 0x25, 0, 0);
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] + 0x4000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        em->x388 = 2;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x26, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x2E, 0, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x29, 0, 0);
            em->work08 = 0x12C;
            Em_hagi_point_set(em, 0);
        }
        break;
    case 4:
        Em_hagi_point_cnt_ck(em);
        if (--em->work08 <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 5:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
        }
        break;
    }
}

static void em_die04_005D4810(EMW *em, EM16W *w) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        if (Quest_enemy_revival_ck(em) == 1) {
            Quest_enemy_revival_set(em);
            em_status_init(em);
            em16_init(em);
            em_cmd_reset(em);
            em->x839 = 0;
            em->mode = 5;
            em->x15 = 4;
            switch (em->stg) {
            case 0x2: {
                int r = (u16)(em->x39A % 6);

                SetVector(em->pos, st02_pos_tbl[r][0], st02_pos_tbl[r][1], st02_pos_tbl[r][2]);
                SetVector(em->x5A0, st02_pos_tbl[r][0], st02_pos_tbl[r][1], st02_pos_tbl[r][2]);
                SetVector(v, st02_pos_tbl[r][3], st02_pos_tbl[r][4], st02_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x9: {
                int r = (u16)(em->x39A % 6);

                SetVector(em->pos, st09_pos_tbl[r][0], st09_pos_tbl[r][1], st09_pos_tbl[r][2]);
                SetVector(em->x5A0, st09_pos_tbl[r][0], st09_pos_tbl[r][1], st09_pos_tbl[r][2]);
                SetVector(v, st09_pos_tbl[r][3], st09_pos_tbl[r][4], st09_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x12: {
                int r = (u16)(em->x39A % 4);

                SetVector(em->pos, st18_pos_tbl[r][0], st18_pos_tbl[r][1], st18_pos_tbl[r][2]);
                SetVector(em->x5A0, st18_pos_tbl[r][0], st18_pos_tbl[r][1], st18_pos_tbl[r][2]);
                SetVector(v, st18_pos_tbl[r][3], st18_pos_tbl[r][4], st18_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x1F: {
                int r = (u16)(em->x39A % 4);

                SetVector(em->pos, st31_pos_tbl[r][0], st31_pos_tbl[r][1], st31_pos_tbl[r][2]);
                SetVector(em->x5A0, st31_pos_tbl[r][0], st31_pos_tbl[r][1], st31_pos_tbl[r][2]);
                SetVector(v, st31_pos_tbl[r][3], st31_pos_tbl[r][4], st31_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 1);
                break;
            }
            case 3:
            case 0x16:
            case 0x17:
            case 0x1A:
            case 0x20:
            case 0x21:
            case 0x24:
            case 0x28:
            case 0x2D:
            case 0x3B:
                Quest_enemy_escape(em);
                em->x04++;
                em->x01 = 0;
                break;
            case 0x26: {
                int r = (u16)(em->x39A % 5);

                SetVector(em->pos, st38_pos_tbl_00663520[r][0], st38_pos_tbl_00663520[r][1], st38_pos_tbl_00663520[r][2]);
                SetVector(em->x5A0, st38_pos_tbl_00663520[r][0], st38_pos_tbl_00663520[r][1], st38_pos_tbl_00663520[r][2]);
                SetVector(v, st38_pos_tbl_00663520[r][3], st38_pos_tbl_00663520[r][4], st38_pos_tbl_00663520[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 2);
                break;
            }
            case 0x27: {
                int r = (u16)(em->x39A % 4);

                SetVector(em->pos, st39_pos_tbl_006635A0[r][0], st39_pos_tbl_006635A0[r][1], st39_pos_tbl_006635A0[r][2]);
                SetVector(em->x5A0, st39_pos_tbl_006635A0[r][0], st39_pos_tbl_006635A0[r][1], st39_pos_tbl_006635A0[r][2]);
                SetVector(v, st39_pos_tbl_006635A0[r][3], st39_pos_tbl_006635A0[r][4], st39_pos_tbl_006635A0[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x2E: {
                int r = (u16)(em->x39A % 6);

                SetVector(em->pos, st46_pos_tbl[r][0], st46_pos_tbl[r][1], st46_pos_tbl[r][2]);
                SetVector(em->x5A0, st46_pos_tbl[r][0], st46_pos_tbl[r][1], st46_pos_tbl[r][2]);
                SetVector(v, st46_pos_tbl[r][3], st46_pos_tbl[r][4], st46_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x31: {
                int r = (u16)(em->x39A % 6);

                SetVector(em->pos, st49_pos_tbl_00663690[r][0], st49_pos_tbl_00663690[r][1], st49_pos_tbl_00663690[r][2]);
                SetVector(em->x5A0, st49_pos_tbl_00663690[r][0], st49_pos_tbl_00663690[r][1], st49_pos_tbl_00663690[r][2]);
                SetVector(v, st49_pos_tbl_00663690[r][3], st49_pos_tbl_00663690[r][4], st49_pos_tbl_00663690[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x33: {
                int r = (u16)(em->x39A % 3);

                SetVector(em->pos, st51_pos_tbl_00663720[r][0], st51_pos_tbl_00663720[r][1], st51_pos_tbl_00663720[r][2]);
                em->ang[1] = st51_ang_tbl[r];
                em_act_set(em, 0, 0xC);
                break;
            }
            case 0x35: {
                int r = (u16)(em->x39A % 4);

                SetVector(em->pos, st53_pos_tbl_00663750[r][0], st53_pos_tbl_00663750[r][1], st53_pos_tbl_00663750[r][2]);
                em->ang[1] = st53_ang_tbl[r];
                em_act_set(em, 0, 0xC);
                break;
            }
            case 0x3A: {
                int r = (u16)(em->x39A % 6);

                SetVector(em->pos, st58_pos_tbl[r][0], st58_pos_tbl[r][1], st58_pos_tbl[r][2]);
                SetVector(em->x5A0, st58_pos_tbl[r][0], st58_pos_tbl[r][1], st58_pos_tbl[r][2]);
                SetVector(v, st58_pos_tbl[r][3], st58_pos_tbl[r][4], st58_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x3F: {
                int r = (u16)(em->x39A % 3);

                SetVector(em->pos, st63_pos_tbl[r][0], st63_pos_tbl[r][1], st63_pos_tbl[r][2]);
                SetVector(em->x5A0, st63_pos_tbl[r][0], st63_pos_tbl[r][1], st63_pos_tbl[r][2]);
                SetVector(v, st63_pos_tbl[r][3], st63_pos_tbl[r][4], st63_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x42: {
                int r = (u16)(em->x39A % 6);

                SetVector(em->pos, st66_pos_tbl[r][0], st66_pos_tbl[r][1], st66_pos_tbl[r][2]);
                SetVector(em->x5A0, st66_pos_tbl[r][0], st66_pos_tbl[r][1], st66_pos_tbl[r][2]);
                SetVector(v, st66_pos_tbl[r][3], st66_pos_tbl[r][4], st66_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x45: {
                int r = (u16)(em->x39A % 3);

                SetVector(em->pos, st69_pos_tbl[r][0], st69_pos_tbl[r][1], st69_pos_tbl[r][2]);
                SetVector(em->x5A0, st69_pos_tbl[r][0], st69_pos_tbl[r][1], st69_pos_tbl[r][2]);
                SetVector(v, st69_pos_tbl[r][3], st69_pos_tbl[r][4], st69_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            case 0x47: {
                int r = (u16)(em->x39A % 6);

                SetVector(em->pos, st71_pos_tbl[r][0], st71_pos_tbl[r][1], st71_pos_tbl[r][2]);
                SetVector(em->x5A0, st71_pos_tbl[r][0], st71_pos_tbl[r][1], st71_pos_tbl[r][2]);
                SetVector(v, st71_pos_tbl[r][3], st71_pos_tbl[r][4], st71_pos_tbl[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
                em_act_set(em, 7, 0);
                break;
            }
            default:
                if (em->boss == 0) {
                    em_act_set(em, 0, 0xC);
                } else {
                    em->x9E1 = 5;
                    em_act_set(em, 0, 0xA);
                }
                break;
            }
        } else {
            em->x04++;
        }
        break;
    }
}

static void em_die05_005D5640(EMW *em, EM16W *w) {
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0x64;
        break;
    case 1:
        em->work08--;
        em->x798 = (f32)em->work08 / 100.0f;
        if (em->work08 <= 0) {
            em16_init2(em);
        }
        break;
    }
}

void em_die06(EMW *em, EM16W *w) {
    em->x95C = 2;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        w->x60 = 1;
        Quest_enemy_die(em);
        em->x388 = 2;
        em_char_set(em, 0x2D, 0, 0);
        em->x7D6 = 1;
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        em->x798 -= 0.025f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em->x05++;
            em_act_set(em, 5, 4);
        }
        break;
    }
}

void em_die07(EMW *em, EM16W *w) {
    em->x95C = 2;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        w->x60 = 1;
        Quest_enemy_die(em);
        em->x388 = 2;
        em_char_set(em, 0x2D, 0, 0);
        em->x7D6 = 1;
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] + 0x8000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        em->x798 -= 0.025f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em->x05++;
            em_act_set(em, 5, 4);
        }
        break;
    }
}

void em_die08(EMW *em, EM16W *w) {
    em->x95C = 2;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        w->x60 = 1;
        Quest_enemy_die(em);
        em->x388 = 2;
        em_char_set(em, 0x2D, 0, 0);
        em->x7D6 = 1;
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] - 0x4000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        em->x798 -= 0.025f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em->x05++;
            em_act_set(em, 5, 4);
        }
        break;
    }
}

void em_die09(EMW *em, EM16W *w) {
    em->x95C = 2;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        w->x60 = 1;
        Quest_enemy_die(em);
        em->x388 = 2;
        em_char_set(em, 0x2D, 0, 0);
        em->x7D6 = 1;
        w->spd[0] = 0;
        w->spd[1] = (u16)(em->ang[1] + 0x4000);
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 23.35f;
        em->adj_z = -27.0f;
        em->x3C0[1] = -2.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        em->x798 -= 0.025f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em->x05++;
            em_act_set(em, 5, 4);
        }
        break;
    }
}

void em_move05_005D5BD0(EMW *em, EM16W *w) {
    em->x40C = 10;
    em->x40E = 10;
    switch (em->x15) {
    case 0: em_die00_005D3F90(em, w); break;
    case 1: em_die01_005D41B0(em, w); break;
    case 2: em_die02_005D43D0(em, w); break;
    case 3: em_die03_005D45F0(em, w); break;
    case 4: em_die04_005D4810(em, w); break;
    case 5: em_die05_005D5640(em, w); break;
    case 6: em_die06(em, w); break;
    case 7: em_die07(em, w); break;
    case 8: em_die08(em, w); break;
    case 9: em_die09(em, w); break;
    }
}

static void em_demo00_005D5CC0(EMW *em, EM16W *w) {
    u16 d;

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em->pos[0] = 8460.0f;
        em->pos[1] = 0.0f;
        em->pos[2] = 9367.0f;
        em->tgt_pos[0] = 8212.0f;
        em->tgt_pos[1] = 0.0f;
        em->tgt_pos[2] = 9356.0f;
        em->ang[0] = 0;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[2] = 0;
        em_char_set(em, 0x35, 0, 0);
        em->work08 = 0x1C2;
        break;
    case 1:
        if (--em->work08 <= 0 && em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x32, 0, 0);
        }
        break;
    case 4:
        if (em->x1C4 != 0) {
            break;
        }
        d = (u16)(w->tgt_ang - (u16)em->ang[1]);
        if (em_frame_check2(em, 0, 24.0f) != 0 && em_frame_check2(em, 0, 44.0f) == 0) {
            if ((u16)(d + 0x666) < 0xCCC) {
                em->ang[1] = w->tgt_ang;
            } else {
                em->ang[1] = (u16)(em->ang[1] - 0x666);
            }
        }
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 6, 0, 0);
            em->tgt_pos[0] = 8836.0f;
            em->tgt_pos[1] = 0.0f;
            em->tgt_pos[2] = 9382.0f;
        }
        break;
    case 5:
        if (em->x1C4 != 0) {
            break;
        }
        d = (u16)(w->tgt_ang - (u16)em->ang[1]);
        if (em_frame_check2(em, 0, 24.0f) != 0 && em_frame_check2(em, 0, 44.0f) == 0) {
            if ((u16)(d + 0x666) < 0xCCC) {
                em->ang[1] = w->tgt_ang;
            } else {
                em->ang[1] = (u16)(em->ang[1] - 0x666);
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x33, 0, 0);
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0xB, 0, 0);
        }
        break;
    case 7:
        if (Event_flag_ck(0xA) == 1) {
            em->x05++;
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_demo01_005D6000(EMW *em, EM16W *w) {
    f32 dist;

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em->pos[0] = 8200.0f;
        em->pos[1] = 0.0f;
        em->pos[2] = 7300.0f;
        em->tgt_pos[0] = 7730.0f;
        em->tgt_pos[1] = 0.0f;
        em->tgt_pos[2] = 9300.0f;
        em->ang[0] = 0;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[2] = 0;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0x2EE;
        em->ex[0x90] = 0;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 2, 0, 0);
            em->work08 = 0x12C;
        }
        break;
    case 2:
        dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || dist <= 100.0f) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 3:
        if (Event_flag_ck(0xA) == 1) {
            em->x05++;
            em16_to_normal(em, 0, 0);
            em->ex[0x90] = 1;
        }
        break;
    }
}

static void em_demo02_005D61E0(EMW *em, EM16W *w) {
    f32 dist;

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em->pos[0] = 8340.0f;
        em->pos[1] = 300.0f;
        em->pos[2] = 12470.0f;
        em->tgt_pos[0] = 8000.0f;
        em->tgt_pos[1] = 300.0f;
        em->tgt_pos[2] = 10850.0f;
        em->ang[0] = 0;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[2] = 0;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0x2EE;
        em->ex[0x90] = 0;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 2, 0, 0);
            em->work08 = 0x12C;
        }
        break;
    case 2:
        dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || dist <= 100.0f) {
            em->x05++;
            em_char_set(em, 0x32, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
            em->work08 = 0x12C;
        }
        break;
    case 4:
        if (Event_flag_ck(0xA) == 1) {
            em->x05++;
            em->ex[0x90] = 1;
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_demo03_005D63F0(EMW *em, EM16W *w) {
    switch (em->type) {
    case 0:
        em16_act_set(em, 6, 0, 0);
        break;
    case 1:
        em16_act_set(em, 6, 1, 0);
        break;
    case 2:
        em16_act_set(em, 6, 2, 0);
        break;
    }
}

static void em_demo04_005D6480(EMW *em, EM16W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 1, 0, 0);
        em->x01 = 0;
        em->x839 = 0;
        break;
    case 1:
        if (Event_flag_ck(0x13) == 1) {
            em->x05++;
            em->x01 = 1;
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_demo05_005D6530(EMW *em, EM16W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 1, 0, 0);
        em->x01 = 0;
        em->x839 = 0;
        break;
    case 1:
        if (Event_flag_ck(0x12) == 1) {
            em->x05++;
            em->x01 = 1;
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_move06_005D65E0(EMW *em, EM16W *w) {
    switch (em->x15) {
    case 0: em_demo00_005D5CC0(em, w); break;
    case 1: em_demo01_005D6000(em, w); break;
    case 2: em_demo02_005D61E0(em, w); break;
    case 3: em_demo03_005D63F0(em, w); break;
    case 4: em_demo04_005D6480(em, w); break;
    case 5: em_demo05_005D6530(em, w); break;
    }
}

static void em_revival00_005D6680(EMW *em, EM16W *w) {
    em->x9E1 = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        em_rate_clear(em);
        em->adj_y = 70.0f;
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
            em->x9E1 = 0;
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_revival01_005D6840(EMW *em, EM16W *w) {
    em->x9E1 = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        em_rate_clear(em);
        em->adj_y = 50.0f;
        em->adj_z = 30.0f;
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
        if (em->adj_z < 10.0f) {
            em->adj_z = 10.0f;
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
            em->x9E1 = 0;
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_revival02_005D6A00(EMW *em, EM16W *w) {
    em->x9E1 = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        em_rate_clear(em);
        em->adj_y = 80.0f;
        em->adj_z = 50.0f;
        em->x3C0[1] = -4.0f;
        em->x3C0[2] = -0.5f;
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
        if (em->adj_z < 30.0f) {
            em->adj_z = 30.0f;
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
            em->x9E1 = 0;
            em16_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_move07_005D6BC0(EMW *em, EM16W *w) {
    switch (em->x15) {
    case 0: em_revival00_005D6680(em, w); break;
    case 1: em_revival01_005D6840(em, w); break;
    case 2: em_revival02_005D6A00(em, w); break;
    }
}

void em16_main_sub(EMW *em, EM16W *w);

void em16_main(EMW *em) {
    EM16W *w = (EM16W *)em->ex;
    u8 dmg[4];
    u8 pull = 1;
    int yobi;
    int d;
    u8 r;

    em->act_spd = 1.2f;
    if (em->x9E1 != 0) {
        em->x40C = 5;
    }
    yobi = (u8)Em_Yobi_Ck(em, w->yobi);
    if (em->x9E1 == 0 && em->x888 == 0 && yobi != 0 && em_cancel_act_ck(em, 4) == 0) {
        em->x917 |= 4;
    }
    em_mode_timer_sub(em);
    r = Em_Dmg_Sys(em, dmg);
    switch (r) {
    case 0:
        pull = 0;
        break;
    case 3:
    case 4:
    case 7:
    case 9:
    case 11:
    case 13:
        break;
    case 1:
    case 2:
        if (em->x388 == 2 && r == 1 && (em->x788[0] & 2)) {
            d = (u16)(em->dm_ang - em->ang[1]);
            if (d >= 0x6000 && d < 0xA000) {
                em16_act_set(em, 5, 6, 0);
            } else if (d >= 0x2000 && d < 0x6000) {
                em16_act_set(em, 5, 8, 0);
            } else if (d >= 0xA000 && d < 0xE000) {
                em16_act_set(em, 5, 9, 0);
            } else {
                em16_act_set(em, 5, 7, 0);
            }
        } else {
            d = (u16)(em->dm_ang - em->ang[1]);
            if (d >= 0x6000 && d < 0xA000) {
                em16_act_set(em, 5, 0, 0);
            } else if (d >= 0x2000 && d < 0x6000) {
                em16_act_set(em, 5, 2, 0);
            } else if (d >= 0xA000 && d < 0xE000) {
                em16_act_set(em, 5, 3, 0);
            } else {
                em16_act_set(em, 5, 1, 0);
            }
        }
        break;
    case 5:
        if (!(em->mode == 4 && em->x15 == 1)) {
            em16_act_set(em, 4, 1, 0);
        }
        break;
    case 6:
        if (!(em->mode == 4 && em->x15 == 1) && !(em->mode == 0 && em->x15 == 6)) {
            em_mahi_dmg_timer_set(em);
            em16_act_set(em, 4, 6, 0);
        }
        break;
    case 8:
        if (!(em->mode == 4 && em->x15 == 1) && !(em->mode == 0 && em->x15 == 6)) {
            em_sleep_dmg_timer_set(em);
            em16_act_set(em, 0, 7, 0);
        }
        break;
    case 10:
        em16_act_set(em, 0, 8, 0);
        break;
    case 12:
            d = (u16)(em->dm_ang - em->ang[1]);
            if (d >= 0x6000 && d < 0xA000) {
                em16_act_set(em, 4, 1, 0);
            } else if (d >= 0x2000 && d < 0x6000) {
                em16_act_set(em, 4, 3, 0);
            } else if (d >= 0xA000 && d < 0xE000) {
                em16_act_set(em, 4, 4, 0);
            } else {
                em16_act_set(em, 4, 2, 0);
            }
        break;
    case 14:
        em->x839 = 0;
        if (em->x388 == 2) {
            d = em->dm_ang;
            if (d >= 0x6000 && d < 0xA000) {
                em16_act_set(em, 4, 1, 0);
            } else if (d >= 0x2000 && d < 0x6000) {
                em16_act_set(em, 4, 3, 0);
            } else if (d >= 0xA000 && d < 0xE000) {
                em16_act_set(em, 4, 4, 0);
            } else {
                em16_act_set(em, 4, 2, 0);
            }
        } else {
            em16_act_set(em, 4, 0, 0);
        }
        break;
    }
    if (pull) {
        if (w->yobi_st != 0) {
            pull_em_yobi(w->yobi);
            w->yobi_st = 0;
        }
    }
    switch (quest_w.x08) {
    case 0x88:
        if (em->stg == 0x23 && Event_flag_ck(0xA) == 0) {
            if (game_w.info_stop == 1 && em->mode != 6) {
                em16_act_set(em, 6, 3, 0);
            }
            break;
        }
        goto cmd;
    case 0x8A:
        if (em->stg == 0x28 && Event_flag_ck(0x13) == 0) {
            if (game_w.info_stop == 1 && em->mode != 6) {
                em16_act_set(em, 6, 4, 0);
            }
            break;
        }
        goto cmd;
    case 0xAB:
        if (em->stg == 0x35 && Event_flag_ck(0x12) == 0) {
            if (game_w.info_stop == 1 && em->mode != 6) {
                em16_act_set(em, 6, 5, 0);
            }
            break;
        }
        goto cmd;
    default:
    cmd:
        if (em->x839 != 0) {
            em_cmd_ck(em);
            em->x839 = 0;
        }
        break;
    }
    em16_main_sub(em, w);
    if (em->x6FF != 0) {
        em16_main_sub(em, w);
        em->x6FF = 0;
    }
}

void em16_main_sub(EMW *em, EM16W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->mode) {
    case 0: em_move00_005D1320(em, w); break;
    case 1: em_move01_005D1A80(em, w); break;
    case 2: em_move02_005D1F20(em, w); break;
    case 3: em_move03_005D3660(em, w); break;
    case 4: em_move04_005D3EE0(em, w); break;
    case 5: em_move05_005D5BD0(em, w); break;
    case 6: em_move06_005D65E0(em, w); break;
    case 7: em_move07_005D6BC0(em, w); break;
    }
}

#define UV_RESET(i) \
    do { \
        uv[i][0] = 0.0f; \
        uv[i][1] = 0.0f; \
        tm[i] = 0xFFFF; \
        ty[i] = 0xFF; \
    } while (0)

void em16_uvmove(EMW *em) {
    f32 (*uv)[3] = (f32 (*)[3])((u8 *)em + 0x5C0);
    u16 *tm = (u16 *)((u8 *)em + 0x5F0);
    u8 *ty = (u8 *)em + 0x5F8;
    int i;

    for (i = 0; i < 4; i++) {
        if (tm[i] != 0xFFFF) {
            tm[i]++;
        }
        switch (ty[i]) {
        case 0xFF:
            break;
        case 0:
            UV_RESET(i);
            break;
        case 1:
            if (tm[i] >= 0x3E) {
                UV_RESET(i);
            } else {
                int k = (tm[i] >> 1) + 1;

                uv[i][0] = 0.125f * (f32)(k % 8);
                uv[i][1] = 0.25f * (f32)(k / 8 % 4);
            }
            break;
        case 2:
            uv[i][0] = 0.125f;
            uv[i][1] = 0.0f;
            tm[i] = 0xFFFF;
            ty[i] = 0xFF;
            break;
        case 3:
            if (tm[i] >= 0xC) {
                UV_RESET(i);
            } else {
                int k = (tm[i] >> 1) + 2;

                uv[i][0] = 0.125f * (f32)(k % 4);
                uv[i][1] = 0.25f * (f32)(k / 4 % 4);
            }
            break;
        }
    }
}

static void sound_call_005D7610(EMW *em, int frame, int se, int joint) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, 0)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, 5, 0);
    }
}

static void move_default_005D76B0(EMW *em) {
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
void ef_move_sub_005D7700(EMW *em, EM16W *w) {
    f32 v[3];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_005D7610(em, 200, Code_Make(14, 2, 14, 2), 0);
        break;
    case 0x3EA:
        sound_call_005D7610(em, 8, 2, 0);
        sound_call_005D7610(em, 32, 2, 0);
        break;
    case 0x3EB:
        sound_call_005D7610(em, 20, 8, 0);
        sound_call_005D7610(em, 16, 2, 0);
        sound_call_005D7610(em, 20, 3, 0);
        if (em_frame_check(em, 18.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3EC:
        sound_call_005D7610(em, 22, 8, 0);
        sound_call_005D7610(em, 22, 2, 0);
        sound_call_005D7610(em, 22, 3, 0);
        if (em_frame_check(em, 20.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3ED:
        sound_call_005D7610(em, 4, 2, 0);
        sound_call_005D7610(em, 10, 1, 0);
        break;
    case 0x3EE:
    case 0x3EF:
        sound_call_005D7610(em, 6, Code_Make(5, 2, 5, 2), 0);
        sound_call_005D7610(em, 26, 2, 0);
        sound_call_005D7610(em, 26, 4, 0);
        sound_call_005D7610(em, 42, 2, 0);
        sound_call_005D7610(em, 48, 1, 0);
        break;
    case 0x3F0:
        sound_call_005D7610(em, 8, 2, 0);
        sound_call_005D7610(em, 8, 4, 0);
        sound_call_005D7610(em, 26, 2, 0);
        sound_call_005D7610(em, 32, 1, 0);
        sound_call_005D7610(em, 64, 7, 0);
        if (em_frame_check(em, 12.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3F1:
    case 0x3F2:
        sound_call_005D7610(em, 12, 2, 0);
        sound_call_005D7610(em, 12, 4, 0);
        sound_call_005D7610(em, 30, 2, 0);
        sound_call_005D7610(em, 36, 1, 0);
        sound_call_005D7610(em, 64, 7, 0);
        if (em_frame_check(em, 14.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3F3:
        sound_call_005D7610(em, 68, 17, 0);
        sound_call_005D7610(em, 164, 18, 0);
        sound_call_005D7610(em, 34, 23, 0);
        sound_call_005D7610(em, 260, 23, 0);
        sound_call_005D7610(em, 288, 23, 0);
        break;
    case 0x3F4:
        sound_call_005D7610(em, 118, 14, 0);
        break;
    case 0x3FC:
        sound_call_005D7610(em, 22, 2, 0);
        sound_call_005D7610(em, 22, 3, 0);
        sound_call_005D7610(em, 22, 8, 0);
        break;
    case 0x3FD:
        sound_call_005D7610(em, 4, 2, 0);
        sound_call_005D7610(em, 6, 1, 0);
        sound_call_005D7610(em, 36, 0, 0);
        break;
    case 0x3FE:
        sound_call_005D7610(em, 40, 2, 0);
        sound_call_005D7610(em, 60, 2, 0);
        sound_call_005D7610(em, 118, 2, 0);
        sound_call_005D7610(em, 26, 16, 0);
        break;
    case 0x3FF:
        sound_call_005D7610(em, 22, 2, 0);
        sound_call_005D7610(em, 22, 3, 0);
        sound_call_005D7610(em, 22, 8, 0);
        if (em_frame_check(em, 32.0f, 0)) {
            shell13_set(em, 2);
        }
        if (em_frame_check(em, 22.0f, 0)) {
            Eft13_set_em(em, 27, 0);
        }
        break;
    case 0x400:
        sound_call_005D7610(em, 4, 2, 0);
        sound_call_005D7610(em, 6, 1, 0);
        break;
    case 0x401:
        sound_call_005D7610(em, 2, 16, 0);
        break;
    case 0x402:
        sound_call_005D7610(em, 60, 0, 0);
        sound_call_005D7610(em, 128, 2, 0);
        sound_call_005D7610(em, 72, 28, 0);
        sound_call_005D7610(em, 60, 8, 0);
        break;
    case 0x406:
        sound_call_005D7610(em, 4, 9, 0);
        sound_call_005D7610(em, 30, 2, 0);
        sound_call_005D7610(em, 28, 1, 0);
        sound_call_005D7610(em, 54, 8, 0);
        break;
    case 0x407:
    case 0x409:
    case 0x40B:
    case 0x40D:
        sound_call_005D7610(em, 4, 11, 0);
        break;
    case 0x408:
    case 0x40A:
    case 0x40C:
    case 0x40E:
        sound_call_005D7610(em, 4, 15, 0);
        break;
    case 0x40F:
        sound_call_005D7610(em, 24, 3, 0);
        sound_call_005D7610(em, 44, 1, 0);
        sound_call_005D7610(em, 54, 2, 0);
        sound_call_005D7610(em, 88, 0, 0);
        break;
    case 0x412:
        sound_call_005D7610(em, 180, 9, 0);
        sound_call_005D7610(em, 132, 11, 0);
        sound_call_005D7610(em, 52, 10, 0);
        sound_call_005D7610(em, 68, 1, 0);
        sound_call_005D7610(em, 86, 0, 0);
        sound_call_005D7610(em, 132, 1, 0);
        sound_call_005D7610(em, 140, 0, 0);
        break;
    case 0x413:
        sound_call_005D7610(em, 4, 17, 0);
        sound_call_005D7610(em, 66, 17, 0);
        sound_call_005D7610(em, 130, 18, 0);
        sound_call_005D7610(em, 48, 23, 0);
        sound_call_005D7610(em, 122, 0, 0);
        sound_call_005D7610(em, 162, 15, 0);
        break;
    case 0x414:
        sound_call_005D7610(em, 4, 14, 0);
        v[1] = 10.0f;
        v[2] = 40.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 15, v, 1.0f);
        break;
    case 0x416:
        sound_call_005D7610(em, 10, 13, 0);
        break;
    case 0x417:
        sound_call_005D7610(em, 14, 25, 0);
        break;
    case 0x41A:
        sound_call_005D7610(em, 32, 12, 0);
        sound_call_005D7610(em, 68, 12, 0);
        sound_call_005D7610(em, 98, 6, 0);
        sound_call_005D7610(em, 20, 0, 0);
        sound_call_005D7610(em, 160, 23, 0);
        break;
    case 0x41B:
        sound_call_005D7610(em, 10, 7, 0);
        sound_call_005D7610(em, 68, 7, 0);
        sound_call_005D7610(em, 6, 1, 0);
        sound_call_005D7610(em, 122, 1, 0);
        sound_call_005D7610(em, 134, 0, 0);
        break;
    case 0x41C:
        sound_call_005D7610(em, 8, 5, 0);
        sound_call_005D7610(em, 98, 5, 0);
        sound_call_005D7610(em, 22, 1, 0);
        sound_call_005D7610(em, 58, 0, 0);
        sound_call_005D7610(em, 106, 1, 0);
        sound_call_005D7610(em, 158, 0, 0);
        break;
    case 0x41D:
        break;
    default:
        move_default_005D76B0(em);
        break;
    }
}

void em16_effect_move(EMW *em) {
    EM16W *w = (EM16W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_005D7700(em, w);
        break;
    }
    em16_uvmove(em);
}

void dummy_em_prog_005D8190(void) {
}
