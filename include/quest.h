#ifndef QUEST_H
#define QUEST_H
/* Quest work and mission data (f_quest*.c). Layouts come from the offsets
 * used by the matched code; names and meanings are guesses. */
#include "f_game.h"
#include "em.h"

/* A (value, flag) pair, 4 bytes (quest_w+0x98 and +0xBC lists). */
typedef struct QPAIR {
    s16 v;
    s8 f;
    u8 _pad3;
} QPAIR;

/* Mission data header in mission_area: offsets of the tables (loaded from the
 * quest file). */
typedef struct MISSION {
    s32 o[14];
} MISSION;

/* Info block (quest_w.x94 points at it): time, fees. */
typedef struct MISSION2 {
    u16 x00;            /* 0x00 */
    u8 _pad02[6];
    s32 x08;            /* 0x08 reward money */
    s32 x0C;            /* 0x0C fee */
    s32 x10;            /* 0x10 time limit */
    s32 x14;            /* 0x14 */
    s32 x18;            /* 0x18 */
} MISSION2;

typedef struct QUEST_W {
    s8 x00;             /* 0x00 condition type */
    s8 x01;             /* 0x01 step */
    u8 _pad02;
    s8 x03;             /* 0x03 */
    s8 x04;             /* 0x04 */
    u8 _pad05;
    s8 x06;             /* 0x06 state: 4 clear, 6, 7 retire, 8 error */
    u8 _pad07;
    s16 no;             /* 0x08 quest number (0: free hunt) */
    u8 _pad0A;
    s8 x0B;             /* 0x0B */
    u8 _pad0C[4];
    s32 x10;            /* 0x10 time left */
    s32 x14;            /* 0x14 reward money */
    s32 x18;            /* 0x18 fee (subtracted from the reward) */
    s16 x1C[4];         /* 0x1C */
    s16 x24[4];         /* 0x24 */
    s16 x2C[2];         /* 0x2C */
    s16 x30[2];         /* 0x30 */
    s16 x34;            /* 0x34 monsters left */
    s16 x36;            /* 0x36 condition program counter, -1 done */
    s16 x38;            /* 0x38 */
    s8 x3A;             /* 0x3A */
    s8 x3B;             /* 0x3B number of pick points in stiem_stack_tbl */
    void *x3C;          /* 0x3C last monster (EMW) that counted */
    s32 x40;            /* 0x40 flags from the mission info (bit 1: ...) */
    u8 _pad44[0x64 - 0x44];
    MISSION *x64;       /* 0x64 mission data */
    u8 _pad68[4];
    s32 *x6C;           /* 0x6C */
    s32 *x70;           /* 0x70 */
    s32 *x74;           /* 0x74 */
    s32 *x78;           /* 0x78 */
    s32 *x7C;           /* 0x7C */
    s32 *x80;           /* 0x80 */
    s32 *x84;           /* 0x84 */
    s32 *x88;           /* 0x88 */
    s32 *x8C;           /* 0x8C */
    s32 *x90;           /* 0x90 */
    MISSION2 *x94;      /* 0x94 */
    QPAIR x98[5];       /* 0x98 */
    s8 xAC;             /* 0xAC */
    s8 xAD;             /* 0xAD */
    s8 xAE;             /* 0xAE */
    s8 xAF;             /* 0xAF */
    void *xB0;          /* 0xB0 */
    struct { s8 a, b; } xB4[4]; /* 0xB4 b = hunter rank per player */
    QPAIR xBC[32];      /* 0xBC */
    s32 x13C;           /* 0x13C */
    s32 x140;           /* 0x140 */
    s32 x144;           /* 0x144 */
    s32 x148;           /* 0x148 */
    s16 x14C;           /* 0x14C */
    s8 x14E;            /* 0x14E */
    s8 x14F;            /* 0x14F */
    s8 x150;            /* 0x150 */
    u8 _pad151[0x180 - 0x151];
    s8 x180;            /* 0x180 */
    s8 x181;            /* 0x181 */
    s16 x182;           /* 0x182 */
    s16 x184;           /* 0x184 */
    s16 x186;           /* 0x186 */
} QUEST_W;
extern QUEST_W quest_w;

/* One monster entry of the mission's enemy table (0x30 bytes). */
typedef struct QEM {
    s16 id;             /* 0x00 */
    s16 x02;            /* 0x02 */
    s8 x04;             /* 0x04 lives left */
    s8 x05;             /* 0x05 counts toward quest_w.x34 */
    s8 x06;             /* 0x06 */
    s8 x07;             /* 0x07 stage */
    s16 x08;            /* 0x08 */
    s16 x0A;            /* 0x0A */
    s32 x0C;            /* 0x0C */
    s32 x10;            /* 0x10 */
    s32 x14;            /* 0x14 */
    u8 _pad18[4];
    s32 x1C;            /* 0x1C */
    f32 pos[3];         /* 0x20 */
    s16 x2C;            /* 0x2C */
    s16 x2E;            /* 0x2E flags: 1 gone, 2, 4 dead, 8 captured */
} QEM;

/* One extra pick-up point (StiEM_data[20], 0x1C bytes): treasure/hagi spot
 * with a radius, an item id (bit 15: ...) and a remaining count. Guesses. */
typedef struct STIEM {
    f32 pos[3];         /* 0x00 */
    f32 rad;            /* 0x0C pick radius */
    u16 id;             /* 0x10 item id, 0xFFFF = free */
    u16 cnt;            /* 0x12 remaining picks (0xFF: endless) */
    s16 x14;            /* 0x14 */
    u8 _pad16[2];
    u8 stg;             /* 0x18 stage */
    u8 x19;             /* 0x19 flags */
    s16 x1A;            /* 0x1A */
} STIEM;
extern STIEM StiEM_data[20];
extern s8 stiem_stack_tbl[20];

extern u8 *mission_area;
#endif
