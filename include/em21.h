#ifndef EM21_H
#define EM21_H
/* em21 (monster kind 21, a swimmer/flyer) work area at EMW+0x444, used by
 * src/game/em/em21*.c. Offsets come from the code in f_em_5FFFD0 and em21.c;
 * the names are guesses. */
#include "em.h"

typedef struct EM21W {
    u8 eff;             /* 0x00 em21_effect_move step */
    u8 _pad01[3];
    s16 anim;           /* 0x04 animation the sound/effect script follows */
    u8 _pad06;
    u8 x07;             /* 0x07 4 when circling (fly 10/11) */
    u8 _pad08[2];
    u16 tgt_ang;        /* 0x0A angle toward the target */
    u8 _pad0C;
    u8 has_tgt;         /* 0x0D */
    u8 _pad0E;
    u8 hire_mode;       /* 0x0F 0..3, see hire_req_set */
    s16 x10;            /* 0x10 */
    u8 _pad12;
    u8 x13;             /* 0x13 set by atk action 1 */
    s32 x14;            /* 0x14 */
    s32 x18;            /* 0x18 */
    s32 spd[3];         /* 0x1C passed to speed_add(_g) (angle in [1]) */
    s32 x28;            /* 0x28 */
    s32 x2C;            /* 0x2C */
    s32 x30;            /* 0x30 */
    f32 dist;           /* 0x34 distance to the target (1000 with none) */
    u8 x38;             /* 0x38 */
    u8 x39;             /* 0x39 */
    u8 x3A;             /* 0x3A */
    u8 _pad3B;
    u8 adj_x;           /* 0x3C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x3D */
    u8 adj_z;           /* 0x3E */
    u8 adj_type;        /* 0x3F table row */
    s16 adj_tm;         /* 0x40 time into the table */
    s16 hire_tm[4];     /* 0x42 per part timers (hire_move_sub1) */
    u8 hire_st[4][2];   /* 0x4A per part state: [0] sub1 (0..3), [1] sub2 (0..2) */
    u16 hire_ang[4][2]; /* 0x52 per part angles: [0] sub2, [1] sub1 */
    s16 hire_cnt[4];    /* 0x62 per part frame counters */
    s8 hire_cnt2[4];    /* 0x6A per part frame counters (sub2) */
    u8 _pad6E[0xA3 - 0x6E];
    u8 xA3;             /* 0xA3 cleared at init */
} EM21W;

#endif
