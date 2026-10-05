/* Memory card work struct (MemcardWork, 0xB4 bytes) and the per-game save file
 * description tables used by the McAct* layer (main 0x27FDF0-0x280EF0). Field names are guesses. */
#ifndef MCW_H
#define MCW_H
#include "types.h"

typedef struct MCW {
    s32 astep;      /* 0x00 step inside the current mc_act_* machine */
    s32 step;       /* 0x04 step inside the low level mc_* function */
    s32 slot;       /* 0x08 file index being saved (mc_act_save) */
    s32 cnt;        /* 0x0C result count of mc_get_dir */
    s32 nports;     /* 0x10 number of ports (2) */
    s32 port;       /* 0x14 */
    s32 cmd;        /* 0x18 sceMcSync command result */
    s32 res;        /* 0x1C sceMcSync result */
    s32 retry;      /* 0x20 */
    s32 ret;        /* 0x24 result of the act: -1 busy, 0 ok, <0 error code */
    s32 type;       /* 0x28 sceMcGetInfo: card type */
    s32 free;       /* 0x2C free blocks (per port at 0x40) */
    s32 fmt;        /* 0x30 */
    s32 state[3];   /* 0x34 */
    s32 info[3];    /* 0x40 */
    s32 changed;    /* 0x4C bit mask of ports with a changed card */
    char name[0x40];/* 0x50 */
    s32 fd;         /* 0x90 */
    u8 *buf;        /* 0x94 */
    s32 len;        /* 0x98 */
    s32 x9C;        /* 0x9C */
    s32 xA0;        /* 0xA0 */
    s32 act;        /* 0xA4 which mc_act_* machine runs (index of mc_act_jmp) */
    s32 file;       /* 0xA8 index into mc_file_tbl (0 game, 2 net) */
    s32 xAC;        /* 0xAC */
    s32 xB0;        /* 0xB0 */
} MCW;

/* one file of a save directory, 0x10 bytes, at MCFILE.f[i] (overlaps the header) */
typedef struct MCF {
    s32 _pad0[2];
    s32 on;         /* +0x8 file present */
    char *name;     /* +0xC */
    u8 *data;       /* +0x10 */
    s32 size;       /* +0x14 */
} MCF;

typedef struct MCFILE {
    char *title;    /* 0x00 */
    char *title2;   /* 0x04 */
    s32 _pad8[1];
    char *dir;      /* 0x0C directory name */
    u8 *x10;        /* 0x10 */
    s32 dsize;      /* 0x14 */
    s32 _pad18[1];
    char *l1;       /* 0x1C */
    s32 _pad20[3];
    char *l2;       /* 0x2C */
    s32 _pad30[3];
    char *l3;       /* 0x3C */
    s32 _pad40[2];
    s32 icon_on;    /* 0x48 */
    s32 _pad4C;
    u8 *icon;       /* 0x50 */
    s32 _pad54;
    s32 blocks;     /* 0x58 blocks needed */
} MCFILE;

extern MCW MemcardWork;
extern MCFILE *mc_file_tbl[];
#endif
