/* em17b - game.bin 0x005D9570-0x005D9BC4: second matching run of the em17
 * file (see em17.c). Action setters for monster 17,
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

u16 em17_senkai_target(EMW *em) {
    EM17W *w = (EM17W *)em->ex;
    int a;
    u16 ret = 0;

    if (em->x881 == 0) {
        return 1;
    }
    w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
    w->dang = w->dang - em->ang[1];
    if (w->dang != 0) {
        a = w->dang & 0xFFFF;
        if (a <= 0x8000) {
            if (a <= w->turn) {
                ret = 1;
                em->ang[1] += a;
            } else {
                em->ang[1] += w->turn;
            }
        } else {
            if (a >= 0x10000 - w->turn) {
                ret = 1;
                em->ang[1] -= 0x10000 - a;
            } else {
                em->ang[1] -= w->turn;
            }
        }
    }
    em->ang[1] = (u16)em->ang[1];
    return ret;
}

void em17_fly_adjy(EMW *em, int type) {
    f32 (*tbl)[2] = fly_adjy_hosei_tbl_006646E0[type];
    EM17W *w = (EM17W *)em->ex;
    f32 t;
    f32 v;
    int i;

    if (tbl != 0 && em->x1C4 == 0) {
        t = em->x19C;
        if (t == 0.0f || t == 1.0f) {
            em->adj_y = tbl[0][1];
        } else {
            i = 1;
            do {
                v = tbl[i][0];
                if (t > v && v != 0.0f) {
                    i++;
                } else if (v == 0.0f) {
                    break;
                } else {
                    em->adj_y = (tbl[i][1] - tbl[i - 1][1]) / ((v - tbl[i - 1][0]) / em->chr_spd0);
                    i = 0;
                }
            } while (i != 0);
        }
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
    }
}

void em17_fly_adjy2_init(EMW *em, u8 type) {
    EM17W *w = (EM17W *)em->ex;

    w->adj_x = 0;
    w->adj_y = 0;
    w->adj_z = 0;
    w->adj_type = type;
    switch (w->adj_type) {
    case 2:
        w->adj_y = 1;
        w->adj_tm = 0x60;
        break;
    case 3:
        w->adj_y = 1;
        w->adj_tm = 0x1E;
        break;
    case 5:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0x3C;
        break;
    case 8:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0x4C;
        break;
    case 9:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0;
        break;
    case 10:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0x5C;
        break;
    case 11:
        w->adj_y = 1;
        w->adj_tm = 0;
        break;
    case 12:
        w->adj_y = 1;
        w->adj_tm = 0x2C;
        break;
    case 13:
        w->adj_y = 1;
        w->adj_tm = 0;
        break;
    }
    em_rate_clear(em);
}

static u8 fly_adjy2_subx(EMW *em, EM17W *w) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM17W *w) {
    int i;
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_00664AF0[w->adj_type];
    u8 ret = 0;
    f32 t;
    f32 v;

    if (tbl == 0) {
        return ret;
    }
    t = w->adj_tm;
    if (t != 0.0f && t != 1.0f) {
        i = 1;
        do {
            v = tbl[i][0];
            if (t > v && v > 0.0f) {
                i++;
            } else if (v == 0.0f) {
                ret = 2;
                if (em->adj_y > -50.0f) {
                    em->adj_y -= 1.0f;
                }
                break;
            } else if (v < 0.0f) {
                w->adj_tm = 0;
                ret = 2;
                em->adj_y = 0.0f;
                break;
            } else {
                em->adj_y = (tbl[i][1] - tbl[i - 1][1]) / ((v - tbl[i - 1][0]) / em->chr_spd0);
                i = 0;
            }
        } while (i != 0);
    }
    return ret;
}

static u8 fly_adjy2_subz(EMW *em, EM17W *w) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_00664CB0[w->adj_type];
    u8 ret = 0;
    f32 t;
    f32 v;
    int i;

    if (tbl == 0) {
        return ret;
    }
    t = w->adj_tm;
    if (t != 0.0f && t != 1.0f) {
        i = 1;
        do {
            v = tbl[i][0];
            if (t > v && v != 0.0f) {
                i++;
            } else if (v == 0.0f) {
                ret = 4;
                break;
            } else {
                em->adj_z = (tbl[i][1] - tbl[i - 1][1]) / ((v - tbl[i - 1][0]) / em->chr_spd0);
                i = 0;
            }
        } while (i != 0);
    }
    return ret;
}

u8 em17_fly_adjy2(EMW *em) {
    EM17W *w = (EM17W *)em->ex;
    u8 ret = 0;

    if (w->adj_x != 0) {
        ret = fly_adjy2_subx(em, w);
    }
    if (w->adj_y != 0) {
        ret |= fly_adjy2_suby(em, w);
    }
    if (w->adj_z != 0) {
        ret |= fly_adjy2_subz(em, w);
    }
    w->adj_tm += (s16)em->chr_spd0;
    if (em->x388 == 2) {
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
    }
    return ret;
}
