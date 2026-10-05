#ifndef HIT3_H
#define HIT3_H
/* Stage hit data (f_sphr, src/main/hit/shit*.c): diorama_w holds the grid of
 * the wall ("HITS") and ground collision files of the loaded stage. Each
 * cell is a -1 terminated list of polygon pointers (after WallHitInit /
 * GroundHitInit fix the file offsets). */
#include "types.h"

typedef struct DIORAMA {
    u8 _pad00[8];
    s32 wcsx;           /* 0x08 wall: cell size x */
    s32 wcsz;           /* 0x0C cell size z */
    s32 wnz;            /* 0x10 cells along z (stride of x) */
    s32 wnx;            /* 0x14 cells along x */
    s32 **wtbl;         /* 0x18 cell table (cell lists of polygon pointers) */
    u8 *warea;          /* 0x1C polygon area */
    s32 gcsx;           /* 0x20 ground: cell size x */
    s32 gcsz;           /* 0x24 cell size z */
    s32 gnz;            /* 0x28 */
    s32 gnx;            /* 0x2C */
    s32 **gtbl;         /* 0x30 */
    u8 *garea;          /* 0x34 */
} DIORAMA;

extern DIORAMA diorama_w;

/* One 56-byte collision polygon of a HITS file. */
typedef struct HPOLY {
    u8 kind;            /* 0x00 index into the stage's ground_tbl_add (0x10-byte entries) */
    u8 b1;              /* 0x01 */
    u16 h2;             /* 0x02 */
    f32 v[3][3];        /* 0x04 vertices */
    f32 n[3];           /* 0x28 normal */
    f32 d;              /* 0x34 plane constant: n . p + d = 0 */
} HPOLY;

/* Per kind attributes (ground_tbl_add[stage][kind]), 0x10 bytes; only the
 * flag bytes the stage hit code reads are named. */
typedef struct HKIND {
    u8 _pad00[0xB];
    u8 lava;            /* 0x0B GetYouganHit */
    u8 water;           /* 0x0C GetWaterHit / GetGroundHit skip */
    u8 x0D;             /* 0x0D */
    u8 _pad0E[2];
} HKIND;

#endif
