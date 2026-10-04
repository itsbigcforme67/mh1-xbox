#ifndef PL_H
#define PL_H
/* Player work: player_work[], 0xA00 bytes per player.
 * Only fields seen in matched code are named; names ending in an offset
 * (work2F4) are placeholders until their meaning is known. Every offset
 * here comes from code that byte-matches (see the file noted per group).
 * First filled from the pl file at 0x14F030 (src/main/pl/pl_normal.c). */
#include "types.h"

/* Pad/switch state at PLW+0x364, written by sw_set_sub. */
typedef struct PLSW {
    u16 now;            /* 0x00 */
    u16 old;            /* 0x02 */
    u16 trg;            /* 0x04 */
    u16 trg_old;        /* 0x06 */
    u16 pad08[4];       /* 0x08 */
    s16 chg;            /* 0x10 */
    u16 pad12;          /* 0x12 */
    u16 an_now;         /* 0x14 */
    u16 an_old;         /* 0x16 */
    u16 an_trg;         /* 0x18 */
    u16 an_trg_old;     /* 0x1A */
    u16 ang[2];         /* 0x1C */
    u16 pow[2];         /* 0x20 */
} PLSW;

typedef struct PLW {
    u8 _pad000[0x4];
    s32   work04;        /* 0x004 */
    s32   work08;        /* 0x008 */
    u16   id;            /* 0x00C */
    u8 _pad00E[0x4];
    u8    flag12;        /* 0x012 */
    u8 _pad013[0x1];
    s8    flag14;        /* 0x014 */
    s8    flag15;        /* 0x015 */
    u8 _pad016[0x62];
    u16   shell_flag;    /* 0x078 */
    u8 _pad07A[0x11E];
    s32   chr_no0;       /* 0x198 */
    u8 _pad19C[0x4];
    f32   chr_spd0;      /* 0x1A0 */
    u8 _pad1A4[0x44];
    s32   chr_no1;       /* 0x1E8 */
    u8 _pad1EC[0x4];
    f32   chr_spd1;      /* 0x1F0 */
    u8 _pad1F4[0xE8];
    u16   char0;         /* 0x2DC */
    u16   char1;         /* 0x2DE */
    u8 _pad2E0[0x4];
    s16   act_tm0;       /* 0x2E4 */
    s16   act_tm1;       /* 0x2E6 */
    u8 _pad2E8[0x4];
    s16   blend0;        /* 0x2EC */
    s16   blend1;        /* 0x2EE */
    u8 _pad2F0[0x4];
    s16   work2F4;       /* 0x2F4 */
    u8 _pad2F6[0x2];
    s8    work2F8;       /* 0x2F8 */
    u8 _pad2F9[0x3];
    s16   work2FC;       /* 0x2FC */
    u8 _pad2FE[0x66];
    PLSW  sw;            /* 0x364 */
    u8    st;            /* 0x388 */
    u8 _pad389[0x1];
    s8    work38A;       /* 0x38A */
    u8 _pad38B[0x5];
    s32   act_flag;      /* 0x390 */
    s32   work394;       /* 0x394 */
    s16   work398;       /* 0x398 */
    u8 _pad39A[0x2];
    s32   work39C;       /* 0x39C */
    u8 _pad3A0[0x14];
    s32   work3B4[6];    /* 0x3B4 */
    u8 _pad3CC[0x4];
    s8    work3D0;       /* 0x3D0 */
    s8    work3D1;       /* 0x3D1 */
    u8 _pad3D2[0x22];
    s8    work3F4;       /* 0x3F4 */
    u8 _pad3F5[0x17];
    s16   work40C;       /* 0x40C */
    u8 _pad40E[0x2E];
    s16   work43C;       /* 0x43C */
    u8 _pad43E[0x2];
    s8    work440;       /* 0x440 */
    u8 _pad441[0x95];
    s16   work4D6;       /* 0x4D6 */
    u8 _pad4D8[0x5];
    s8    work4DD;       /* 0x4DD */
    u8 _pad4DE[0x2];
    s16   work4E0;       /* 0x4E0 */
    u8 _pad4E2[0x1];
    s8    work4E3;       /* 0x4E3 */
    u8 _pad4E4[0x87];
    s8    work56B;       /* 0x56B */
    u8 _pad56C[0x98];
    u8    flag604;       /* 0x604 */
    u8 _pad605[0x10];
    s8    work615;       /* 0x615 */
    u8 _pad616[0x10A];
    s8    work720[4];    /* 0x720 */
    s16   work724[4];    /* 0x724 */
    s16   work72C[4];    /* 0x72C */
    u8 _pad734[0x2];
    u8    stg;           /* 0x736 */
    u8 _pad737[0x145];
    s16   work87C;       /* 0x87C */
    u8 _pad87E[0x4];
    s16   work882;       /* 0x882 */
    u8 _pad884[0x8];
    u8    work88C;       /* 0x88C */
    u8 _pad88D[0x35];
    u8    work8C2;       /* 0x8C2 */
    u8 _pad8C3[0x2D];
    s8    work8F0;       /* 0x8F0 */
    u8 _pad8F1[0x10F];
} PLW;

extern PLW player_work[];

#endif
