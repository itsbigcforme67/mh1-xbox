#ifndef QUEST_H
#define QUEST_H
/* Quest work and mission data (f_quest*.c). Layouts come from the offsets
 * used by the matched code; names and meanings are guesses. */
#include "f_game.h"
#include "em.h"

#ifndef VERSION_MH2
/* A (value, flag) pair, 4 bytes (quest_w+0x98 and +0xBC lists). */
typedef struct QPAIR {
    s16 v;
    s8 f;
    u8 _pad3;
} QPAIR;
#endif

/* Mission data header in mission_area: offsets of the tables (loaded from the
 * quest file). */
typedef struct MISSION {
    s32 o[14];
} MISSION;

/* Info block (quest_w.x94 points at it): time, fees. */
#ifdef VERSION_MH2
/* MH2's info block (the .mib's first table): fields keep G's names, at MH2's places, each read from MH2's own code:
 * sub_main Quest_start 0x3A11A0 (+0x00 lw, +0x0C, +0x10, +0x24), Quest_time_get 0x3A1FC0 (+0x1C), Quest_pl_stage_init
 * 0x3A1D00 (+0x20, the start-area list). 0x04..0x0B and 0x14..0x1B: not seen used by name. */
typedef struct MISSION2 {
    s32 x00;            /* 0x00 flags (quest_w.x40; a word in MH2, G's u16) */
    u8 _pad04[8];
    s32 x08;            /* 0x0C reward money (G 0x08) */
    s32 x0C;            /* 0x10 fee (G 0x0C) */
    u8 _pad14[0x1C - 0x14];
    s32 x10;            /* 0x1C time limit (G 0x10) */
    s32 x14;            /* 0x20 start-area list (G 0x14) */
    s32 x18;            /* 0x24 table offset (G 0x18) */
} MISSION2;
#else
typedef struct MISSION2 {
    u16 x00;            /* 0x00 */
    u8 _pad02[6];
    s32 x08;            /* 0x08 reward money */
    s32 x0C;            /* 0x0C fee */
    s32 x10;            /* 0x10 time limit */
    s32 x14;            /* 0x14 */
    s32 x18;            /* 0x18 */
} MISSION2;
#endif

/* One command of the quest condition program (quest_w.x6C): 8 bytes. */
typedef struct QCMD {
    s16 cmd;            /* 0x00 */
    s16 a;              /* 0x02 */
    s16 b;              /* 0x04 */
    s16 c;              /* 0x06 */
} QCMD;

#ifdef VERSION_MH2
#include "mh2_layouts.h"    /* MH2 moved most of quest_w */
#else
typedef struct QUEST_W {
    s8 x00;             /* 0x00 condition type */
    s8 x01;             /* 0x01 step */
    u8 _pad02;
    s8 x03;             /* 0x03 */
    s8 x04;             /* 0x04 */
    u8 _pad05;
    s8 x06;             /* 0x06 state: 4 clear, 6, 7 retire, 8 error */
    u8 _pad07;
#ifdef VERSION_MHG
    u16 no;             /* 0x08 quest number (0: free hunt); u16 in G (lhu in Stage_item_data_get) */
#else
    s16 no;             /* 0x08 quest number (0: free hunt) */
#endif
    s8 x0A;             /* 0x0A section number (quest_condition_prog case 0x1B) */
    s8 x0B;             /* 0x0B */
    s32 x0C;            /* 0x0C wait counter (condition program cmd 5/6) */
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
    s16 x144[2];        /* 0x144 copy of x2C (quest_enemy_set) */
    s16 x148[2];        /* 0x148 copy of x30 */
    s16 x14C;           /* 0x14C */
    s8 x14E;            /* 0x14E */
    s8 x14F;            /* 0x14F */
#ifdef VERSION_MHG
    /* G (0x290 bytes, Quest_init's memset): MH1's x150 and x180.. block moved; offsets in the
     * comments are G's (sub_main Quest_start 0x4B3AA0, Quest_condition_judging) */
    u8 _pad150[0x154 - 0x150];
    s8 x150;            /* 0x154 (MH1 0x150; G clears 0x154 where MH1 cleared 0x150: a guess) */
    u8 rnd[15];         /* 0x155 random bytes per quest (offline ran_suu, online from the host);
                         * seed of the quest's own generator (Quest_rand_seed); [master] picks the start area */
    u8 _pad164;
    s8 x165[3];         /* 0x165 mission header bytes 0x45..0x47 */
    u8 x168;            /* 0x168 cleared by Quest_start */
    s8 x169;            /* 0x169 (time limit - 1) / 9000, not capped (xAF is capped at 2) */
    u16 rnd_seed;       /* 0x16A the quest's generator state (Quest_rand) */
    u16 x16C[52];       /* 0x16C (u16: lhu in G's Quest_enemy_die) cleared by Quest_start */
    u16 x1D4[52];       /* 0x1D4 (u16: lhu in G's Quest_enemy_die) cleared by Quest_start */
    s16 x23C;           /* 0x23C cleared by Quest_start */
    s16 x23E;           /* 0x23E */
    s16 x240;           /* 0x240 */
    u8 _pad242[0x280 - 0x242];
    s8 x180;            /* 0x280 (MH1 0x180; Quest_condition_judging) */
    s8 x181;            /* 0x281 */
    s16 x182;           /* 0x282 */
    s16 x184;           /* 0x284 */
    s16 x186;           /* 0x286 */
    u8 _pad288[0x290 - 0x288];
#else
    s8 x150;            /* 0x150 */
    u8 _pad151[0x180 - 0x151];
    s8 x180;            /* 0x180 */
    s8 x181;            /* 0x181 */
    s16 x182;           /* 0x182 */
    s16 x184;           /* 0x184 */
    s16 x186;           /* 0x186 */
#endif
} QUEST_W;
#endif
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
#ifdef VERSION_MHG
    s16 x30;            /* 0x30 G: size in percent (quest_em_init_sub: 100, or random for big monsters) */
    u8 _pad32[0xA];
#else
    u8 _pad30[0xC];     /* 0x30 (entry size is 0x3C) */
#endif
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
