/* em20 - game.bin 0x005FCDC0-0x005FFCB4. Action setters for monster 20
 * (a flying monster, close to em01 and em17): ground_point_search picks
 * the stage point farthest from the target player to land on, three
 * senkai_sub variants bank and turn in flight, xang_set_pl pitches toward
 * a player, plus senkai_player / senkai_target / fly_adjy / fly_adjy2 as in
 * em02. Meanings of the constants are guesses. */
#include "em.h"
#include "pl.h"
#include "game.h"

/* em20's part of the per-monster work at EMW+0x444. */
typedef struct EM20W {
    u8 _pad00[5];
    u8 x05;             /* 0x05 fly mode (4 circling, 1 fly 19) */
    u8 _pad06[2];
    u8 x08;             /* 0x08 cleared by every em20_act_set */
    u8 _pad09[7];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    u8 _pad18[4];
    s32 turn_left;      /* 0x1C */
    s32 bank_max;       /* 0x20 bank limit for senkai_sub */
    u8 _pad24[0x30 - 0x24];
    s32 spd[3];         /* 0x30 passed to speed_add(_g) (angle in [1]) */
    s32 pitch_spd;      /* 0x3C ang[0] step */
    s32 turn;           /* 0x40 maximum turn per call */
    s32 bank_spd;       /* 0x44 ang[2] step */
    u8 _pad48[4];
    u8 adj_x;           /* 0x4C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x4D */
    u8 adj_z;           /* 0x4E */
    u8 adj_type;        /* 0x4F table row */
    s16 adj_tm;         /* 0x50 time into the table */
} EM20W;

typedef struct STAGE_DATA {
    u8 _pad00[0x18];
    f32 floor_y;        /* 0x18 */
} STAGE_DATA;

extern f32 (*fly_adjy_hosei_tbl_0066E660[])[2];
extern f32 (*fly_adjy2_hosei_tbl_0066EAD0[])[2];
extern f32 (*fly_adjz2_hosei_tbl_0066ECD0[])[2];

f32 CalcDistanceXZ(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_cdm_act_flag_ck(EMW *);
void em_rate_clear(EMW *);
void target_kind_set(EMW *, f32 *);
void cmd_target_kind_set(EMW *, f32 *);
void em20_horm_init(EMW *);
s16 act_ck(EMW *, u16, u16);
STAGE_DATA *Stage_data_get(u8);
void em_char_set(EMW *, int, int, int);
void em_pl_pos_set(EMW *, u8, f32 *);
void World_calc2(u8, f32 *, f32 *);
void SetVector(f32 *, f32, f32, f32);
u16 calc_vec_ang(f32, f32, f32, f32);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
void em20_act_set();
void em20_ground_point_search(EMW *em);

#define DIST_CLAMP(w, d)            \
    w->dist -= d;                   \
    if (w->dist <= 0.0f) {          \
        w->dist = 0.0f;             \
    }

void em20_act_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    switch (no) {
    case 1:
        if (em->x888 == 1) {
            no = 0x11;
        }
        break;
    }
    em->x388 = 0;
    em_act_set2(em, 0, no, arg);
}

void em20_move_act_set(EMW *em, u16 no, u16 arg) {
    EM20W *w = (EM20W *)em->ex;

    em->x388 = 0;
    em->act_spd = 1.0f;
    switch (no) {
    case 0:
        if (em->kind == 20) {
            if (em->x302 < (s16)(0.2f * em->x792)) {
                no = 6;
                goto act6;
            }
        } else {
            if (em->x302 < (s16)(0.2f * em->x792)) {
                no = 6;
                goto act6;
            }
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
        em->act_spd = 0.8f;
    case 2:
    case 4:
    case 10:
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

void em20_fly_act_set(EMW *em, u16 no, u16 arg) {
    EM20W *w = (EM20W *)em->ex;

    em->act_spd = 1.0f;
    switch (no) {
    case 1:
        if (em->x8C3 == 0 && !(em->adj_z < 50.0f)) {
            no = 0x12;
        }
        break;
    case 6:
    case 17:
        w->x05 = 4;
        if (em->x8C3 == 0) {
            em->x829 = em->x39A & 3;
            em->x827 = 2;
            em->x828 = 2;
            cmd_target_kind_set(em, em->tgt_pos);
            em->work08 = 1.5f * CalcDistanceXZ(em->pos, em->tgt_pos) / 50.0f;
        }
        break;
    case 7:
    case 21:
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->work08 = (int)(flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z) * 3;
        break;
    case 8:
    case 13:
        if (em->x8C3 == 0 && em->x827 != 3) {
            em20_act_set(em, 2, 0, 1);
            return;
        }
        break;
    case 9:
        if (em->x8C3 == 0) {
            em->x827 = 2;
            if (em->x84D != 0) {
                em->x828 = 3;
                if (game_w.x2E == 6) {
                    if (em->kind == 6) {
                        em->x829 = 1;
                    } else {
                        em->x829 = 0;
                    }
                } else {
                    em->x829 = 0;
                }
            } else {
                em->x828 = 0;
            }
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
    case 19:
        w->x05 = 1;
        if (em->x8C3 == 0) {
            em->work08 = 1.5f * CalcDistanceXZ(em->pos, em->tgt_pos) / 50.0f;
        }
        break;
    case 24:
        if (em->adj_z <= 0.0f) {
            em->adj_z = 20.0f;
        }
        if (em->x8C3 == 0) {
            em->work08 = flvecCalcDistance(em->pos, em->tgt_pos) / em->adj_z;
        }
        break;
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 10:
    case 12:
    case 14:
    case 15:
    case 16:
    case 18:
    case 20:
    case 22:
    case 23:
        break;
    default:
        return;
    }
    em_act_set2(em, 2, no, arg);
}

void em20_atk_act_set(EMW *em, u16 no, u16 arg) {
    EM20W *w = (EM20W *)em->ex;
    f32 p[3];
    f32 d;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 1:
    case 10:
    case 11:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
        em20_horm_init(em);
        break;
    case 4:
    case 9:
        em->work08 = 1800;
        break;
    case 2:
        em20_horm_init(em);
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
    case 8:
    case 21:
        if (em->x8C3 == 0) {
            em20_ground_point_search(em);
            em->work08 = 1.5f * CalcDistanceXZ(em->pos, em->tgt_pos) / 50.0f;
        }
        break;
    case 3:
    case 5:
    case 6:
    case 7:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 19:
    case 20:
    case 22:
    case 29:
    case 30:
    case 31:
        break;
    case 18:
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

void em20_act_set(em, kind, no, arg) EMW *em; u16 kind; u16 no; u16 arg; {
    EM20W *w = (EM20W *)em->ex;

    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    w->x08 = 0;
    if (em->x8C3 == 0 && kind != 5 && kind != 4) {
        if (act_ck(em, 3, 9) != 0 || act_ck(em, 3, 4) != 0) {
            kind = 3;
            no = 7;
        }
    }
    switch (kind) {
    case 0:
        em20_act_act_set(em, no, arg);
        break;
    case 1:
        em20_move_act_set(em, no, arg);
        break;
    case 2:
        em20_fly_act_set(em, no, arg);
        break;
    case 3:
        em20_atk_act_set(em, no, arg);
        break;
    case 4:
    case 5:
    case 6:
        em->act_spd = 1.0f;
        em_act_set2(em, kind, no, arg);
        break;
    }
}

void em20_ground_point_search(EMW *em) {
    u32 i;
    EM_STG_POS *p;
    f32 (*pos)[3];
    f32 pp[3];
    f32 dist[4];
    f32 max;
    f32 *q;
    f32 (*q2)[3];
    u8 n;

    if (em->x617 == -1) {
        em20_act_set(em, 2, 6, 1);
        return;
    }
    pos = 0;
    p = em->area->stg_pos;
    for (i = 0; i < 88; p++, i++) {
        if (p->stg == -1 || em->area->stg_pos[i].stg == em->stg) {
            pos = em->area->stg_pos[i].pos;
            break;
        }
    }
    if (pos == 0) {
        em->tgt_pos[0] = 1000.0f;
        em->tgt_pos[1] = 0.0f;
        em->tgt_pos[2] = 1000.0f;
        return;
    }
    q2 = pos;
    em_pl_pos_set(em, em->x617, pp);
    for (i = 0; i < 4; i++, q2++) {
        dist[i] = CalcDistanceXZ(pp, *q2);
    }
    n = 0;
    max = dist[0];
    if (max < dist[1]) {
        max = dist[1];
        n = 1;
    }
    if (max < dist[2]) {
        max = dist[2];
        n = 2;
    }
    if (max < dist[3]) {
        n = 3;
    }
    q = pos[n];
    em->x827 = 2;
    em->x828 = 2;
    em->x829 = n;
    em->x881 = em->x827;
    em->x882 = em->x828;
    em->x883 = em->x829;
    em->tgt_pos[0] = q[0];
    em->tgt_pos[1] = q[1];
    em->tgt_pos[2] = q[2];
}

void em20_senkai_sub(EMW *em, int flags, int mode) {
    EM20W *w = (EM20W *)em->ex;
    STAGE_DATA *sd = Stage_data_get(em->stg);
    int a;
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
            int b = em->ang[2];
            if (b <= 0x4000) {
                em->ang[2] = b - w->bank_spd * 2;
            } else if (b <= w->bank_max || b > 0x10000 - w->bank_max) {
                em->ang[2] -= w->bank_spd;
            } else if (b != 0x10000 - w->bank_max) {
                em->ang[2] = b + w->bank_spd;
            }
        } else {
            int b = em->ang[2];
            if (b >= 0xC000) {
                em->ang[2] = b + w->bank_spd * 2;
            } else if (b < w->bank_max || b >= 0x10000 - w->bank_max) {
                em->ang[2] += w->bank_spd;
            } else if (b != w->bank_max) {
                em->ang[2] = b - w->bank_spd;
            }
        }
    } else {
        int b = em->ang[2];
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
        int b = em->ang[2];
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

void em20_senkai_sub2(EMW *em, int flags, int mode) {
    EM20W *w = (EM20W *)em->ex;
    STAGE_DATA *sd = Stage_data_get(em->stg);
    int a;
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
            int b = em->ang[2];
            if (b <= 0x4000) {
                em->ang[2] = b - w->bank_spd * 2;
            } else if (b <= w->bank_max || b > 0x10000 - w->bank_max) {
                em->ang[2] -= w->bank_spd;
            } else if (b != 0x10000 - w->bank_max) {
                em->ang[2] = b + w->bank_spd;
            }
        } else {
            int b = em->ang[2];
            if (b >= 0xC000) {
                em->ang[2] = b + w->bank_spd * 2;
            } else if (b < w->bank_max || b >= 0x10000 - w->bank_max) {
                em->ang[2] += w->bank_spd;
            } else if (b != w->bank_max) {
                em->ang[2] = b - w->bank_spd;
            }
        }
    } else {
        int b = em->ang[2];
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
        w->turn = 0x100;
    } else {
        w->turn = (a - 0x1000) / 16 + 0x100;
    }
    a = em->ang[2];
    if (a != 0) {
        if (a >= 0xC000) {
            if (w->turn_left <= w->turn) {
                em->ang[1] += w->turn_left;
                w->turn_left = 0;
            } else {
                em->ang[1] += w->turn;
                w->turn_left -= w->turn;
            }
        } else if (a <= 0x4000) {
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
    if (!(flags & 4)) {
        if (em->char0 == 0x3F4) {
            if (em->x194 == 0) {
                em_char_set(em, 14, 0, 0);
            }
            if (!(em->tgt_pos[1] < em->pos[1])) {
                em->pos[1] += 5.0f;
                if (em->adj_z < 80.0f) {
                    em->adj_z += 1.0f;
                }
            } else if (!(1500.0f + sd->floor_y < em->pos[1])) {
                em->pos[1] += 3.0f;
                if (em->adj_z < 80.0f) {
                    em->adj_z += 0.5f;
                }
            } else if (!(2000.0f + sd->floor_y < em->pos[1])) {
                em->pos[1] += 2.0f;
                if (em->adj_z < 80.0f) {
                    em->adj_z += 0.5f;
                }
            } else {
                if (!(2100.0f + sd->floor_y < em->pos[1])) {
                    em->pos[1] += 1.0f;
                }
                if (em->adj_z < 80.0f) {
                    em->adj_z += 1.0f;
                }
            }
        } else if (!(2000.0f + sd->floor_y < em->pos[1]) || em->adj_z <= 60.0f) {
            em_char_set(em, 12, 0, 0);
        }
    }
    if (em->char0 != 0x3F4 && em->x194 == 0) {
        int b = em->ang[2];
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

void em20_senkai_sub3(EMW *em, int flags, int mode) {
    EM20W *w = (EM20W *)em->ex;
    int a;
    f32 r;

    Stage_data_get(em->stg);
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
            int b = em->ang[2];
            if (b <= 0x4000) {
                em->ang[2] = b - w->bank_spd * 2;
            } else if (b <= w->bank_max || b > 0x10000 - w->bank_max) {
                em->ang[2] -= w->bank_spd;
            } else if (b != 0x10000 - w->bank_max) {
                em->ang[2] = b + w->bank_spd;
            }
        } else {
            int b = em->ang[2];
            if (b >= 0xC000) {
                em->ang[2] = b + w->bank_spd * 2;
            } else if (b < w->bank_max || b >= 0x10000 - w->bank_max) {
                em->ang[2] += w->bank_spd;
            } else if (b != w->bank_max) {
                em->ang[2] = b - w->bank_spd;
            }
        }
    } else {
        int b = em->ang[2];
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
        w->turn = 0x100;
    } else {
        w->turn = (a - 0x1000) / 16 + 0x100;
    }
    a = em->ang[2];
    if (a != 0) {
        if (a >= 0xC000) {
            if (w->turn_left <= w->turn) {
                em->ang[1] += w->turn_left;
                w->turn_left = 0;
            } else {
                em->ang[1] += w->turn;
                w->turn_left -= w->turn;
            }
        } else if (a <= 0x4000) {
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
    if (!(flags & 4)) {
        if (em->char0 == 0x3F4) {
            if (em->x194 == 0) {
                em_char_set(em, 14, 0, 0);
            }
            if (!(em->tgt_pos[1] < em->pos[1])) {
                em->pos[1] += 5.0f;
                if (em->adj_z < 80.0f) {
                    em->adj_z += 1.0f;
                }
            } else {
                if (!(em->tgt_pos[1] < em->pos[1])) {
                    em->pos[1] += 1.0f;
                }
                if (em->adj_z < 80.0f) {
                    em->adj_z += 1.0f;
                }
            }
        } else if (!(em->tgt_pos[1] < em->pos[1]) || em->adj_z <= 60.0f) {
            em_char_set(em, 12, 0, 0);
        }
    }
    if (em->char0 != 0x3F4 && em->x194 == 0) {
        int b = em->ang[2];
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

void em20_senkai_player(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
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

u16 em20_senkai_target(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
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

void em20_xang_set_pl(EMW *em, int mode, f32 h) {
    EM20W *w = (EM20W *)em->ex;
    f32 p[3];
    int a;
    s8 n = em->x617;

    if (n == -1) {
        return;
    }
    switch (mode) {
    case 0:
        em_pl_pos_set(em, n, p);
        a = 0x10000 - calc_vec_ang(flvecCalcDistance(em->pos, p), em->pos[1], 0.0f, p[1] + h);
        a = (u16)(a - em->ang[0]);
        break;
    case 1:
        a = (u16)(0xD000 - em->ang[0]);
        break;
    case 2:
        a = (u16)-em->ang[0];
        break;
    }
    if (a != 0) {
        if (a <= 0x7FFF) {
            if (em->ang[0] < 0x3000 || em->ang[0] >= 0xD000) {
                if (a <= w->pitch_spd) {
                    em->ang[0] += a;
                } else {
                    em->ang[0] += w->pitch_spd;
                }
            }
        } else {
            if (em->ang[0] > 0xD000 || em->ang[0] <= 0x3000) {
                if (a >= 0x10000 - w->pitch_spd) {
                    em->ang[0] -= 0x10000 - a;
                } else {
                    em->ang[0] -= w->pitch_spd;
                }
            }
        }
        em->ang[0] = (u16)em->ang[0];
    }
}

void em20_fly_adjy(EMW *em, int type) {
    f32 (*tbl)[2] = fly_adjy_hosei_tbl_0066E660[type];
    EM20W *w = (EM20W *)em->ex;
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

void em20_fly_adjy2_init(EMW *em, u8 type) {
    EM20W *w = (EM20W *)em->ex;

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
    case 14:
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
        w->adj_tm = 0x24;
        break;
    case 13:
        w->adj_y = 1;
        w->adj_tm = 0;
        break;
    }
    em_rate_clear(em);
}

static u8 fly_adjy2_subx(EMW *em, EM20W *w) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM20W *w) {
    int i;
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_0066EAD0[w->adj_type];
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

static u8 fly_adjy2_subz(EMW *em, EM20W *w) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_0066ECD0[w->adj_type];
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

u8 em20_fly_adjy2(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
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
