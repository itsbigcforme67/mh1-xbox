#ifndef TRANS_PL_H
#define TRANS_PL_H
/* Overlay of the PLW fields the display code (f_weapon, 0x164F70-0x169230)
 * reads. Same layout as PLW (pl.h), offsets in comments; kept separate so
 * display-only names do not touch the shared header. Guesses where noted. */
#include "types.h"
#include "fl.h"
#include "clay.h"

/* player / armor / item model: clay list at +0x30, skin at +0x24 */
typedef struct PLMDL {
    u8 _pad00[0x10];
    u8 *mat;            /* 0x10 material table (0x4C bytes each) */
    u8 _pad14[0x24 - 0x14];
    u8 *skin;           /* 0x24 skin handle (s16 at +0xC2: joint count) */
    u8 _pad28[0x2C - 0x28];
    s16 num;            /* 0x2C clay count */
    u8 _pad2E[0x30 - 0x2E];
    CLAY *clay;         /* 0x30 */
    u8 _pad34[0x44 - 0x34];
    void *skel;         /* 0x44 skeleton nodes (0x190 bytes each, matrix at +0x40) */
    void *skel2;        /* 0x48 second skeleton (weapon hand parts) */
} PLMDL;

/* skeleton node: 0x190 bytes, matrix at +0x40 */
typedef struct WNODE {
    u8 _pad00[0x40];
    FLMAT m;            /* 0x40 */
    u8 _pad80[0x190 - 0x80];
} WNODE;

typedef struct PLX {
    u8      be_flag;     /* 0x000  */
    u8      x01;         /* 0x001  */
    u8      kind;        /* 0x002 character kind */
    u8  _pad003[0x9];
    u16     id;          /* 0x00C  */
    u8  _pad00E[0x3];
    u8      x11;         /* 0x011  */
    u8      flag12;      /* 0x012  */
    u8  _pad013[0x1];
    u8      flag14;      /* 0x014  */
    u8  _pad015[0x7];
    u8      work1C;      /* 0x01C  */
    u8  _pad01D[0x43];
    u8      rot[0x40];   /* 0x060 world matrix */
    s32     ang[3];      /* 0x0A0  */
    f32     pos[3];      /* 0x0AC  */
    f32     scl[3];      /* 0x0B8  */
    u8  _pad0C4[0x4C];
    u8 *    part[0x18];  /* 0x110 joint matrix blocks (+0x40 = world matrix) */
    u8  _pad170[0x54];
    s32     x1C4;        /* 0x1C4  */
    u8  _pad1C8[0x114];
    u16     char0;       /* 0x2DC  */
    u8  _pad2DE[0x74];
    u8      armor[0x12]; /* 0x352 (+6+i: armor id of part i) */
    u16     sw_now;      /* 0x364  */
    u8  _pad366[0x22];
    u8      st;          /* 0x388  */
    u8  _pad389[0x13];
    s32     work39C;     /* 0x39C  */
    u8  _pad3A0[0x146];
    s8      vis[0x26];   /* 0x4E6 per-clay visibility */
    PLMDL * mdl;         /* 0x50C  */
    u8  _pad510[0x4];
    PLMDL * wmdl;        /* 0x514 weapon model */
    u8  _pad518[0x1C];
    PLMDL * amdl[6];     /* 0x534 armor part models */
    u8  _pad54C[0xB0];
    u32     col5FC;      /* 0x5FC  */
    u8  _pad600[0x139];
    u8      x739;        /* 0x739  */
    u8  _pad73A[0x29];
    u8      x763;        /* 0x763  */
    u8      pch_on;      /* 0x764  */
    u8  _pad765[0x33];
    f32     alpha;       /* 0x798  */
    u8  _pad79C[0xE0];
    s16     x87C;        /* 0x87C  */
    u8  _pad87E[0xC];
    u16     x88A;        /* 0x88A  */
    u8  _pad88C[0x39];
    u8      wpn_on;      /* 0x8C5  */
    u8  _pad8C6[0xA];
    u8      x8D0;        /* 0x8D0  */
    u8  _pad8D1[0x12F];
} PLX;

#endif
