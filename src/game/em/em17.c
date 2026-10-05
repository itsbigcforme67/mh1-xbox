/* em17 - game.bin 0x005D81A0-0x005D8C64 (setters; em17_senkai_sub is still
 * asm, near-match in em17_nm.c; the rest of the file is em17b.c). Action setters for monster 17,
 * its flying turn (senkai_sub: bank ang[2] toward the turn, turn ang[1]
 * by a rate taken from the bank, sink while banking and climb back toward
 * 1000-2100 above the stage floor, switching flight animations), plus
 * senkai_target / fly_adjy / fly_adjy2 as in em02/em14. Meanings of the
 * constants are guesses. */
#include "em.h"

/* em17's part of the per-monster work at EMW+0x444. */
typedef struct EM17W {
    u8 _pad00[0x10];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    u8 _pad18[2];
    u8 x1A;             /* 0x1A attack variant (atk 4) */
    u8 _pad1B;
    s32 turn_left;      /* 0x1C */
    s32 bank_max;       /* 0x20 bank limit for senkai_sub */
    u8 _pad24[0x30 - 0x24];
    s32 spd[3];         /* 0x30 passed to speed_add(_g) (angle in [1]) */
    s32 pitch_spd;      /* 0x3C ang[0] returns to 0 by this much */
    s32 turn;           /* 0x40 maximum turn per call */
    s32 bank_spd;       /* 0x44 ang[2] step */
    u8 _pad48[4];
    u8 adj_x;           /* 0x4C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x4D */
    u8 adj_z;           /* 0x4E */
    u8 adj_type;        /* 0x4F table row */
    s16 adj_tm;         /* 0x50 time into the table */
} EM17W;

typedef struct STAGE_DATA {
    u8 _pad00[0x18];
    f32 floor_y;        /* 0x18 */
} STAGE_DATA;

extern f32 (*fly_adjy_hosei_tbl_006646E0[])[2];
extern f32 (*fly_adjy2_hosei_tbl_00664AF0[])[2];
extern f32 (*fly_adjz2_hosei_tbl_00664CB0[])[2];
extern u16 st58_dir[4];
extern u16 st64_dir[4];
extern u16 st75_dir[4];

f32 CalcDistanceXZ(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void flvecCopy(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_cdm_act_flag_ck(EMW *);
void em_rate_clear(EMW *);
void target_kind_set(EMW *, f32 *);
void cmd_target_kind_set(EMW *, f32 *);
void em17_horm_init(EMW *);
int Online_ck(void);
s16 act_ck(EMW *, u16, u16);
STAGE_DATA *Stage_data_get(u8);
void em_char_set(EMW *, int, int, int);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);

#define DIST_CLAMP(w, d)            \
    w->dist -= d;                   \
    if (w->dist <= 0.0f) {          \
        w->dist = 0.0f;             \
    }

void em17_act_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    switch (no) {
    case 1:
        if (em->x8B6 != 0) {
            no = 2;
        }
        break;
    case 14:
        em17_horm_init(em);
        break;
    case 22:
        em->act_spd = 0.0f;
        break;
    case 26:
        flvecCopy(em->pos, em->tgt_pos);
        switch (em->stg) {
        case 0x3A:
            em->ang[1] = st58_dir[em->x829];
            break;
        case 0x40:
            em->ang[1] = st64_dir[em->x829];
            break;
        case 0x4B:
            em->ang[1] = st75_dir[em->x829];
            break;
        default:
            em->ang[1] = 0;
            break;
        }
        em->act_spd = 0.0f;
        break;
    }
    em->x388 = 0;
    em_act_set2(em, 0, no, arg);
}

void em17_move_act_set(EMW *em, u16 no, u16 arg) {
    EM17W *w = (EM17W *)em->ex;

    em->x388 = 0;
    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 5:
        if (em->x302 < (s16)(0.3f * em->x792)) {
            no = 6;
            goto act6;
        }
        if (em->x8C3 != 0) {
            if (em->x881 == 0) {
                w->has_tgt = 0;
                break;
            }
            w->has_tgt = 1;
            target_kind_set(em, em->tgt_pos);
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                if (em->x617 == -1) {
                    w->has_tgt = 0;
                    w->dist = 1000.0f;
                    break;
                }
                DIST_CLAMP(w, 500.0f);
            }
            if (em->x881 == 7) {
                DIST_CLAMP(w, 500.0f);
            }
        } else {
            if (em->x827 == 0) {
                w->has_tgt = 0;
                em->work08 = (em->x39A % 120 + 180) / 2;
                break;
            }
            w->has_tgt = 1;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                if (em->x617 == -1) {
                    w->has_tgt = 0;
                    w->dist = 1000.0f;
                    break;
                }
                DIST_CLAMP(w, 500.0f);
            }
            if (em->x881 == 7) {
                DIST_CLAMP(w, 500.0f);
            }
            em->work08 = (int)w->dist / 7;
        }
        break;
    case 1:
        em->x839 = 1;
        return;
    case 8:
        em->act_spd = 0.7f;
    case 4:
        if (em->x881 == 0) {
            w->has_tgt = 0;
            em->work08 = (em->x39A % 120 + 180) / 2;
            break;
        }
        w->has_tgt = 1;
        w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (em->x881 == 1 && em->x882 == 0 && em->x617 == -1) {
            w->has_tgt = 0;
            w->dist = 300.0f;
            break;
        }
        em->work08 = (int)w->dist / 7;
        break;
    case 6:
    act6:
        if (em->x881 == 0) {
            w->has_tgt = 0;
            em->work08 = (em->x39A % 120 + 180) / 2;
        } else {
            w->has_tgt = 1;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            em->work08 = (int)w->dist / 7;
        }
        if (em->work08 > 300) {
            em->work08 = 300;
        }
        break;
    case 2:
    case 3:
    case 7:
        break;
    default:
        return;
    }
    em_act_set2(em, 1, no, arg);
}

void em17_fly_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 4:
    case 5:
    case 14:
    case 23:
    case 1:
    case 2:
    case 3:
        break;
    case 7:
    case 21:
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->work08 = (int)(flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z) * 3;
        break;
    case 9:
        if (em->x8C3 == 0) {
            em->x827 = 2;
            if (em->x84D != 0) {
                em->x828 = 3;
                em->x829 = 0;
            } else {
                em->x828 = 0;
            }
            cmd_target_kind_set(em, em->tgt_pos);
        }
        em->work08 = 500;
        break;
    case 16:
        break;
    case 20:
        if (em->adj_z <= 0.0f) {
            em->adj_z = 20.0f;
        }
        if (em->x8C3 == 0) {
            em->work08 = flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z;
        }
        break;
    default:
        return;
    }
    em_act_set2(em, 2, no, arg);
}

void em17_atk_act_set(EMW *em, u16 no, u16 arg) {
    EM17W *w = (EM17W *)em->ex;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 5:
    case 8:
        em17_horm_init(em);
        break;
    case 4:
        if (em->x302 >= 1250) {
            w->x1A = em->x39A % 2 + 1;
        } else {
            w->x1A = em->x39A % 2 + 2;
        }
        em17_horm_init(em);
        break;
    case 1:
    case 2:
    case 3:
    case 6:
    case 7:
    case 9:
        break;
    }
    em_act_set2(em, 3, no, arg);
}

void em17_act_set(EMW *em, int kind, u16 no, u16 arg) {
    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    if (Online_ck() == 0) {
        if (act_ck(em, 0, 0x16) != 0 || act_ck(em, 0, 0x1A) != 0) {
            kind = 0;
            no = 12;
        }
    } else if ((u8)arg != 0) {
        if (act_ck(em, 0, 0x16) != 0 || act_ck(em, 0, 0x1A) != 0) {
            kind = 0;
            no = 12;
        }
    }
    switch ((u16)kind) {
    case 0:
        em17_act_act_set(em, no, arg);
        break;
    case 1:
        em17_move_act_set(em, no, arg);
        break;
    case 2:
        em17_fly_act_set(em, no, arg);
        break;
    case 3:
        em17_atk_act_set(em, no, arg);
        break;
    case 4:
    case 5:
    case 6:
        em->act_spd = 1.0f;
        em_act_set2(em, kind, no, arg);
        break;
    }
}
