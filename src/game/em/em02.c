/* em02 - game.bin 0x0057E120-0x0057EF94. Action setters for monster 2,
 * its turning helpers (senkai_player / senkai_target: turn ang[1] toward
 * a player or the target by at most w->turn per call) and two height
 * corrections (fly_adjy from fly_adjy_hosei_tbl, fly_adjy2 as in em08).
 * Most fly and attack actions first fall back to em02_act_set(em, 2, 0, 1)
 * while x8C3 and x388 are clear (meaning of x388 is a guess: some
 * movement state, 2 in the attack check). */
#include "em.h"
#include "pl.h"

/* em02's part of the per-monster work at EMW+0x444. */
typedef struct EM02W {
    u8 _pad00[0x10];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    u8 _pad18[2];
    u8 x1A;             /* 0x1A set by attack actions 5/9-11 */
    u8 _pad1B[0x30 - 0x1B];
    s32 spd[3];         /* 0x30 passed to speed_add(_g) (angle in [1]) */
    u8 _pad3C[4];
    s32 turn;           /* 0x40 maximum turn per call */
    u8 _pad44[8];
    u8 adj_x;           /* 0x4C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x4D */
    u8 adj_z;           /* 0x4E */
    u8 adj_type;        /* 0x4F table row */
    s16 adj_tm;         /* 0x50 time into the table */
} EM02W;

extern f32 (*fly_adjy_hosei_tbl_00388500[2])[2];
extern f32 (*fly_adjy2_hosei_tbl_00654420[])[2];
extern f32 (*fly_adjz2_hosei_tbl_006544A0[])[2];

f32 CalcDistanceXZ(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_cdm_act_flag_ck(EMW *);
void em_rate_clear(EMW *);
void em_pl_pos_set(EMW *, u8, f32 *);
void World_calc2(u8, f32 *, f32 *);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
void em02_act_set(EMW *em, int kind, u16 no, u16 arg);

#define EM02_ACT_CK(em) (em->x8C3 == 0 && em->x388 == 0)

void em02_act_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    switch (no) {
    case 1:
    case 2:
        if ((em->x39A & 1) && arg) {
            no = 1;
        } else {
            no = 2;
        }
        break;
    }
    em_act_set2(em, 0, no, arg);
}

void em02_move_act_set(EMW *em, u16 no, u16 arg) {
    EM02W *w = (EM02W *)em->ex;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
        if (em->x881 == 0) {
            w->has_tgt = 0;
            w->dist = 1000.0f;
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
            w->dist -= 500.0f;
            if (w->dist <= 0.0f) {
                w->dist = 0.0f;
            }
        }
        if (em->x881 == 7) {
            w->dist -= 500.0f;
            if (w->dist <= 0.0f) {
                w->dist = 0.0f;
            }
        }
        break;
    case 1:
        break;
    default:
        return;
    }
    em_act_set2(em, 1, no, arg);
}

void em02_fly_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 1:
        break;
    case 2:
    case 3:
        if (EM02_ACT_CK(em)) {
            em02_act_set(em, 2, 0, 1);
            return;
        }
        em->work08 = 300;
        break;
    case 4:
    case 9:
    case 10:
        if (EM02_ACT_CK(em)) {
            em02_act_set(em, 2, 0, 1);
            return;
        }
        em_rate_clear(em);
        em->adj_z = 10.0f;
        em->work08 = CalcDistanceXZ(em->pos, em->tgt_pos) / em->adj_z;
        if (em->work08 < 30) {
            em->work08 = 30;
        }
        break;
    case 5:
        if (EM02_ACT_CK(em)) {
            em02_act_set(em, 2, 0, 1);
            return;
        }
        em_rate_clear(em);
        em->adj_z = 10.0f;
        em->work08 = (int)(flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z) * 3;
        break;
    case 6:
        if (EM02_ACT_CK(em)) {
            em02_act_set(em, 2, 0, 1);
            return;
        }
        em_rate_clear(em);
        em->adj_z = 10.0f;
        em->work08 = (int)(flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z) * 3;
        break;
    case 7:
        if (EM02_ACT_CK(em)) {
            em02_act_set(em, 2, 0, 1);
            return;
        }
        em_rate_clear(em);
        em->adj_z = 10.0f;
        em->work08 = 300;
        break;
    case 8:
        if (EM02_ACT_CK(em)) {
            em02_act_set(em, 2, 0, 1);
            return;
        }
        em_rate_clear(em);
        em->adj_z = 10.0f;
        em->work08 = 300;
        break;
    default:
        return;
    }
    em_act_set2(em, 2, no, arg);
}

void em02_atk_act_set(EMW *em, u16 no, u16 arg) {
    EM02W *w = (EM02W *)em->ex;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
    case 8:
        if (em->x8C3 == 0 && em->x388 == 2) {
            em02_act_set(em, 2, 1, 1);
            return;
        }
        break;
    case 6:
        if (em->x8C3 == 0 && em->x388 == 2) {
            em02_act_set(em, 2, 1, 1);
            return;
        }
        if (em->x881 == 0) {
            w->has_tgt = 0;
            em->work08 = (em->x39A % 120 + 180) / 2;
        } else {
            w->has_tgt = 1;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            em->work08 = (int)w->dist / 7;
        }
        break;
    case 5:
    case 10:
    case 11:
        if (EM02_ACT_CK(em)) {
            em02_act_set(em, 2, 0, 1);
            return;
        }
        w->x1A = 1;
        break;
    case 9:
        if (EM02_ACT_CK(em)) {
            em02_act_set(em, 2, 0, 1);
            return;
        }
        w->x1A = 3;
        break;
    }
    em_act_set2(em, 3, no, arg);
}

void em02_act_set(EMW *em, int kind, u16 no, u16 arg) {
    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    switch ((u16)kind) {
    case 0:
        em02_act_act_set(em, no, arg);
        break;
    case 1:
        em02_move_act_set(em, no, arg);
        break;
    case 2:
        em02_fly_act_set(em, no, arg);
        break;
    case 3:
        em02_atk_act_set(em, no, arg);
        break;
    case 4:
    case 5:
    case 6:
        em->act_spd = 1.0f;
        em_act_set2(em, kind, no, arg);
        break;
    }
}

void em02_senkai_player(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
    f32 p[3];
    f32 q[3];
    s8 n = em->x617;
    int a;

    if (n != -1) {
        em_pl_pos_set(em, n, p);
        World_calc2(((PLW *)player_work)[n].stg, p, q);
        w->dang = Em_Calc_angY(em->x754, q);
        w->dang = w->dang - em->ang[1];
        if (w->dang != 0) {
            a = w->dang & 0xFFFF;
            if (a <= 0x8000) {
                if (a <= w->turn) {
                    em->ang[1] += a;
                } else {
                    em->ang[1] += w->turn;
                }
            } else {
                if (a >= 0x10000 - w->turn) {
                    em->ang[1] -= 0x10000 - a;
                } else {
                    em->ang[1] -= w->turn;
                }
            }
        }
        em->ang[1] = (u16)em->ang[1];
    }
}

u16 em02_senkai_target(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
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

void em02_fly_adjy(EMW *em, int type) {
    f32 (*tbl)[2] = fly_adjy_hosei_tbl_00388500[type];
    EM02W *w = (EM02W *)em->ex;
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

void em02_fly_adjy2_init(EMW *em, u8 type) {
    EM02W *w = (EM02W *)em->ex;

    w->adj_x = 0;
    w->adj_y = 0;
    w->adj_z = 0;
    w->adj_type = type;
    switch (w->adj_type) {
    case 0:
        w->adj_y = 1;
        w->adj_tm = 0x86;
        break;
    case 2:
        w->adj_y = 1;
        w->adj_tm = 0;
        break;
    case 3:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0x46;
        break;
    }
    em_rate_clear(em);
}

static u8 fly_adjy2_subx(EMW *em, EM02W *w) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM02W *w) {
    int i;
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_00654420[w->adj_type];
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

static u8 fly_adjy2_subz(EMW *em, EM02W *w) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_006544A0[w->adj_type];
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

u8 em02_fly_adjy2(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
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
