/* em27 - game.bin 0x0060D440-0x006139D0: per-monster AI for monster kind 27 (also 28 and 31).
 * Same layout as the other monsters: em27_init, em27_to_normal (back to the idle step), action
 * steps em_act00-11, move states (em_mv00-05 turning/walking, fly), attacks em_atk00-13 (shells),
 * damage reactions, death, demo, revival, em27_main (damage system), em27_uvmove, the
 * sound/effect script. The action setters and fly_adjy2 are in em27.c. Field meanings are guesses. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"

/* Per-monster work at EMW+0x444 (em27.c has the same start). */
typedef struct EM27W {
    u8 eff;             /* 0x00 em27_effect_move step */
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
    u8 _pad30[4];
    u8 yobi_stg;        /* 0x34 stage */
    u8 _pad35[3];
    s32 x38;            /* 0x38 */
    s32 x3C;            /* 0x3C */
    f32 x40;            /* 0x40 saved 0x3BC */
    s32 x44;            /* 0x44 */
    s32 x48;            /* 0x48 */
    s32 x4C;            /* 0x4C */
} EM27W;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;
extern s16 em27_stay_timer_tbl[];
extern s16 em27_runaway_timer_tbl[];
extern u8 em27_act_tbl[8];

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set2(EMW *, s16, s16);
void em_dur_init(EMW *);
u16 em_act_search(void *);
void em27_act_set(EMW *em, int kind, u16 no, u16 arg);
void em27_to_normal(EMW *em, s16 a, s16 b);
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
u8 em27_fly_adjy2(EMW *);
void em27_fly_adjy2_init(EMW *, u8);
void shell15_set(EMW *, int);
int em_mode_timer_sub(EMW *);
void em_no_floor_ck2(EMW *);
void em_no_battle_area_ck(EMW *, int, int);
int Em_Yobi_Ck(EMW *, f32 *);
int em_cancel_act_ck(EMW *, u8);
void em_hinshi_ck(EMW *, f32);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void em_cmd_ck(EMW *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);
int Code_Make(int, int, int, int);
void Eft13_set_em(EMW *, int, int);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void em27_init(EMW *);
int Event_flag_ck(int);
extern f32 st01_pos_tbl[6];
extern f32 st34_pos_tbl[6];
extern f32 st38_pos_tbl_00671470[][6];
extern f32 st39_pos_tbl_006714A0[][6];
extern f32 st53_pos_tbl_00671500[6];
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void em_cmd_reset(EMW *);
void Em_Mahi_Start(EMW *);
void em_mahi_eff_set(EMW *, int);
f32 CalcDistanceXZ(f32 *, f32 *);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);

void em27_local_init(EMW *em) {
    eft01_set((PLW *)em, 0);
}

void em27_init(EMW *em) {
    EM27W *w = (EM27W *)em->ex;

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
        case 34:
            em->pos[0] = 9522.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9559.0f + 150.0f * (f32)(em->x13 & 7);
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
    em_act_set(em, 0, 1);
    switch (em->kind) {
    case 27:
        em->x792 = em->x302 = em_hp_vital_set2(em, 0x104, 0xF0);
        break;
    case 28:
        em->x792 = em->x302 = em_hp_vital_set2(em, 0x96, 0x1F4);
        break;
    case 31:
        em->x792 = em->x302 = em_hp_vital_set2(em, 0x29A, 0x14E);
        break;
    }
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->stay_tm = em27_stay_timer_tbl[em->stg];
    em->runaway_tm = em27_runaway_timer_tbl[em->stg];
    w->yobi_st = 0;
    ((u8 *)w)[0x50] = 0;
    em_dur_init(em);
    em->x734 = 3;
}

void em27_to_normal(EMW *em, s16 a, s16 b) {
    if (em->x734 == 3) {
        em->act_spd = 1.2f;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x839 = 1;
        if (em->x302 < (s16)(0.3f * (f32)em->x792)) {
            em27_act_set(em, 0, 1, 0);
        } else if (em->x888 == 0) {
            em27_act_set(em, 0, 1, 0);
        } else {
            em27_act_set(em, 0, 5, 0);
        }
        return;
    }
    if (em->char0 != 0x3E9) {
        em_char_set(em, 1, a, b);
    }
    em->x388 = 0;
    em->x3F4 = 0;
    em27_act_set(em, 0, 1, 0);
}

static void em_act00_0060D890(EMW *em, EM27W *w) {
}

static void em_act01_0060D8A0(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            if (em->x734 == 3) {
                em->x839 = 1;
            } else {
                u16 a = em_act_search(em27_act_tbl);

                if (a > 1) {
                    em_act_set(em, 0, a);
                }
            }
        }
        break;
    }
}

static void em_act02_0060D960(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x32, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act03_0060D9E0(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x33, 0, 0);
        break;
    case 1:
        if (em_frame_check2(em, 0, 40.0f) != 0) {
            SetVector(w->yobi, em->pos[0], em->pos[1], em->pos[2]);
            w->yobi_r = 1000.0f;
            w->yobi_stg = em->stg;
            push_em_yobi(w->yobi);
            w->yobi_st = 1;
        }
        if (em->x194 == 0) {
            if (w->yobi_st != 0) {
                pull_em_yobi(w->yobi);
                w->yobi_st = 0;
            }
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act04_0060DAF0(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x34, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act05_0060DB70(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0xB, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act06_0060DBF0(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x27, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act07_0060DC70(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
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
            em->x05++;
            em27_act_set(em, 0, 8, 4);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em27_act_set(em, 0, 8, 4);
        }
        break;
    }
}

static void em_act08_0060DD70(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x27, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act09_0060DE10(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        if (em->char0 != 0x412) {
            em_char_set(em, 0x2A, 0, 0);
        }
        break;
    case 1:
        if (em->x94E == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act10_0060DEA0(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xB, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act11_0060DF20(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0xC, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_move00_0060DF90(EMW *em, EM27W *w) {
    switch (em->x15) {
    case 0: em_act00_0060D890(em, w); break;
    case 1: em_act01_0060D8A0(em, w); break;
    case 2: em_act02_0060D960(em, w); break;
    case 3: em_act03_0060D9E0(em, w); break;
    case 4: em_act04_0060DAF0(em, w); break;
    case 5: em_act05_0060DB70(em, w); break;
    case 6: em_act06_0060DBF0(em, w); break;
    case 7: em_act07_0060DC70(em, w); break;
    case 8: em_act08_0060DD70(em, w); break;
    case 9: em_act09_0060DE10(em, w); break;
    case 10: em_act10_0060DEA0(em, w); break;
    case 11: em_act11_0060DF20(em, w); break;
    }
}

static void em_mv00_0060E090(EMW *em, EM27W *w) {
    u32 k;
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
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
            k = (u32)(1820.0f * em->act_spd);
            d = (u16)(w->tgt_ang - (u16)em->ang[1]);
            if (em->x194 == 0) {
                if ((u16)(d + k) < k * 2) {
                    em->x05++;
                    em27_to_normal(em, 0, 0);
                } else if (d >= 0x8000) {
                    em_char_set(em, 6, 0, 0);
                } else {
                    em_char_set(em, 7, 0, 0);
                }
            } else if (em_frame_check2(em, 0, 24.0f) != 0 && em_frame_check2(em, 0, 42.0f) == 0) {
                if ((u16)(d + k) < k * 2) {
                    em->ang[1] = w->tgt_ang;
                } else if (d < 0x8000) {
                    em->ang[1] = (u16)(em->ang[1] + k);
                } else {
                    em->ang[1] = (u16)(em->ang[1] - k);
                }
            }
        }
        break;
    }
}

#define EM27_TURN2(em, L, H) \
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
#define EM27_TURN(em, L) EM27_TURN2(em, L, 0x10000 - L)

static void em_mv01_0060E300(EMW *em, EM27W *w) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM27_TURN2(em, 0x80, 0x10080);
        }
        mot_miration_ret(em, v);
        w->dist -= v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv02_0060E440(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 8, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv03_0060E4C0(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 9, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv04_0060E540(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0xA, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv05_0060E5C0(EMW *em, EM27W *w) {
    u32 k;
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
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
            k = (u32)(1820.0f * em->act_spd);
            d = (u16)(w->tgt_ang - (u16)em->ang[1]);
            if (em->x194 == 0) {
                if ((u16)(d + k) < k * 2) {
                    em->x05++;
                    em27_to_normal(em, 0, 0);
                } else if (d >= 0x8000) {
                    em_char_set(em, 6, 0, 0);
                } else {
                    em_char_set(em, 7, 0, 0);
                }
            } else if (em_frame_check2(em, 0, 24.0f) != 0 && em_frame_check2(em, 0, 42.0f) == 0) {
                if ((u16)(d + k) < k * 2) {
                    em->ang[1] = w->tgt_ang;
                } else if (d < 0x8000) {
                    em->ang[1] = (u16)(em->ang[1] + k);
                } else {
                    em->ang[1] = (u16)(em->ang[1] - k);
                }
            }
        }
        break;
    }
}

void em_move01_0060E830(EMW *em, EM27W *w) {
    switch (em->x15) {
    case 0: em_mv00_0060E090(em, w); break;
    case 1: em_mv01_0060E300(em, w); break;
    case 2: em_mv02_0060E440(em, w); break;
    case 3: em_mv03_0060E4C0(em, w); break;
    case 4: em_mv04_0060E540(em, w); break;
    case 5: em_mv05_0060E5C0(em, w); break;
    }
}

static void em_fly00_0060E8D0(EMW *em, EM27W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        em27_fly_adjy2_init(em, 2);
        break;
    case 1:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x388 = 2;
            landed = em27_fly_adjy2(em) & 0xFF;
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_fly01_0060EA00(EMW *em, EM27W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 4, 0, 0);
        em27_fly_adjy2_init(em, 3);
        break;
    case 1:
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            landed = em27_fly_adjy2(em) & 0xFF;
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_move02_0060EB30(EMW *em, EM27W *w) {
    switch (em->x15) {
    case 0: em_fly00_0060E8D0(em, w); break;
    case 1: em_fly01_0060EA00(em, w); break;
    }
}

static void em_atk00_0060EB80(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            switch (em->kind) {
            case 27:
                shell15_set(em, 1);
                break;
            case 28:
                shell15_set(em, 0xD);
                break;
            case 31:
                shell15_set(em, 7);
                break;
            }
        }
        if (w->has_tgt != 0) {
            EM27_TURN(em, 0x80);
        }
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk01_0060ED30(EMW *em, EM27W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x17, 0, 0);
        em27_fly_adjy2_init(em, 1);
        break;
    case 1:
        if (em_frame_check2(em, 0, 24.0f) != 0) {
            em->x388 = 2;
            landed = em27_fly_adjy2(em) & 0xFF;
        }
        if (w->has_tgt != 0 && em->x388 == 2) {
            EM27_TURN(em, 0x40);
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x18, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk02_0060EEF0(EMW *em, EM27W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        em->adj_y = 75.0f;
        em->x3C0[1] = -10.0f;
        em->adj_z = 50.0f;
        em->x3C0[2] = -1.5f;
        w->spd[0] = 0;
        w->spd[2] = 0;
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
            case 27:
                shell15_set(em, 3);
                break;
            case 28:
                shell15_set(em, 8);
                break;
            case 31:
                shell15_set(em, 9);
                break;
            }
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[1] = em->ang[1];
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
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk03_0060F160(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 12.0f, 0) != 0) {
            switch (em->kind) {
            case 27:
                shell15_set(em, 5);
                break;
            case 28:
                shell15_set(em, 0xA);
                break;
            case 31:
                shell15_set(em, 0xB);
                break;
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk04_0060F280(EMW *em, EM27W *w) {
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
            case 27:
                shell15_set(em, 3);
                break;
            case 28:
                shell15_set(em, 8);
                break;
            case 31:
                shell15_set(em, 9);
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
            EM27_TURN(em, 0x100);
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
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk05_0060F570(EMW *em, EM27W *w) {
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
            case 27:
                shell15_set(em, 3);
                break;
            case 28:
                shell15_set(em, 8);
                break;
            case 31:
                shell15_set(em, 9);
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
            EM27_TURN(em, 0x100);
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
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk06_0060F860(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1A, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 80.0f, 0) != 0) {
            Shell08_set_ang(em, 0xF, 9, 1, 0xF8E5, 0);
        }
        if (em->x194 == 0) {
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk07_0060F910(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell15_set(em, 6);
        }
        if (w->has_tgt != 0) {
            EM27_TURN(em, 0x80);
        }
        if (em->x194 == 0) {
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk08_0060FA40(EMW *em, EM27W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        em->adj_y = 75.0f;
        em->x3C0[1] = -10.0f;
        em->adj_z = 50.0f;
        em->x3C0[2] = -1.5f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        w->x38 = 0;
        w->x3C = 0;
        w->x40 = 0.0f;
        w->x44 = 0;
        w->x48 = 0;
        w->x4C = 0;
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell15_set(em, 3);
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[1] = em->ang[1];
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
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk09_0060FC40(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 12.0f, 0) != 0) {
            shell15_set(em, 5);
        }
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk10_0060FCF0(EMW *em, EM27W *w) {
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
            shell15_set(em, 3);
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
            EM27_TURN(em, 0x100);
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
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk11_0060FF80(EMW *em, EM27W *w) {
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
            shell15_set(em, 3);
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
            EM27_TURN(em, 0x100);
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
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk12_00610210(EMW *em, EM27W *w) {
    u8 landed = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x14, 0, 0);
        em_rate_clear(em);
        if (!(em->pos[1] < em->tgt_pos[1])) {
            em->adj_y = 75.0f;
            em->x3C0[1] = -10.0f;
            em->adj_z = 50.0f;
            em->x3C0[2] = -1.5f;
        } else {
            f32 dist = CalcDistanceXZ(em->pos, em->tgt_pos);

            em->adj_z = 100.0f;
            em->x3C0[1] = -10.0f;
            em->work08 = dist / em->adj_z;
            em->adj_y = ((1000.0f + em->tgt_pos[1]) - em->pos[1]) / (f32)em->work08 - (em->x3C0[1] * (f32)em->work08) / 2.0f;
        }
        w->spd[0] = 0;
        w->spd[2] = 0;
        w->x38 = 0;
        w->x3C = 0;
        w->x40 = 0.0f;
        w->x44 = 0;
        w->x48 = 0;
        w->x4C = 0;
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell15_set(em, 3);
        }
        if (em_frame_check2(em, 0, 22.0f) != 0) {
            em->x388 = 2;
            w->spd[1] = em->ang[1];
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
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x15, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk13_006104A0(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 40.0f, 0) != 0) {
            shell15_set(em, 0xC);
        }
        if (w->has_tgt != 0) {
            EM27_TURN(em, 0x80);
        }
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_move03_006105E0(EMW *em, EM27W *w) {
    switch (em->x15) {
    case 0: em_atk00_0060EB80(em, w); break;
    case 1: em_atk01_0060ED30(em, w); break;
    case 2: em_atk02_0060EEF0(em, w); break;
    case 3: em_atk03_0060F160(em, w); break;
    case 4: em_atk04_0060F280(em, w); break;
    case 5: em_atk05_0060F570(em, w); break;
    case 6: em_atk06_0060F860(em, w); break;
    case 7: em_atk07_0060F910(em, w); break;
    case 8: em_atk08_0060FA40(em, w); break;
    case 9: em_atk09_0060FC40(em, w); break;
    case 10: em_atk10_0060FCF0(em, w); break;
    case 11: em_atk11_0060FF80(em, w); break;
    case 12: em_atk12_00610210(em, w); break;
    case 13: em_atk13_006104A0(em, w); break;
    }
}

static void em_dmg00_00610700(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x1E, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg01_00610790(EMW *em, EM27W *w) {
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
            em->x05++;
            em27_act_set(em, 0, 6, 0);
        }
        break;
    }
}

static void em_dmg02_006108E0(EMW *em, EM27W *w) {
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
            em->x05++;
            em27_act_set(em, 0, 6, 0);
        }
        break;
    }
}

static void em_dmg03_00610A70(EMW *em, EM27W *w) {
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
            em->x05++;
            em27_act_set(em, 0, 6, 0);
        }
        break;
    }
}

static void em_dmg04_00610C00(EMW *em, EM27W *w) {
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
            em->x05++;
            em27_act_set(em, 0, 6, 0);
        }
        break;
    }
}

static void em_dmg05_00610D90(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x2A, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg06_00610E20(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x2F, 0, 0);
        Em_Mahi_Start(em);
        em_cmd_reset(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            Em_Mahi_End(em);
            em->x05++;
            em27_act_set(em, 0, 0xA, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em27_act_set(em, 0, 0xA, 4);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

void em_move04_00610F10(EMW *em, EM27W *w) {
    switch (em->x15) {
    case 0: em_dmg00_00610700(em, w); break;
    case 1: em_dmg01_00610790(em, w); break;
    case 2: em_dmg02_006108E0(em, w); break;
    case 3: em_dmg03_00610A70(em, w); break;
    case 4: em_dmg04_00610C00(em, w); break;
    case 5: em_dmg05_00610D90(em, w); break;
    case 6: em_dmg06_00610E20(em, w); break;
    }
}

static void em_die00_00610FC0(EMW *em, EM27W *w) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        ((u8 *)w)[0x50] = 1;
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
            em->work08 = 0xA8C;
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
            em->x798 = 0.0f;
            em->x05++;
        }
        break;
    case 6:
        if (em->x8C3 == 0) {
            em->x05++;
            em27_act_set(em, 5, 4, 2);
        }
        break;
    }
}

static void em_die01_00611200(EMW *em, EM27W *w) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        ((u8 *)w)[0x50] = 1;
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
            em->work08 = 0xA8C;
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
            em->x798 = 0.0f;
            em->x05++;
        }
        break;
    case 6:
        if (em->x8C3 == 0) {
            em->x05++;
            em27_act_set(em, 5, 4, 2);
        }
        break;
    }
}

static void em_die02_00611450(EMW *em, EM27W *w) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        ((u8 *)w)[0x50] = 1;
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
            em->work08 = 0xA8C;
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
            em->x798 = 0.0f;
            em->x05++;
        }
        break;
    case 6:
        if (em->x8C3 == 0) {
            em->x05++;
            em27_act_set(em, 5, 4, 2);
        }
        break;
    }
}

static void em_die03_006116A0(EMW *em, EM27W *w) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        ((u8 *)w)[0x50] = 1;
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
            em->work08 = 0xA8C;
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
            em->x798 = 0.0f;
            em->x05++;
        }
        break;
    case 6:
        if (em->x8C3 == 0) {
            em->x05++;
            em27_act_set(em, 5, 4, 2);
        }
        break;
    }
}

static void em_die04_006118F0(EMW *em, EM27W *w) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        if (Quest_enemy_revival_ck(em) == 1) {
            Quest_enemy_revival_set(em);
            em_status_init(em);
            em27_init(em);
            em_cmd_reset(em);
            em->x839 = 0;
            switch (em->stg) {
            case 1:
                SetVector(em->pos, st01_pos_tbl[0], st01_pos_tbl[1], st01_pos_tbl[2]);
                SetVector(em->x5A0, st01_pos_tbl[0], st01_pos_tbl[1], st01_pos_tbl[2]);
                SetVector(v, st01_pos_tbl[3], st01_pos_tbl[4], st01_pos_tbl[5]);
                em->ang[1] = Em_Calc_angY(em->pos, v);
                em_act_set(em, 7, 0);
                break;
            case 0x22:
                SetVector(em->pos, st34_pos_tbl[0], st34_pos_tbl[1], st34_pos_tbl[2]);
                SetVector(em->x5A0, st34_pos_tbl[0], st34_pos_tbl[1], st34_pos_tbl[2]);
                SetVector(v, st34_pos_tbl[3], st34_pos_tbl[4], st34_pos_tbl[5]);
                em->ang[1] = Em_Calc_angY(em->pos, v);
                em_act_set(em, 7, 0);
                break;
            case 0x26: {
                int r = (u16)(em->x39A % 2);

                SetVector(em->pos, st38_pos_tbl_00671470[r][0], st38_pos_tbl_00671470[r][1], st38_pos_tbl_00671470[r][2]);
                SetVector(em->x5A0, st38_pos_tbl_00671470[r][0], st38_pos_tbl_00671470[r][1], st38_pos_tbl_00671470[r][2]);
                SetVector(v, st38_pos_tbl_00671470[r][3], st38_pos_tbl_00671470[r][4], st38_pos_tbl_00671470[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v);
                em_act_set(em, 7, 0);
                break;
            }
            case 0x27: {
                int r = (u16)(em->x39A % 4);

                SetVector(em->pos, st39_pos_tbl_006714A0[r][0], st39_pos_tbl_006714A0[r][1], st39_pos_tbl_006714A0[r][2]);
                SetVector(em->x5A0, st39_pos_tbl_006714A0[r][0], st39_pos_tbl_006714A0[r][1], st39_pos_tbl_006714A0[r][2]);
                SetVector(v, st39_pos_tbl_006714A0[r][3], st39_pos_tbl_006714A0[r][4], st39_pos_tbl_006714A0[r][5]);
                em->ang[1] = Em_Calc_angY(em->pos, v);
                em_act_set(em, 7, 0);
                break;
            }
            case 0x35:
                SetVector(em->pos, st53_pos_tbl_00671500[0], st53_pos_tbl_00671500[1], st53_pos_tbl_00671500[2]);
                SetVector(em->x5A0, st53_pos_tbl_00671500[0], st53_pos_tbl_00671500[1], st53_pos_tbl_00671500[2]);
                SetVector(v, st53_pos_tbl_00671500[3], st53_pos_tbl_00671500[4], st53_pos_tbl_00671500[5]);
                em->ang[1] = Em_Calc_angY(em->pos, v);
                em_act_set(em, 0, 0xB);
                break;
            default:
                em_act_set(em, 0, 0xB);
                break;
            }
        } else {
            em->x04++;
        }
        break;
    }
}

void em_move05_00611D40(EMW *em, EM27W *w) {
    em->x40C = 10;
    em->x40E = 10;
    switch (em->x15) {
    case 0: em_die00_00610FC0(em, w); break;
    case 1: em_die01_00611200(em, w); break;
    case 2: em_die02_00611450(em, w); break;
    case 3: em_die03_006116A0(em, w); break;
    case 4: em_die04_006118F0(em, w); break;
    }
}

static void em_demo00_00611DF0(EMW *em, EM27W *w) {
    f32 dist;

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em->ex[0x90] = 0;
        em->pos[0] = 7370.0f;
        em->pos[1] = 680.0f;
        em->pos[2] = 5800.0f;
        em->tgt_pos[0] = 7550.0f;
        em->tgt_pos[1] = 680.0f;
        em->tgt_pos[2] = 7200.0f;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos);
        em_char_set(em, 2, 0, 0);
        em->work08 = 0x12C;
        break;
    case 1:
        dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || dist <= 100.0f) {
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
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em->tgt_pos[0] = 9150.0f;
            em->tgt_pos[1] = 0.0f;
            em->tgt_pos[2] = 9850.0f;
            em_char_set(em, 3, 0, 0);
        }
        break;
    case 4:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x05++;
            em->x388 = 2;
            em_rate_clear(em);
            em->adj_y = 75.0f;
            em->x3C0[1] = -5.0f;
            em->adj_z = 69.0f;
            em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos);
            w->spd[0] = 0;
            w->spd[2] = 0;
            em->ex[0x90] = 0;
        }
        break;
    case 5:
        w->spd[1] = em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 5, 0, 0);
            em_rate_clear(em);
            em->ex[0x90] = 1;
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x33, 0, 0);
        }
        break;
    case 7:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 8:
        if (Event_flag_ck(0xC) == 1) {
            em->x05++;
            em->x9E1 = 0;
            em->x40C = 0;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_move06_00612130(EMW *em, EM27W *w) {
    switch (em->x15) {
    case 0:
        em_demo00_00611DF0(em, w);
        break;
    }
}

static void em_revival00_00612170(EMW *em, EM27W *w) {
    u8 landed = 0;

    em->x9E1 = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x839 = 0;
        em_char_set(em, 3, 0, 0);
        em27_fly_adjy2_init(em, 2);
        break;
    case 1:
        if (em_frame_check2(em, 0, 10.0f) != 0) {
            em->x388 = 2;
            landed = em27_fly_adjy2(em) & 0xFF;
        }
        if (landed && em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x9E1 = 0;
            em->x40C = 5;
            em->x05++;
            em27_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_revival01_006122B0(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em27_act_set(em, 2, 0, 0);
        break;
    }
}

static void em_revival02_00612300(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em27_act_set(em, 0, 0, 0);
        break;
    }
}

void em_revival03(EMW *em, EM27W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em27_act_set(em, 0, 0, 0);
        break;
    }
}

void em_move07_006123A0(EMW *em, EM27W *w) {
    switch (em->x15) {
    case 0: em_revival00_00612170(em, w); break;
    case 1: em_revival01_006122B0(em, w); break;
    case 2: em_revival02_00612300(em, w); break;
    case 3: em_revival03(em, w); break;
    }
}

void em27_main_sub(EMW *em, EM27W *w);

void em27_main(EMW *em) {
    EM27W *w = (EM27W *)em->ex;
    u8 dmg[4];
    s8 pn;
    int yobi;
    int d;

    if (em->mode < 5) {
        em->x798 = 1.0f;
        em->x01 = 1;
    }
    em->act_spd = 1.2f;
    if (em->x9E1 != 0) {
        em->x40C = 5;
    }
    if (em_mode_timer_sub(em)) {
        em_cmd_reset(em);
        for (pn = 0; pn < game_w.pl_num; pn++) {
            if (em->stg == ((PLW *)player_work)[pn].stg) {
                break;
            }
            if (pn == game_w.pl_num - 1) {
                em->x839 = 1;
            }
        }
    }
    em_no_floor_ck2(em);
    em_no_battle_area_ck(em, 0, 1);
    yobi = (u8)Em_Yobi_Ck(em, w->yobi);
    if (em->x9E1 == 0 && em->x888 == 0 && yobi != 0 && em_cancel_act_ck(em, 4) == 0) {
        em->x917 |= 4;
    }
    if (em->x8C2 != 1 && em->x888 == 1) {
        em_hinshi_ck(em, 0.2f);
    }
    switch ((u8)Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 3:
    case 4:
    case 7:
    case 9:
    case 11:
    case 13:
        break;
    case 1:
    case 2:
        d = (u16)(em->dm_ang - em->ang[1]);
        if (d >= 0x6000 && d < 0xA000) {
            em27_act_set(em, 5, 0, 2);
        } else if (d >= 0x2000 && d < 0x6000) {
            em27_act_set(em, 5, 2, 2);
        } else if (d >= 0xA000 && d < 0xE000) {
            em27_act_set(em, 5, 3, 2);
        } else {
            em27_act_set(em, 5, 1, 2);
        }
        break;
    case 5:
        if ((em->mode == 4 && em->x15 == 1) || (em->mode == 4 && em->x15 == 2) || (em->mode == 4 && em->x15 == 3)
            || (em->mode == 4 && em->x15 == 4)) {
            break;
        }
        d = (u16)(em->dm_ang - em->ang[1]);
        if (d >= 0x6000 && d < 0xA000) {
            em27_act_set(em, 4, 1, 2);
        } else if (d >= 0x2000 && d < 0x6000) {
            em27_act_set(em, 4, 3, 2);
        } else if (d >= 0xA000 && d < 0xE000) {
            em27_act_set(em, 4, 4, 2);
        } else {
            em27_act_set(em, 4, 2, 2);
        }
        break;
    case 6:
        em_cmd_reset(em);
        em_mahi_dmg_timer_set(em);
        em27_act_set(em, 4, 6, 2);
        break;
    case 8:
        em_sleep_dmg_timer_set(em);
        em27_act_set(em, 0, 7, 2);
        break;
    case 10:
        em_cmd_reset(em);
        em27_act_set(em, 0, 8, 2);
        break;
    case 12:
        d = (u16)(em->dm_ang - em->ang[1]);
        if (d >= 0x6000 && d < 0xA000) {
            em27_act_set(em, 4, 1, 2);
        } else if (d >= 0x2000 && d < 0x6000) {
            em27_act_set(em, 4, 3, 2);
        } else if (d >= 0xA000 && d < 0xE000) {
            em27_act_set(em, 4, 4, 2);
        } else {
            em27_act_set(em, 4, 2, 2);
        }
        break;
    case 14:
        em->x839 = 0;
        if (em->x388 == 2) {
            d = em->dm_ang;
            if (d >= 0x6000 && d < 0xA000) {
                em27_act_set(em, 4, 1, 2);
            } else if (d >= 0x2000 && d < 0x6000) {
                em27_act_set(em, 4, 3, 2);
            } else if (d >= 0xA000 && d < 0xE000) {
                em27_act_set(em, 4, 4, 2);
            } else {
                em27_act_set(em, 4, 2, 2);
            }
        } else {
            em27_act_set(em, 4, 0, 2);
        }
        break;
    }
    if (quest_w.x08 == 0x89 && em->stg == 0x22 && Event_flag_ck(0xC) == 0) {
        if (game_w.info_stop == 1 && em->mode != 6) {
            em27_act_set(em, 6, 0, 1);
        }
    } else if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em27_main_sub(em, w);
    if (em->x6FF != 0) {
        em27_main_sub(em, w);
        em->x6FF = 0;
    }
}

void em27_main_sub(EMW *em, EM27W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->mode) {
    case 0: em_move00_0060DF90(em, w); break;
    case 1: em_move01_0060E830(em, w); break;
    case 2: em_move02_0060EB30(em, w); break;
    case 3: em_move03_006105E0(em, w); break;
    case 4: em_move04_00610F10(em, w); break;
    case 5: em_move05_00611D40(em, w); break;
    case 6: em_move06_00612130(em, w); break;
    case 7: em_move07_006123A0(em, w); break;
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

void em27_uvmove(EMW *em) {
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

static void sound_call_00612DA0(EMW *em, int frame, int se, int joint) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, 0)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, 5, 0);
    }
}

static void move_default_00612E40(EMW *em) {
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
void ef_move_sub_00612E90(EMW *em, EM27W *w) {
    f32 v[3];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_00612DA0(em, 200, Code_Make(14, 2, 14, 2), 0);
        break;
    case 0x3EA:
        sound_call_00612DA0(em, 8, 2, 0);
        sound_call_00612DA0(em, 32, 2, 0);
        break;
    case 0x3EB:
        sound_call_00612DA0(em, 20, 8, 0);
        sound_call_00612DA0(em, 16, 2, 0);
        sound_call_00612DA0(em, 20, 3, 0);
        if (em_frame_check(em, 18.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3EC:
        sound_call_00612DA0(em, 22, 8, 0);
        sound_call_00612DA0(em, 22, 2, 0);
        sound_call_00612DA0(em, 22, 3, 0);
        if (em_frame_check(em, 20.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3ED:
        sound_call_00612DA0(em, 4, 2, 0);
        sound_call_00612DA0(em, 10, 1, 0);
        break;
    case 0x3EE:
    case 0x3EF:
        sound_call_00612DA0(em, 6, 5, 0);
        sound_call_00612DA0(em, 26, 2, 0);
        sound_call_00612DA0(em, 26, 4, 0);
        sound_call_00612DA0(em, 42, 2, 0);
        sound_call_00612DA0(em, 48, 1, 0);
        break;
    case 0x3F0:
        sound_call_00612DA0(em, 8, 2, 0);
        sound_call_00612DA0(em, 8, 4, 0);
        sound_call_00612DA0(em, 26, 2, 0);
        sound_call_00612DA0(em, 32, 1, 0);
        sound_call_00612DA0(em, 64, 7, 0);
        if (em_frame_check(em, 12.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3F1:
    case 0x3F2:
        sound_call_00612DA0(em, 12, 2, 0);
        sound_call_00612DA0(em, 12, 4, 0);
        sound_call_00612DA0(em, 30, 2, 0);
        sound_call_00612DA0(em, 36, 1, 0);
        sound_call_00612DA0(em, 64, 7, 0);
        if (em_frame_check(em, 14.0f, 0)) {
            Eft13_set_em(em, 1, 0);
        }
        break;
    case 0x3F3:
        sound_call_00612DA0(em, 68, 17, 0);
        sound_call_00612DA0(em, 164, 18, 0);
        sound_call_00612DA0(em, 34, 23, 0);
        sound_call_00612DA0(em, 260, 23, 0);
        sound_call_00612DA0(em, 288, 23, 0);
        break;
    case 0x3F4:
        sound_call_00612DA0(em, 118, 14, 0);
        break;
    case 0x3FC:
        sound_call_00612DA0(em, 22, 2, 0);
        sound_call_00612DA0(em, 22, 3, 0);
        sound_call_00612DA0(em, 22, 8, 0);
        break;
    case 0x3FD:
        sound_call_00612DA0(em, 4, 2, 0);
        sound_call_00612DA0(em, 6, 1, 0);
        sound_call_00612DA0(em, 36, 0, 0);
        break;
    case 0x3FE:
        sound_call_00612DA0(em, 40, 2, 0);
        sound_call_00612DA0(em, 60, 2, 0);
        sound_call_00612DA0(em, 118, 2, 0);
        sound_call_00612DA0(em, 26, 16, 0);
        break;
    case 0x3FF:
        sound_call_00612DA0(em, 22, 2, 0);
        sound_call_00612DA0(em, 22, 3, 0);
        sound_call_00612DA0(em, 22, 8, 0);
        if (em_frame_check(em, 32.0f, 0)) {
            shell15_set(em, 2);
        }
        if (em_frame_check(em, 22.0f, 0)) {
            Eft13_set_em(em, 0x1B, 0);
        }
        break;
    case 0x400:
        sound_call_00612DA0(em, 4, 2, 0);
        sound_call_00612DA0(em, 6, 1, 0);
        break;
    case 0x401:
        sound_call_00612DA0(em, 2, 16, 0);
        break;
    case 0x402:
        sound_call_00612DA0(em, 60, 0, 0);
        sound_call_00612DA0(em, 128, 2, 0);
        sound_call_00612DA0(em, 72, 28, 0);
        sound_call_00612DA0(em, 60, 8, 0);
        break;
    case 0x406:
        sound_call_00612DA0(em, 4, 9, 0);
        sound_call_00612DA0(em, 30, 2, 0);
        sound_call_00612DA0(em, 28, 1, 0);
        sound_call_00612DA0(em, 54, 8, 0);
        break;
    case 0x407:
    case 0x409:
    case 0x40B:
    case 0x40D:
        sound_call_00612DA0(em, 4, 11, 0);
        break;
    case 0x408:
    case 0x40A:
    case 0x40C:
    case 0x40E:
        sound_call_00612DA0(em, 4, 15, 0);
        break;
    case 0x40F:
        sound_call_00612DA0(em, 24, 3, 0);
        sound_call_00612DA0(em, 44, 1, 0);
        sound_call_00612DA0(em, 54, 2, 0);
        sound_call_00612DA0(em, 88, 0, 0);
        break;
    case 0x412:
        sound_call_00612DA0(em, 180, 9, 0);
        sound_call_00612DA0(em, 132, 11, 0);
        sound_call_00612DA0(em, 52, 10, 0);
        sound_call_00612DA0(em, 68, 1, 0);
        sound_call_00612DA0(em, 86, 0, 0);
        sound_call_00612DA0(em, 132, 1, 0);
        sound_call_00612DA0(em, 140, 0, 0);
        break;
    case 0x413:
        sound_call_00612DA0(em, 4, 17, 0);
        sound_call_00612DA0(em, 66, 17, 0);
        sound_call_00612DA0(em, 130, 18, 0);
        sound_call_00612DA0(em, 48, 23, 0);
        sound_call_00612DA0(em, 122, 0, 0);
        sound_call_00612DA0(em, 162, 15, 0);
        break;
    case 0x414:
        sound_call_00612DA0(em, 4, 14, 0);
        v[1] = 10.0f;
        v[2] = 40.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 15, v, 1.0f);
        break;
    case 0x416:
        sound_call_00612DA0(em, 10, 13, 0);
        break;
    case 0x417:
        sound_call_00612DA0(em, 14, 25, 0);
        break;
    case 0x41A:
        sound_call_00612DA0(em, 32, 21, 0);
        sound_call_00612DA0(em, 68, 21, 0);
        sound_call_00612DA0(em, 98, 22, 0);
        sound_call_00612DA0(em, 20, 0, 0);
        sound_call_00612DA0(em, 160, 23, 0);
        break;
    case 0x41B:
        sound_call_00612DA0(em, 10, 20, 0);
        sound_call_00612DA0(em, 64, 20, 0);
        sound_call_00612DA0(em, 6, 1, 0);
        sound_call_00612DA0(em, 122, 1, 0);
        sound_call_00612DA0(em, 134, 0, 0);
        break;
    case 0x41C:
        sound_call_00612DA0(em, 8, 21, 0);
        sound_call_00612DA0(em, 98, 22, 0);
        sound_call_00612DA0(em, 22, 1, 0);
        sound_call_00612DA0(em, 58, 0, 0);
        sound_call_00612DA0(em, 106, 1, 0);
        sound_call_00612DA0(em, 158, 0, 0);
        break;
    case 0x41D:
        sound_call_00612DA0(em, 84, 24, 0);
        sound_call_00612DA0(em, 126, 24, 0);
        sound_call_00612DA0(em, 216, 26, 0);
        sound_call_00612DA0(em, 258, 26, 0);
        sound_call_00612DA0(em, 292, 27, 0);
        sound_call_00612DA0(em, 156, 11, 0);
        sound_call_00612DA0(em, 14, 0, 0);
        sound_call_00612DA0(em, 46, 2, 0);
        break;
    default:
        move_default_00612E40(em);
        break;
    }
}

void em27_effect_move(EMW *em) {
    EM27W *w = (EM27W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_00612E90(em, w);
        break;
    }
    em27_uvmove(em);
}

void dummy_em_prog_006139C0(void) {
}
