/* em16 - game.bin 0x005E65D0-0x005E6C74. Action setters for monster 16
 * and its fly_adjy2 height/depth correction. Unlike em07/em08 the setters
 * go through em_act_set(em, group, no) and there is no em_cdm_act_flag_ck
 * call. act action 1 turns into 5 when x888 and a target of kind 1 are set,
 * x39A is even and x8C4[x883] is above x818 (meaning unknown). */
#include "em.h"

/* em16's part of the per-monster work at EMW+0x444. */
typedef struct EM16W {
    u8 _pad00[8];
    s32 spd[3];         /* 0x08 passed to speed_add_g (angle in [1]) */
    u8 adj_x;           /* 0x14 fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x15 */
    u8 adj_z;           /* 0x16 */
    u8 adj_type;        /* 0x17 table row */
    s16 adj_tm;         /* 0x18 time into the table */
    u8 _pad1A;
    u8 has_tgt;         /* 0x1B */
    f32 dist;           /* 0x1C distance to the target (1000 with none) */
} EM16W;

extern f32 (*fly_adjy2_hosei_tbl_006669A0[])[2];
extern f32 (*fly_adjz2_hosei_tbl_00666A30[])[2];

f32 CalcDistanceXZ(f32 *, f32 *);
void em_act_set(EMW *, int, u16);
void em_act_set2(EMW *, int, u16, u16);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);

void em16_act_act_set(EMW *em, u16 no, u16 arg) {
    switch (no) {
    case 1:
        if (em->x888 == 1 && em->x881 == 1 && em->x882 == 0 && em->x883 != -1 &&
            em->x39A % 2 == 0 && em->x8C4[em->x883] > em->x818) {
            no = 5;
        }
        break;
    }
    em->x388 = 0;
    em_act_set(em, 0, no);
}

void em16_move_act_set(EMW *em, u16 no, u16 arg) {
    EM16W *w = (EM16W *)em->ex;

    em->x388 = 0;
    switch (no) {
    case 0:
        if (em->x888 == 1) {
            no = 5;
        }
        break;
    case 1:
        if (em->x881 == 0) {
            w->has_tgt = 0;
            w->dist = 1000.0f;
        } else {
            w->has_tgt = 1;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        }
        if (em->x881 == 1 && em->x882 == 0) {
            if (em->x617 == -1) {
                w->has_tgt = 0;
                w->dist = 1000.0f;
            } else {
                w->dist -= 300.0f;
                if (w->dist < 0.0f) {
                    w->dist = 0.0f;
                }
            }
        }
        break;
    }
    em_act_set(em, 1, no);
}

void em16_fly_act_set(EMW *em, u16 no, u16 arg) {
    em_act_set(em, 2, no);
}

void em16_atk_act_set(EMW *em, u16 no, u16 arg) {
    EM16W *w = (EM16W *)em->ex;

    switch (no) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        w->has_tgt = 0;
        if (em->x881 == 1 && em->x617 != -1) {
            w->has_tgt = 1;
        }
        break;
    }
    em_act_set(em, 3, no);
}

void em16_act_set(EMW *em, int kind, u16 no, u16 arg) {
    switch ((u16)kind) {
    case 0:
        em16_act_act_set(em, no, arg);
        break;
    case 1:
        em16_move_act_set(em, no, arg);
        break;
    case 2:
        em16_fly_act_set(em, no, arg);
        break;
    case 3:
        em16_atk_act_set(em, no, arg);
        break;
    case 4:
    case 5:
    case 6:
        em_act_set(em, kind, no);
        break;
    case 7:
        em_act_set2(em, kind, no, arg);
        break;
    }
}

void em16_fly_adjy2_init(EMW *em, u8 type) {
    EM16W *w = (EM16W *)em->ex;

    w->adj_x = 0;
    w->adj_y = 0;
    w->adj_z = 0;
    w->adj_type = type;
    switch (w->adj_type) {
    case 0:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0x16;
        break;
    case 1:
        w->adj_y = 1;
        w->adj_tm = 0x16;
        break;
    case 2:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0x16;
        break;
    case 3:
        w->adj_y = 1;
        w->adj_tm = 0x16;
        break;
    }
    em_rate_clear(em);
}

static u8 fly_adjy2_subx(EMW *em, EM16W *w) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM16W *w) {
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_006669A0[w->adj_type];
    u8 ret = 0;
    f32 t;
    f32 v;
    int i;

    if (tbl == 0) {
        return ret;
    }
    t = w->adj_tm;
    if (t == 0.0f || t == 1.0f) {
        em->adj_y = tbl[0][1];
    } else {
        i = 1;
        do {
            v = tbl[i][0];
            if (t > v && v != 0.0f) {
                i++;
            } else if (v == 0.0f) {
                ret = 2;
                if (em->adj_y > -50.0f) {
                    em->adj_y -= 1.0f;
                }
                break;
            } else {
                em->adj_y = (tbl[i][1] - tbl[i - 1][1]) / ((v - tbl[i - 1][0]) / em->chr_spd0);
                i = 0;
            }
        } while (i != 0);
    }
    return ret;
}

static u8 fly_adjy2_subz(EMW *em, EM16W *w) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_00666A30[w->adj_type];
    u8 ret = 0;
    f32 t;
    f32 v;
    int i;

    if (tbl == 0) {
        return ret;
    }
    t = w->adj_tm;
    if (t == 0.0f || t == 1.0f) {
        em->adj_z = tbl[0][1];
    } else {
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
    if (em->x74C & 0xF000000F) {
        em->adj_z = 0.0f;
    }
    return ret;
}

u8 em16_fly_adjy2(EMW *em) {
    EM16W *w = (EM16W *)em->ex;
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
