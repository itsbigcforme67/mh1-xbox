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

#ifdef VERSION_MH2
/* MH2's places (PLW 0x970; enemy_trans passes an EMW, whose fields here sit at the same places), read from MH2's own
 * display functions (sub_main): player_trans 0x373D30, Lb_player_trans 0x374810, weapon_trans 0x370E00, pl_item_trans
 * 0x36FBB0, pl_item_trans_sub 0x36F460, enemy_trans 0x376640 (SHARED agent, round 4). 'map only': from include/mh2_ofs.h,
 * no MH2 read seen. MH2 changed the logic of these functions too (sizes differ): only the places are MH2's. */
typedef struct PLX {
    u8 be_flag;              /* 0x000 (MH1 0x000) */
    u8 x01;                  /* 0x001 (MH1 0x001) */
    u8 kind;                 /* 0x002 (MH1 0x002) */
    u8 _pad003[0xC - 0x3];
    u16 id;                  /* 0x00C (MH1 0x00C) */
    u8 _pad00E[0x11 - 0xE];
    u8 x11;                  /* 0x011 (MH1 0x011): map only */
    u8 flag12;               /* 0x012 (MH1 0x012): map only */
    u8 _pad013[0x14 - 0x13];
    u8 flag14;               /* 0x014 (MH1 0x014): map only */
    u8 _pad015[0x1C - 0x15];
    u8 work1C;               /* 0x01C (MH1 0x01C): map only */
    u8 _pad01D[0x60 - 0x1D];
    u8 rot[0x40];            /* 0x060 (MH1 0x060) */
    s32 ang[3];              /* 0x0A0 (MH1 0x0A0) */
    f32 pos[3];              /* 0x0AC (MH1 0x0AC) */
    f32 scl[3];              /* 0x0B8 (MH1 0x0B8) */
    u8 _pad0C4[0xD0 - 0xC4];
    u8 *part[0x18];          /* 0x0D0 (MH1 0x110): weapon_trans lw 0x114, player_trans 0x120 */
    u8 _pad130[0x184 - 0x130];
    s32 x1C4;                /* 0x184 (MH1 0x1C4): weapon_trans */
    u8 _pad188[0x29C - 0x188];
    u16 char0;               /* 0x29C (MH1 0x2DC): weapon_trans, Lb_player_trans */
    u8 _pad29E[0x30F - 0x29E];
    u8 x34F;                 /* 0x30F (MH1 0x34F): enemy_trans */
    u8 _pad310[0x360 - 0x310];
    u8 armor[0xC];           /* 0x360 (MH1 0x352): player_trans lbu 0x366 = armor[6]; 0xC to sw_now (MH1 0x12) */
    u16 sw_now;              /* 0x36C (MH1 0x364): player_trans */
    u8 _pad36E[0x390 - 0x36E];
    u8 st;                   /* 0x390 (MH1 0x388): f_sphr HENT */
    u8 _pad391[0x3A4 - 0x391];
    s32 work39C;             /* 0x3A4 (MH1 0x39C): map only */
    u8 _pad3A8[0x4C8 - 0x3A8];
    s8 vis[0x24];            /* 0x4C8 (MH1 0x4E6): player_trans, enemy_trans; MH1 0x26 entries, MH2 room for 0x24 */
    PLMDL *mdl;              /* 0x4EC (MH1 0x50C): player_trans, enemy_trans */
    u8 _pad4F0[0x4F4 - 0x4F0];
    PLMDL *wmdl;             /* 0x4F4 (MH1 0x514): weapon_trans */
    u8 _pad4F8[0x4FC - 0x4F8];
    PLMDL *amdl[6];          /* 0x4FC (MH1 0x534): player_trans, Lb_player_trans */
    u8 _pad514[0x533 - 0x514];
    u8 x56B;                 /* 0x533 (MH1 0x56B): pl_item_trans */
    u8 _pad534[0x574 - 0x534];
    f32 gy;                  /* 0x574 (MH1 0x5AC): pl_item_trans */
    u8 _pad578[0x580 - 0x578];
    u32 col5FC;              /* 0x580 (MH1 0x5FC): map only: MH2's player_trans has no read of it */
    u8 _pad584[0x683 - 0x584];
    u8 x739;                 /* 0x683 (MH1 0x739): player_trans */
    u8 _pad684[0x6AF - 0x684];
    u8 x763;                 /* 0x6AF (MH1 0x763): player_trans */
    u8 pch_on;               /* 0x6B0 (MH1 0x764): player_trans */
    u8 _pad6B1[0x6E4 - 0x6B1];
    f32 alpha;               /* 0x6E4 (MH1 0x798): player_trans, enemy_trans */
    u8 _pad6E8[0x7D4 - 0x6E8];
    s16 x87C;                /* 0x7D4 (MH1 0x87C): weapon_trans */
    u8 _pad7D6[0x7E2 - 0x7D6];
    u16 x88A;                /* 0x7E2 (MH1 0x88A): pl_item_trans(_sub) */
    u8 _pad7E4[0x81D - 0x7E4];
    u8 wpn_on;               /* 0x81D (MH1 0x8C5): player_trans */
    u8 _pad81E[0x828 - 0x81E];
    u8 x8D0;                 /* 0x828 (MH1 0x8D0): pl_item_trans_sub */
    u8 _pad829[0x970 - 0x829];
} PLX;
#else
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
    u16     char0;       /* 0x2DC current action id */
    u8  _pad2DE[0x71];
    u8      x34F;        /* 0x34F  */
    u8  _pad350[0x2];
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
    u8  _pad54C[0x1F];
    u8      x56B;        /* 0x56B low nibble: item in use, high nibble: which */
    u8  _pad56C[0x40];
    f32     gy;          /* 0x5AC ground height */
    u8  _pad5B0[0x4C];
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

#endif
