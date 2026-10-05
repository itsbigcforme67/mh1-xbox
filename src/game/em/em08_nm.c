/* em08 - game.bin 0x005A7380-0x005A7F68. Action setters for monster 8
 * (a flying monster: it has fly_adjy2 height correction and senkai
 * turning points). Same layout as em07 (see docs/agents/agent-C.md), plus:
 * - move: action 0 becomes 3 when x302 < 10% of x792 (low health? guess);
 *   actions 0, 1, 3 measure the distance to the target.
 * - fly: actions 10/11 pick the next of four stage points to circle
 *   (em08_senkai_pos_no) and a turn time from the distance.
 * - fly_adjy2: per-frame height/depth correction read from piecewise
 *   tables (fly_adjy2_hosei_tbl / fly_adjz2_hosei_tbl). */
#include "em.h"

/* em08's part of the per-monster work at EMW+0x444. */
typedef struct EM08W {
    u8 _pad00[7];
    u8 x07;             /* 0x07 4 when circling (fly 10/11) */
    u8 _pad08[5];
    u8 has_tgt;         /* 0x0D */
    u8 _pad0E[5];
    u8 x13;             /* 0x13 set by atk action 1 */
    u8 _pad14[8];
    s32 spd[3];         /* 0x1C passed to speed_add_g (angle in [1]) */
    u8 _pad28[0x34 - 0x28];
    f32 dist;           /* 0x34 distance to the target (1000 with none) */
    u8 _pad38[4];
    u8 adj_x;           /* 0x3C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x3D */
    u8 adj_z;           /* 0x3E */
    u8 adj_type;        /* 0x3F table row */
    s16 adj_tm;         /* 0x40 time into the table */
} EM08W;

extern f32 (*fly_adjy2_hosei_tbl_00659560[])[2];
extern f32 (*fly_adjz2_hosei_tbl_006596C8[])[2];

f32 CalcDistanceXZ(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_cdm_act_flag_ck(EMW *);
void target_kind_set(EMW *, f32 *);
void cmd_target_kind_set(EMW *, f32 *);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);
u8 em08_senkai_pos_no(EMW *em, f32 *out);

void em08_act_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    em->x388 = 0;
    em_act_set2(em, 0, no, arg);
}

void em08_move_act_set(EMW *em, u16 no, u16 arg) {
    EM08W *w = (EM08W *)em->ex;

    em->x388 = 0;
    em->act_spd = 1.0f;
    switch (no) {
    case 2:
        break;
    case 0:
        if (em->x302 < (s16)(0.1f * em->x792)) {
            no = 3;
        }
    case 1:
    case 3:
        if (em->x881 == 0) {
            w->has_tgt = 0;
            w->dist = 1000.0f;
        } else {
            w->has_tgt = 1;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 7) {
                w->dist -= 550.0f;
                if (w->dist <= 0.0f) {
                    w->dist = 0.0f;
                }
            }
        }
        break;
    default:
        return;
    }
    em_act_set2(em, 1, no, arg);
}

void em08_fly_act_set(EMW *em, u16 no, u16 arg) {
    EM08W *w = (EM08W *)em->ex;
    f32 pt[3];
    u16 a;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 7:
    case 8:
    case 9:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
        break;
    case 4:
    case 5:
        if (em->x8C3 != 0) {
            if (em->x881 == 0) {
                w->has_tgt = 0;
                w->dist = 1000.0f;
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
                w->dist += 1000.0f;
            }
            if (em->x881 == 7) {
                w->dist -= 500.0f;
                if (w->dist <= 0.0f) {
                    w->dist = 0.0f;
                }
            }
        } else {
            if (em->x827 == 0) {
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
        }
        break;
    case 6:
        no = 8;
        break;
    case 10:
    case 11:
        w->x07 = 4;
        if (em->x8C3 == 0) {
            em->x829 = em08_senkai_pos_no(em, pt);
            em->x827 = 2;
            em->x828 = 2;
            cmd_target_kind_set(em, em->tgt_pos);
            a = Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1];
            if ((0 <= a && a <= 0x2000) || (a >= 0xE000 && a <= 0xFFFF)) {
                a = Em_Calc_angY(em->pos, pt) - em->ang[1];
                if (a >= 0x4000 && a <= 0xC000 && no == 10) {
                    no = 11;
                }
                em->work08 = 1.5f * (CalcDistanceXZ(em->pos, em->tgt_pos) / 0.5f) / 50.0f;
            } else {
                no = 9;
            }
        } else {
            target_kind_set(em, em->tgt_pos);
        }
        break;
    default:
        return;
    }
    em_act_set2(em, 2, no, arg);
}

void em08_atk_act_set(EMW *em, u16 no, u16 arg) {
    EM08W *w = (EM08W *)em->ex;

    em->act_spd = 1.0f;
    switch (no) {
    case 1:
        w->x13 = 1;
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        break;
    default:
        return;
    }
    em_act_set2(em, 3, no, arg);
}

void em08_act_set(EMW *em, int kind, u16 no, u16 arg) {
    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    switch ((u16)kind) {
    case 0:
        em08_act_act_set(em, no, arg);
        break;
    case 1:
        em08_move_act_set(em, no, arg);
        break;
    case 2:
        em08_fly_act_set(em, no, arg);
        break;
    case 3:
        em08_atk_act_set(em, no, arg);
        break;
    case 4:
        if (no == 15) {
            arg = 2;
        }
    case 5:
    case 6:
        em->act_spd = 1.0f;
        em_act_set2(em, kind, no, arg);
        break;
    }
}

void em08_fly_adjy2_init(EMW *em, u8 type) {
    EM08W *w = (EM08W *)em->ex;

    w->adj_x = 0;
    w->adj_y = 0;
    w->adj_z = 0;
    w->adj_type = type;
    switch (w->adj_type) {
    case 0:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0;
        break;
    case 1:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0;
        break;
    case 2:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0;
        break;
    case 3:
        w->adj_y = 1;
        w->adj_tm = 0x26;
        break;
    }
    em_rate_clear(em);
}

static u8 fly_adjy2_subx(EMW *em, EM08W *w) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM08W *w) {
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_00659560[w->adj_type];
    u8 ret = 0;
    f32 t;
    f32 v;
    int i;

    if (tbl == 0) {
        return ret;
    }
    {
        if (em->x1C4 != 0) {
            t = 0.0f;
        } else {
            t = w->adj_tm;
        }
        if (t != 0.0f && t != 1.0f) {
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
    }
    return ret;
}

static u8 fly_adjy2_subz(EMW *em, EM08W *w) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_006596C8[w->adj_type];
    u8 ret = 0;
    f32 t;
    f32 v;
    int i;

    if (tbl == 0) {
        return ret;
    }
    {
        if (em->x1C4 != 0) {
            t = 0.0f;
        } else {
            t = w->adj_tm;
        }
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
    }
    return ret;
}

u8 em08_fly_adjy2(EMW *em) {
    EM08W *w = (EM08W *)em->ex;
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
    if (em->x388 == 2 || em->x388 == 4) {
        w->spd[0] = 0;
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
    }
    return ret;
}

u8 em08_senkai_pos_no(EMW *em, f32 *out) {
    u32 i;
    EM_STG_POS *p;
    f32 (*pos)[3];
    f32 dist[4];
    f32 min;
    u8 n;

    pos = 0;
    p = em->area->stg_pos;
    for (i = 0; i < 88; p++, i++) {
        if (p->stg == -1 || em->area->stg_pos[i].stg == em->stg) {
            pos = em->area->stg_pos[i].pos;
            break;
        }
    }
    if (pos == 0) {
        out[0] = 1000.0f;
        out[1] = 0.0f;
        out[2] = 1000.0f;
        return 0;
    }
    for (i = 0; i < 4; i++) {
        dist[i] = CalcDistanceXZ(em->pos, pos[i]);
    }
    n = 0;
    min = dist[0];
    if (min > dist[1]) {
        min = dist[1];
        n = 1;
    }
    if (min > dist[2]) {
        min = dist[2];
        n = 2;
    }
    if (min > dist[3]) {
        n = 3;
    }
    i = (n + 1) & 3;
    out[0] = pos[i][0];
    out[1] = pos[i][1];
    out[2] = pos[i][2];
    return n;
}
