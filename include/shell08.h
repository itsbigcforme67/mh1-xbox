#ifndef SHELL08_H
#define SHELL08_H
/* shell08: monster breath/projectile shells (fireballs and the like).
 * Types shared by the shell08 split files. Offsets from matched code. */
#include "shell.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

/* One particle of the shot (0x20 bytes, array at SH08W+0x38). */
typedef struct SH08P {
    s8 no;              /* 0x00 */
    u8 x01;             /* 0x01 */
    s16 x02;            /* 0x02 frame counter (negative = delay) */
    f32 scl[3];         /* 0x04 scale */
    u16 x10;            /* 0x10 */
    u16 x12;            /* 0x12 */
    u16 x14;            /* 0x14 */
    u16 x16;            /* 0x16 */
    u16 x18;            /* 0x18 */
    u16 x1A;            /* 0x1A */
    s8 x1C;             /* 0x1C */
    s8 x1D;             /* 0x1D */
    s8 x1E;             /* 0x1E */
    s8 x1F;             /* 0x1F */
} SH08P;

/* The work that sh->senko (0x18) points at for shell08. */
typedef struct SH08W {
    u8 x00;             /* 0x00 type 8: strikes left */
    u8 x01;             /* 0x01 type 8: strike number; type 1: pitch (<< 8) */
    s16 prim_no;        /* 0x02 second draw primitive */
    PRIM *prim;         /* 0x04 */
    VEC3 base;          /* 0x08 type 8: centre of the strikes */
    VEC3 x14;           /* 0x14 type 8: last strike position */
    u8 _pad20[0x2C - 0x20];
    s16 joint;          /* 0x2C joint the shot starts from */
    u16 x2E;            /* 0x2E timer */
    f32 x30;            /* 0x30 type 1: height the shot hits at */
    s16 char0;          /* 0x34 owner's animation */
    u8 x36;             /* 0x36 */
    u8 x37;             /* 0x37 type 1: yaw relative to the owner (<< 8) */
    SH08P p[1];         /* 0x38 */
} SH08W;

/* Colour key for shell08_rgba: colours are blended between keys. */
typedef struct RGBA_KEY {
    s32 time;           /* -1 ends the table */
    u32 rgba;
} RGBA_KEY;

#define SH08_W(sh) ((SH08W *)(sh)->senko)

void Shell08_set_shl(EMW *em, u8 arg, u8 x07, SHLW *src, f32 *pos);
void shell08_move(SHLW *sh);
void shell08_i(SHLW *sh);
void shell08_m(SHLW *sh);
void shell08_h(SHLW *sh);
void shell08_d(SHLW *sh);
void shell08_e(SHLW *sh);
void shell08_trans_sub(CLAY *cl, FLMAT *mat, u32 tex, int ope, void *mats);
void shell08_trans(PRIM *pr);
void shell08_impact_set(SHLW *sh, s16 kind, f32 *pos);
void shell08_type1_pos_set(SHLW *sh, SH08W *w);
void shell08_type1_impact_pos(SHLW *sh, SH08W *w, f32 *out);
void shell08_z_adj(FLMAT *m, f32 *pos);
void shell08_rgba(RGBA_KEY *k, int t, u32 *out);
void shell08_type6_init(SHLW *sh, SH08P *p);
void shell08_type8_init(SHLW *sh, SH08W *w, SH08P *p);
void shell08_type8_pos_set(SHLW *sh, SH08W *w);

#endif
