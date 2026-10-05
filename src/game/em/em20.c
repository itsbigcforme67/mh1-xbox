/* em20 - game.bin 0x005FCDC0-0x005FD898 (setters; em20_act_set is still asm,
 * 1 instruction off, near-match in em20_nm.c; the rest is em20b.c). Action setters for monster 20
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
void em20_act_set(EMW *em, int kind, u16 no, u16 arg);
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
