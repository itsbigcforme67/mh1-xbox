/* em14 - game.bin 0x005C1260-0x005C2634. Action setters for monster 14,
 * its turning (senkai_target, as em02), the charge turn (tossin_move: at
 * most 0x40 per frame toward the target, then subtract the animation's
 * forward motion from the distance), fly_adjy and fly_adjy2 (as em02).
 * Many actions compute a time in work08 from the target distance
 * (dist / 7 or (x39A % 120 + 180) / 2 without a target). */
#include "em.h"

/* em14's part of the per-monster work at EMW+0x444. */
typedef struct EM14W {
    u8 _pad00[0x10];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    u8 _pad18[0x30 - 0x18];
    s32 spd[3];         /* 0x30 passed to speed_add(_g) (angle in [1]) */
    u8 _pad3C[4];
    s32 turn;           /* 0x40 maximum turn per call */
    u8 _pad44[4];
    u8 adj_x;           /* 0x48 fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x49 */
    u8 adj_z;           /* 0x4A */
    u8 adj_type;        /* 0x4B table row */
    s16 adj_tm;         /* 0x4C time into the table */
} EM14W;

extern f32 (*fly_adjy_hosei_tbl_0065EE60[])[2];
extern f32 (*fly_adjy2_hosei_tbl_0065F260[])[2];
extern f32 (*fly_adjz2_hosei_tbl_0065F420[])[2];

f32 CalcDistanceXZ(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_cdm_act_flag_ck(EMW *);
void em_rate_clear(EMW *);
void target_kind_set(EMW *, f32 *);
void em14_horm_init(EMW *);
void mot_miration_ret(EMW *, f32 *);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
void em14_act_set(EMW *em, int kind, u16 no, u16 arg);

#define DIST_CLAMP(w, d)            \
    w->dist -= d;                   \
    if (w->dist <= 0.0f) {          \
        w->dist = 0.0f;             \
    }

void em14_act_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    em->x388 = 0;
    em_act_set2(em, 0, no, arg);
}

void em14_move_act_set(EMW *em, u16 no, u16 arg) {
    EM14W *w = (EM14W *)em->ex;

    em->x388 = 0;
    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 2:
        if (em->x302 < (s16)(0.3f * em->x792)) {
            no = 6;
            goto act6;
        }
        if (no == 2 && em->x888 == 1) {
            no = 4;
            goto act4;
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
    case 3:
        if (em->x888 == 1) {
            no = 5;
        }
        break;
    case 8:
        em->act_spd = 0.7f;
    case 4:
    act4:
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
    case 5:
    case 7:
        break;
    default:
        return;
    }
    em_act_set2(em, 1, no, arg);
}

void em14_fly_act_set(EMW *em, u16 no, u16 arg) {
    EM14W *w = (EM14W *)em->ex;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
        if (em->x7E9 == 0) {
            em14_act_set(em, 0, 3, 1);
            return;
        }
        break;
    case 1:
    case 2:
    case 3:
    case 6:
    case 10:
    case 12:
        break;
    case 4:
    case 8:
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
        em->work08 = (int)w->dist / 7;
        break;
    case 5:
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
    case 7:
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->work08 = (int)(flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z) * 3;
        break;
    case 9:
        if (em->adj_z <= 0.0f) {
            em->adj_z = 20.0f;
        }
        if (em->x8C3 == 0) {
            em->work08 = flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z;
        }
        break;
    case 11:
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->work08 = (int)(flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z) * 3;
        break;
    }
    em_act_set2(em, 2, no, arg);
}

void em14_atk_act_set(EMW *em, u16 no, u16 arg) {
    EM14W *w = (EM14W *)em->ex;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 26:
    case 27:
    case 29:
        em14_horm_init(em);
        break;
    case 3:
    case 6:
        break;
    case 15:
        if (em->x617 == -1) {
            em->work08 = 150;
            w->has_tgt = 0;
        } else {
            w->has_tgt = 1;
            em->work08 = (int)((w->dist = CalcDistanceXZ(em->pos, em->tgt_pos) + 500.0f) / 30.0f) + 30;
        }
        break;
    case 16:
    case 17:
        if (em->x617 == -1) {
            em->work08 = 150;
            w->has_tgt = 0;
        } else {
            w->has_tgt = 1;
            em->work08 = (int)((w->dist = CalcDistanceXZ(em->pos, em->tgt_pos)) / 30.0f) + 30;
        }
        break;
    }
    em_act_set2(em, 3, no, arg);
}

void em14_act_set(EMW *em, int kind, u16 no, u16 arg) {
    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    switch ((u16)kind) {
    case 0:
        em14_act_act_set(em, no, arg);
        break;
    case 1:
        em14_move_act_set(em, no, arg);
        break;
    case 2:
        em14_fly_act_set(em, no, arg);
        break;
    case 3:
        em14_atk_act_set(em, no, arg);
        break;
    case 4:
    case 5:
    case 6:
        em->act_spd = 1.0f;
        em_act_set2(em, kind, no, arg);
        break;
    }
}

u16 em14_senkai_target(EMW *em) {
    EM14W *w = (EM14W *)em->ex;
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

void em14_tossin_move(EMW *em) {
    EM14W *w = (EM14W *)em->ex;
    f32 v[3];
    int a;

    a = (u16)(Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]);
    if (a <= 0x8000) {
        if (a <= 0x3F) {
            em->ang[1] += a;
        } else {
            em->ang[1] += 0x40;
        }
    } else {
        if (a > 0xFFC0) {
            em->ang[1] += a;
        } else {
            em->ang[1] -= 0x40;
        }
    }
    mot_miration_ret(em, v);
    w->dist -= v[2];
}

void em14_fly_adjy(EMW *em, int type) {
    f32 (*tbl)[2] = fly_adjy_hosei_tbl_0065EE60[type];
    EM14W *w = (EM14W *)em->ex;
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

void em14_fly_adjy2_init(EMW *em, u8 type) {
    EM14W *w = (EM14W *)em->ex;

    w->adj_x = 0;
    w->adj_y = 0;
    w->adj_z = 0;
    w->adj_type = type;
    switch (w->adj_type) {
    case 2:
        w->adj_y = 1;
        w->adj_tm = 0x46;
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

static u8 fly_adjy2_subx(EMW *em, EM14W *w) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM14W *w) {
    int i;
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_0065F260[w->adj_type];
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

static u8 fly_adjy2_subz(EMW *em, EM14W *w) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_0065F420[w->adj_type];
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

u8 em14_fly_adjy2(EMW *em) {
    EM14W *w = (EM14W *)em->ex;
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
