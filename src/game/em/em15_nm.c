/* em15 - game.bin 0x005CF0F0-0x005D04EC. Action setters for monster 15,
 * fly_adjy / fly_adjy2 height corrections (as em02/em14), senkai_sub (turn
 * ang[1] by at most w->turn, keeping the rest in w->turn_left) and
 * senkai_pos_no (same code as em08's). Fly action 33 picks a ceiling point
 * near the target player (GetTenjoHit) to fly to; meanings are guesses. */
#include "em.h"
#include "pl.h"

/* em15's part of the per-monster work at EMW+0x444. */
typedef struct EM15W {
    u8 _pad00[4];
    u8 adj_x;           /* 0x04 fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x05 */
    u8 adj_z;           /* 0x06 */
    u8 adj_type;        /* 0x07 table row */
    s16 adj_tm;         /* 0x08 time into the table */
    u8 x0A;             /* 0x0A 4 when circling (fly 29/30) */
    u8 _pad0B[5];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    u8 _pad18[4];
    s32 turn_left;      /* 0x1C */
    s32 spd[3];         /* 0x20 passed to speed_add(_g) (angle in [1]) */
    u8 _pad2C[4];
    s32 turn;           /* 0x30 maximum turn per call */
    u8 _pad34[0x42 - 0x34];
    u16 x42;            /* 0x42 ceiling area number? (fly 33) */
} EM15W;

extern f32 (*fly_adjy_hosei_tbl_00662C00[])[2];
extern f32 (*fly_adjy2_hosei_tbl_006630C0[])[2];
extern f32 (*fly_adjz2_hosei_tbl_00663300[])[2];

f32 CalcDistanceXZ(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_cdm_act_flag_ck(EMW *);
void em_rate_clear(EMW *);
void target_kind_set(EMW *, f32 *);
void cmd_target_kind_set(EMW *, f32 *);
void pl_flag_clr(EMW *, u32);
void em_pl_pos_set(EMW *, u8, f32 *);
u8 GetTenjoHit(f32 *, f32 *, u16 *);
void SetVector(f32 *, f32, f32, f32);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
void em15_act_set(EMW *em, int kind, u16 no, u16 arg);
u8 em15_senkai_pos_no(EMW *em, f32 *out);

#define DIST_CLAMP(w, d)            \
    w->dist -= d;                   \
    if (w->dist <= 0.0f) {          \
        w->dist = 0.0f;             \
    }

void em15_act_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    switch (no) {
    case 1:
        if (em->x888 == 1) {
            no = 0x11;
        }
        break;
    case 7:
        if (em->x888 == 1) {
            no = 8;
        }
        break;
    }
    em->x388 = 0;
    em_act_set2(em, 0, no, arg);
}

void em15_move_act_set(EMW *em, u16 no, u16 arg) {
    EM15W *w = (EM15W *)em->ex;

    em->x388 = 0;
    em->act_spd = 1.0f;
    switch (no) {
    case 0:
        pl_flag_clr(em, 0x20000);
        if (em->x302 < (s16)(0.2f * em->x792)) {
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
            if (em->x881 == 0) {
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
    case 3:
        if (em->x888 == 1) {
            no = 5;
        }
        break;
    case 5:
    case 7:
        break;
    case 6:
    act6:
        pl_flag_clr(em, 0x20000);
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
    default:
        return;
    }
    em_act_set2(em, 1, no, arg);
}

void em15_fly_act_set(EMW *em, u16 no, u16 arg) {
    EM15W *w = (EM15W *)em->ex;
    f32 pt[3];
    f32 p[3];
    u16 hit[2];
    f32 y;
    u16 *h;
    f32 d;
    PLW *pl;
    u16 a;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 10:
    case 12:
    case 14:
    case 16:
    case 20:
    case 22:
    case 23:
    case 25:
    case 26:
    case 32:
        pl_flag_clr(em, 0x20000);
        break;
    case 7:
        switch (em->stg) {
        case 0x12:
        case 0x16:
        case 0x17:
        case 0x46:
        case 0x49:
        case 0x4E:
            no = 6;
            break;
        }
    case 21:
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->work08 = (int)(flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z) * 3;
        break;
    case 8:
    case 13:
        if (em->x8C3 == 0 && em->x881 != 3) {
            em15_act_set(em, 2, 1, 1);
            return;
        }
        break;
    case 9:
        if (em->x8C3 == 0) {
            em->x827 = 2;
            em->x828 = 3;
            em->x829 = 0;
            cmd_target_kind_set(em, em->tgt_pos);
        }
        em->work08 = 500;
        break;
    case 11:
        if (em->x8C3 == 0) {
            if (em->x84D != 0) {
                em->x827 = 2;
                em->x828 = 3;
                em->x829 = 0;
            }
            cmd_target_kind_set(em, em->tgt_pos);
        }
        em->work08 = 500;
        break;
    case 15:
        em->work08 = 500;
        break;
    case 24:
        if (em->x9F3 == 0 && em->x8C3 == 0) {
            em15_act_set(em, 0, 7, 1);
            return;
        }
        break;
    case 28:
        pl_flag_clr(em, 0x20000);
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
            DIST_CLAMP(w, 500.0f);
        }
        break;
    case 29:
    case 30:
        w->x0A = 4;
        if (em->x8C3 == 0) {
            em->x829 = em15_senkai_pos_no(em, pt);
            em->x827 = 2;
            em->x828 = 2;
            cmd_target_kind_set(em, em->tgt_pos);
            a = Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1];
            if ((0 <= a && a <= 0x2000) || (a >= 0xE000 && a <= 0xFFFF)) {
                a = Em_Calc_angY(em->pos, pt) - em->ang[1];
                if (a >= 0x4000 && a <= 0xC000 && no == 0x1D) {
                    no = 0x1E;
                }
            } else {
                no = 0x1F;
            }
        } else {
            target_kind_set(em, em->tgt_pos);
        }
        break;
    case 33:
        if (em->x8C3 != 0) {
            break;
        }
        pl = &((PLW *)player_work)[em->x617];
        w->has_tgt = 0;
        if (em->x617 == -1) {
            no = 0x22;
            break;
        }
        em->x3B0 = pl;
        em_pl_pos_set(em, em->x617, p);
        if (GetTenjoHit(p, &y, hit) == 0) {
            no = 0x22;
            break;
        }
        d = CalcDistanceXZ(em->pos, p);
        h = &hit[1];
        if (w->x42 == *h || w->x42 == 0 || w->x42 >= 5 || d <= 500.0f) {
            no = 0x22;
            break;
        }
        em->x829 = (*h - 1) & 3;
        switch (w->x42) {
        case 1:
            if (*h == 3) {
                em->x829 = (*h - 2) & 3;
            }
            break;
        case 2:
            if (*h == 4) {
                em->x829 = (*h - 2) & 3;
            }
            break;
        case 3:
            if (*h == 1) {
                em->x829 = (*h - 2) & 3;
            }
            break;
        case 4:
            if (*h == 2) {
                em->x829 = (*h - 2) & 3;
            }
            break;
        }
        em->x827 = 2;
        em->x828 = 2;
        cmd_target_kind_set(em, em->tgt_pos);
        w->has_tgt = 1;
        if ((w->dist = CalcDistanceXZ(em->pos, em->tgt_pos)) <= 500.0f) {
            no = 0x22;
        }
        break;
    case 27:
    case 31:
    case 34:
        break;
    default:
        return;
    }
    em_act_set2(em, 2, no, arg);
}

void em15_atk_act_set(EMW *em, u16 no, u16 arg) {
    EM15W *w = (EM15W *)em->ex;
    f32 p[3];
    f32 d;

    em->act_spd = 1.0f;
    switch (no) {
    case 2:
        if (em->x617 == -1) {
            w->dist = 2500.0f;
            break;
        }
        em->x3B0 = &((PLW *)player_work)[em->x617];
        em_pl_pos_set(em, em->x617, p);
        SetVector(em->tgt_pos, p[0], p[1], p[2]);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        w->dist = d;
        if (d > 5000.0f) {
            w->dist = 5000.0f;
        } else if (d < 2500.0f) {
            w->dist = 2500.0f;
        }
        break;
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        break;
    }
    em_act_set2(em, 3, no, arg);
}

void em15_act_set(EMW *em, int kind, u16 no, u16 arg) {
    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    switch ((u16)kind) {
    case 0:
        em15_act_act_set(em, no, arg);
        break;
    case 1:
        em15_move_act_set(em, no, arg);
        break;
    case 2:
        em15_fly_act_set(em, no, arg);
        break;
    case 3:
        em15_atk_act_set(em, no, arg);
        break;
    case 4:
    case 5:
    case 6:
        em->act_spd = 1.0f;
        em_act_set2(em, kind, no, arg);
        break;
    }
}

void em15_fly_adjy(EMW *em, int type) {
    f32 (*tbl)[2] = fly_adjy_hosei_tbl_00662C00[type];
    EM15W *w = (EM15W *)em->ex;
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

void em15_fly_adjy2_init(EMW *em, u8 type) {
    EM15W *w = (EM15W *)em->ex;

    w->adj_x = 0;
    w->adj_y = 0;
    w->adj_z = 0;
    w->adj_type = type;
    switch (w->adj_type) {
    case 0:
    case 1:
    case 4:
    case 6:
    case 7:
        break;
    case 2:
        w->adj_y = 1;
        w->adj_tm = 0x26;
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
    case 14:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0x4E;
        break;
    case 15:
        w->adj_y = 1;
        w->adj_z = 1;
        w->adj_tm = 0x1E;
        break;
    }
    em_rate_clear(em);
}

static u8 fly_adjy2_subx(EMW *em, EM15W *w) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM15W *w) {
    int i;
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_006630C0[w->adj_type];
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

static u8 fly_adjy2_subz(EMW *em, EM15W *w) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_00663300[w->adj_type];
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

u8 em15_fly_adjy2(EMW *em) {
    EM15W *w = (EM15W *)em->ex;
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

void em15_senkai_sub(EMW *em) {
    EM15W *w = (EM15W *)em->ex;

    w->turn_left = w->dang;
    if (w->turn_left <= 0x8000) {
        if (w->turn_left <= w->turn) {
            em->ang[1] += w->turn_left;
            w->turn_left = 0;
        } else {
            em->ang[1] += w->turn;
            w->turn_left -= w->turn;
        }
    } else {
        if (w->turn_left >= 0x10000 - w->turn) {
            em->ang[1] -= 0x10000 - w->turn_left;
            w->turn_left += 0x10000 - w->turn_left;
        } else {
            em->ang[1] -= w->turn;
            w->turn_left += w->turn;
        }
    }
}

u8 em15_senkai_pos_no(EMW *em, f32 *out) {
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

