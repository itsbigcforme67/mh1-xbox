/* em17 - game.bin 0x005D81A0-0x005D9BC4. Action setters for monster 17,
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

void em17_senkai_sub(EMW *em, int flags, int mode) {
    EM17W *w = (EM17W *)em->ex;
    STAGE_DATA *sd = Stage_data_get(em->stg);
    int a;
    int b;
    f32 r;

    if (flags & 1) {
        w->turn_left = w->dang;
    }
    switch (mode) {
    case 0:
        a = w->turn_left;
        if (a >= 0x8000) {
            a = 0x10000 - a;
        }
        if (a <= 0x2000) {
            w->bank_max = 0x1000;
        } else if (a <= 0x4000) {
            w->bank_max = 0x1800;
        } else if (a <= 0x6000) {
            w->bank_max = 0x2000;
        } else if (a <= 0x8000) {
            w->bank_max = 0x3000;
        }
        if (em->adj_z <= 130.0f) {
            break;
        }
    case 1:
        a = w->turn_left;
        if (a >= 0x8000) {
            a = 0x10000 - a;
        }
        if (a <= 0x2000) {
            w->bank_max = 0x1800;
        } else if (a <= 0x4000) {
            w->bank_max = 0x2000;
        } else if (a <= 0x6000) {
            w->bank_max = 0x2800;
        } else if (a <= 0x8000) {
            w->bank_max = 0x3000;
        }
        break;
    }
    if (w->turn_left != 0) {
        if (w->turn_left <= 0x8000) {
            b = em->ang[2];
            if (b <= 0x4000) {
                em->ang[2] = b - w->bank_spd * 2;
            } else if (b <= w->bank_max || b > 0x10000 - w->bank_max) {
                em->ang[2] -= w->bank_spd;
            } else if (b != 0x10000 - w->bank_max) {
                em->ang[2] = b + w->bank_spd;
            }
        } else {
            b = em->ang[2];
            if (b >= 0xC000) {
                em->ang[2] = b + w->bank_spd * 2;
            } else if (b < w->bank_max || b >= 0x10000 - w->bank_max) {
                em->ang[2] += w->bank_spd;
            } else if (b != w->bank_max) {
                em->ang[2] = b - w->bank_spd;
            }
        }
    } else {
        b = em->ang[2];
        if (b != 0) {
            if (b <= w->bank_max) {
                em->ang[2] = b - w->bank_spd;
            } else if (b >= 0x10000 - w->bank_max) {
                em->ang[2] = b + w->bank_spd;
            }
        }
    }
    a = em->ang[2];
    a = (a < 0x8000) ? a : (u16)(0x10000 - a);
    if (a <= 0x1000) {
        w->turn = 0x80;
    } else {
        w->turn = (a - 0x1000) / 16 + 0x80;
    }
    a = em->ang[2];
    if (a != 0) {
        if (a <= 0xF000 && a >= 0xC000) {
            if (w->turn_left <= w->turn) {
                em->ang[1] += w->turn_left;
                w->turn_left = 0;
            } else {
                em->ang[1] += w->turn;
                w->turn_left -= w->turn;
            }
        } else if (a >= 0x1000 && a <= 0x4000) {
            if (w->turn_left >= 0x10000 - w->turn) {
                em->ang[1] -= 0x10000 - w->turn_left;
                w->turn_left += 0x10000 - w->turn_left;
            } else {
                em->ang[1] -= w->turn;
                w->turn_left += w->turn;
            }
        }
    }
    em->ang[1] = (u16)em->ang[1];
    w->turn_left = (u16)w->turn_left;
    a = em->ang[2];
    if (a >= 0x8000) {
        a = 0x10000 - a;
    }
    if (a < 0x1000) {
        r = 0.1f;
    } else {
        r = 2.0f * (3.1415927f * ((360.0f * a / 65536.0f) / 360.0f));
    }
    if (!(flags & 4)) {
        em->pos[1] -= r;
        if (!(em->adj_z < 50.0f) && r != 0.1f) {
            em->adj_z = em->adj_z - r;
        }
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
    if (!(flags & 4)) {
        if (em->char0 == 0x3F4) {
            if (em->x194 == 0) {
                em_char_set(em, 14, 0, 0);
            }
            if (!(1000.0f + sd->floor_y < em->pos[1])) {
                em->pos[1] += 5.0f;
                if (em->adj_z < 120.0f) {
                    em->adj_z += 1.0f;
                }
            } else if (!(1500.0f + sd->floor_y < em->pos[1])) {
                em->pos[1] += 3.0f;
                if (em->adj_z < 120.0f) {
                    em->adj_z += 0.5f;
                }
            } else if (!(2000.0f + sd->floor_y < em->pos[1])) {
                em->pos[1] += 2.0f;
                if (em->adj_z < 120.0f) {
                    em->adj_z += 0.5f;
                }
            } else {
                if (!(2100.0f + sd->floor_y < em->pos[1])) {
                    em->pos[1] += 1.0f;
                }
                if (em->adj_z < 120.0f) {
                    em->adj_z += 1.0f;
                }
            }
        } else if (!(2000.0f + sd->floor_y < em->pos[1]) || em->adj_z <= 60.0f) {
            em_char_set(em, 12, 0, 0);
        }
    }
    if (em->char0 != 0x3F4 && em->x194 == 0) {
        b = em->ang[2];
        if (b <= 0x7FFF) {
            if (b > 0x2000) {
                if (em->char0 != 0x43E && em->x194 == 0) {
                    em_char_set(em, 0x56, 0, 0);
                }
            } else if (b > 0x1000) {
                if (em->char0 != 0x441 && em->x194 == 0) {
                    em_char_set(em, 0x59, 0, 0);
                }
            } else if (em->char0 != 0x3F6 && em->x194 == 0) {
                em_char_set(em, 14, 0, 0);
            }
        } else if (b <= 0xDFFF) {
            if (em->char0 != 0x43D && em->x194 == 0) {
                em_char_set(em, 0x55, 0, 0);
            }
        } else if (b <= 0xEFFF) {
            if (em->char0 != 0x440 && em->x194 == 0) {
                em_char_set(em, 0x58, 0, 0);
            }
        } else if (em->char0 != 0x3F6 && em->x194 == 0) {
            em_char_set(em, 14, 0, 0);
        }
    }
    if (!(flags & 2)) {
        a = em->ang[0];
        if (a != 0) {
            if (a <= 0x7FFF) {
                em->ang[0] = a - w->pitch_spd;
            } else {
                em->ang[0] = a + w->pitch_spd;
            }
            em->ang[0] = (u16)em->ang[0];
        }
    }
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
