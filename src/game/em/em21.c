/* em21 - game.bin 0x0060C3A0-0x0060D3FC. Action setters for monster 21.
 * Same layout as em08 (fly_adjy2 and senkai_pos_no are identical), plus:
 * most fly and attack actions first fall back to em21_act_set(em, 0, 1, 1)
 * when x8C3 and x7E8 are clear and the monster is on the current stage
 * (game_w.stage); move action 3 (and 0 at low x302) sets a time in work08
 * from the distance (dist / 7, at most 300) or at random-looking
 * (x39A % 120 + 180) / 2 frames without a target. Meanings are guesses. */
#include "em.h"
#include "game.h"

#define EM21_STAGE_CK(em) \
    (em->x8C3 == 0 && game_w.stage == em->stg && em->x7E8 == 0)

/* em21's part of the per-monster work at EMW+0x444. */
typedef struct EM21W {
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
} EM21W;

extern f32 (*fly_adjy2_hosei_tbl_006712B0[])[2];
extern f32 (*fly_adjz2_hosei_tbl_00671418[])[2];

f32 CalcDistanceXZ(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_cdm_act_flag_ck(EMW *);
void em21_act_set(EMW *em, int kind, u16 no, u16 arg);
void target_kind_set(EMW *, f32 *);
void cmd_target_kind_set(EMW *, f32 *);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);
u8 em21_senkai_pos_no(EMW *em, f32 *out);

void em21_act_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    em->x388 = 0;
    em_act_set2(em, 0, no, arg);
}

void em21_move_act_set(EMW *em, u16 no, u16 arg) {
    EM21W *w = (EM21W *)em->ex;

    em->x388 = 0;
    em->act_spd = 1.0f;
    switch (no) {
    case 0:
        if (em->x302 < (s16)(0.2f * em->x792)) {
            no = 3;
            goto act3;
        }
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
        } else {
            if (em->x881 == 0) {
                w->has_tgt = 0;
                em->work08 = (em->x39A % 120 + 180) / 2;
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
            em->work08 = (int)w->dist / 7;
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
        break;
    case 2:
        break;
    case 3:
    act3:
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

void em21_fly_act_set(EMW *em, u16 no, u16 arg) {
    EM21W *w = (EM21W *)em->ex;
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
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 24:
    case 25:
        if (EM21_STAGE_CK(em)) {
            em21_act_set(em, 0, 1, 1);
            return;
        }
        break;
    case 4:
    case 5:
        if (EM21_STAGE_CK(em)) {
            em21_act_set(em, 0, 1, 1);
            return;
        }
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
        if (EM21_STAGE_CK(em)) {
            em21_act_set(em, 0, 1, 1);
            return;
        }
        no = 8;
        break;
    case 10:
    case 11:
        if (EM21_STAGE_CK(em)) {
            em21_act_set(em, 0, 1, 1);
            return;
        }
        w->x07 = 4;
        if (em->x8C3 == 0) {
            em->x829 = em21_senkai_pos_no(em, pt);
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
    case 16:
    case 23:
        break;
    default:
        return;
    }
    em_act_set2(em, 2, no, arg);
}

void em21_atk_act_set(EMW *em, u16 no, u16 arg) {
    EM21W *w = (EM21W *)em->ex;

    em->act_spd = 1.0f;
    switch (no) {
    case 3:
    case 4:
    case 5:
    case 6:
        if (EM21_STAGE_CK(em)) {
            em21_act_set(em, 0, 1, 1);
            return;
        }
        break;
    case 1:
        w->x13 = 1;
        break;
    case 0:
    case 2:
    case 7:
        break;
    default:
        return;
    }
    em_act_set2(em, 3, no, arg);
}

void em21_act_set(EMW *em, int kind, u16 no, u16 arg) {
    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    switch ((u16)kind) {
    case 0:
        em21_act_act_set(em, no, arg);
        break;
    case 1:
        em21_move_act_set(em, no, arg);
        break;
    case 2:
        em21_fly_act_set(em, no, arg);
        break;
    case 3:
        em21_atk_act_set(em, no, arg);
        break;
    case 4:
        if (no == 15) {
            switch (em->stg) {
            case 0x36:
                em->x827 = 2;
                em->x828 = 1;
                em->x829 = 7;
                break;
            case 3:
                em->x827 = 2;
                em->x828 = 1;
                em->x829 = 13;
                break;
            default:
                em->x827 = 2;
                em->x828 = 0;
                em->x829 = 0;
                break;
            }
            cmd_target_kind_set(em, em->tgt_pos);
        }
    case 5:
    case 6:
        em->act_spd = 1.0f;
        em_act_set2(em, kind, no, arg);
        break;
    }
}

void em21_fly_adjy2_init(EMW *em, u8 type) {
    EM21W *w = (EM21W *)em->ex;

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

static u8 fly_adjy2_subx(EMW *em, EM21W *w) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM21W *w) {
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_006712B0[w->adj_type];
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

static u8 fly_adjy2_subz(EMW *em, EM21W *w) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_00671418[w->adj_type];
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

u8 em21_fly_adjy2(EMW *em) {
    EM21W *w = (EM21W *)em->ex;
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

u8 em21_senkai_pos_no(EMW *em, f32 *out) {
    u32 i;
    EM_STG_POS *p;
    f32 (*pos)[3];
    f32 (*q2)[3];
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
    for (q2 = pos, i = 0; i < 4; i++, q2++) {
        dist[i] = CalcDistanceXZ(em->pos, *q2);
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
