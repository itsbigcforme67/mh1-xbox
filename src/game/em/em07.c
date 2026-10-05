/* em07 - game.bin 0x00599ED0-0x0059A23C. Action setters for monster 7.
 * em07_act_set picks one of four action groups (0 act, 1 move, 2 fly,
 * 3 attack; 4-6 go straight to em_act_set2). Each sets the animation speed
 * to 1.0 and calls em_act_set2(em, group, no, arg). The move group first
 * measures the distance to the target and, for actions 1 and 0x13, swaps
 * in another action when the target is within 90 degrees of the facing
 * angle (meaning of the swap is a guess). */
#include "em.h"

/* em07's part of the per-monster work at EMW+0x444. */
typedef struct EM07W {
    u8 _pad00[0x10];
    f32 dist;           /* 0x10 distance to the target (500 with no target) */
    u8 _pad14[2];
    u8 has_tgt;         /* 0x16 */
} EM07W;

f32 CalcDistanceXZ(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_cdm_act_flag_ck(EMW *);

void em07_act_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    em_act_set2(em, 0, no, arg);
}

void em07_move_act_set(EMW *em, u16 no, u16 arg) {
    EM07W *mv = (EM07W *)em->ex;
    u16 a;

    em->act_spd = 1.0f;
    switch (no) {
    case 0:
    case 0x10:
        if (em->x881 == 0) {
            mv->has_tgt = 0;
            mv->dist = 500.0f;
        } else {
            mv->has_tgt = 1;
            mv->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        }
        break;
    case 1:
        if (em->x881 == 0) {
            mv->has_tgt = 0;
            mv->dist = 500.0f;
        } else {
            mv->has_tgt = 1;
            mv->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        }
        a = Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1];
        if ((0 <= a && a <= 0x4000) || (a >= 0xC000 && a <= 0xFFFF)) {
            no = 0;
        }
        break;
    case 2:
        Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 0x13:
        if (em->x881 == 0) {
            mv->has_tgt = 0;
            mv->dist = 500.0f;
        } else {
            mv->has_tgt = 1;
            mv->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        }
        a = Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1];
        if ((0 <= a && a <= 0x4000) || (a >= 0xC000 && a <= 0xFFFF)) {
            no = 0x12;
        }
        break;
    }
    em_act_set2(em, 1, no, arg);
}

void em07_fly_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    em_act_set2(em, 2, no, arg);
}

void em07_atk_act_set(EMW *em, u16 no, u16 arg) {
    em->act_spd = 1.0f;
    em_act_set2(em, 3, no, arg);
}

void em07_act_set(EMW *em, int kind, u16 no, u16 arg) {
    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    switch ((u16)kind) {
    case 0:
        em07_act_act_set(em, no, arg);
        break;
    case 1:
        em07_move_act_set(em, no, arg);
        break;
    case 2:
        em07_fly_act_set(em, no, arg);
        break;
    case 3:
        em07_atk_act_set(em, no, arg);
        break;
    case 4:
    case 5:
    case 6:
        em->act_spd = 1.0f;
        em_act_set2(em, kind, no, arg);
        break;
    }
}
