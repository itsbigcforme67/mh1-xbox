#ifndef EM15_H
#define EM15_H
/* em15 (monster kind 15, a ceiling/flying monster) work area at EMW+0x444, used by
 * src/game/em/em15*.c (em15.c has its own copy of the first fields). Offsets from the code of
 * f_em_5C2A80 and em15.c; names are guesses. */
#include "em.h"

typedef struct EM15W {
    u8 eff;             /* 0x00 em15_effect_move step */
    u8 _pad01;
    s16 anim;           /* 0x02 animation the sound/effect script follows */
    u8 adj_x;           /* 0x04 fly_adjy2 channels on/off (em15.c) */
    u8 adj_y;           /* 0x05 */
    u8 adj_z;           /* 0x06 */
    u8 adj_type;        /* 0x07 table row */
    s16 adj_tm;         /* 0x08 time into the table */
    u8 x0A;             /* 0x0A 4 when circling (fly 29/30) */
    u8 _pad0B[5];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 tgt_ang;        /* 0x14 angle toward the target (em15.c calls it dang) */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    u8 _pad18[4];
    s32 x1C;            /* 0x1C */
    s32 spd[3];         /* 0x20 passed to speed_add(_g) (angle in [1]) */
    s32 x2C;            /* 0x2C */
    s32 x30;            /* 0x30 maximum turn per call */
    s32 x34;            /* 0x34 */
    u8 _pad38[8];
    u16 x40;            /* 0x40 passed to GetTenjoHit (ceiling area) */
    u16 x42;            /* 0x42 */
    u8 x44;             /* 0x44 */
    u8 x45;             /* 0x45 */
    u8 _pad46[0x5D];
    u8 xA3;             /* 0xA3 cleared at init */
} EM15W;

#endif
