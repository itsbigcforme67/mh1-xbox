#ifndef EM14_H
#define EM14_H
/* em14 (monster kind 14, a swimmer/flyer) work area at EMW+0x444, used by src/game/em/em14*.c
 * (em14.c has its own copy of the first fields). Offsets from f_em_5B5290 and em14.c; names are guesses. */
#include "em.h"

typedef struct EM14W {
    u8 eff;             /* 0x00 em14_effect_move step */
    u8 _pad01;
    s16 anim;           /* 0x02 animation the sound/effect script follows */
    u8 _pad04[2];
    s16 x06;            /* 0x06 */
    u8 _pad08[8];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 tgt_ang;        /* 0x14 angle toward the target (em14.c: dang) */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    s8 x18;             /* 0x18 */
    u8 _pad19;
    u8 x1A;             /* 0x1A */
    u8 x1B;             /* 0x1B */
    s32 x1C;            /* 0x1C */
    s32 x20;            /* 0x20 */
    u8 _pad24[0xC];
    s32 spd[3];         /* 0x30 passed to speed_add(_g) (angle in [1]) */
    s32 x3C;            /* 0x3C */
    s32 turn;           /* 0x40 maximum turn per call */
    s32 x44;            /* 0x44 */
    u8 adj_x;           /* 0x48 fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x49 */
    u8 adj_z;           /* 0x4A */
    u8 adj_type;        /* 0x4B table row */
    s16 adj_tm;         /* 0x4C time into the table */
    s16 x4E;            /* 0x4E */
    u8 _pad50[0x53];
    u8 xA3;             /* 0xA3 */
} EM14W;

#endif
