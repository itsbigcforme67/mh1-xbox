#ifndef FLOW_H
#define FLOW_H
/* Helpers for the core game flow files (src/main/game, stage, quest).
 * Globals whose layout is not known yet are accessed by byte offset through
 * these macros (the compiler output is identical to a struct field access). */
#include "types.h"
#include "game.h"
#include "pl.h"

#define FLD8(base, o)   (*(u8 *)((u8 *)&(base) + (o)))
#define FLDS8(base, o)  (*(s8 *)((u8 *)&(base) + (o)))
#define FLD16(base, o)  (*(u16 *)((u8 *)&(base) + (o)))
#define FLDS16(base, o) (*(s16 *)((u8 *)&(base) + (o)))
#define FLD32(base, o)  (*(s32 *)((u8 *)&(base) + (o)))
#define FLDF(base, o)   (*(f32 *)((u8 *)&(base) + (o)))

/* game_w, raw access by offset */
#define GW8(o)   FLD8(game_w, o)
#define GWS8(o)  FLDS8(game_w, o)
#define GW16(o)  FLD16(game_w, o)
#define GWS16(o) FLDS16(game_w, o)
#define GW32(o)  FLD32(game_w, o)

typedef struct SYSTEM_W { u8 _pad00[0x32]; u8 x32; u8 x33; u8 _pad34; u8 x35; u8 _pad36[9]; u8 x3F; s16 x40; /* 0x35 set while loading */ } SYSTEM_W;
typedef struct OPTION_W { u8 _pad00[4]; s8 x04; u8 _pad05[2]; s8 x07; u8 _pad08[0x18]; } OPTION_W;
extern SYSTEM_W system_w;
extern OPTION_W option_w;
/* select_w: character select results (only what game11 and game3/4 read) */
typedef struct SELECT_W {
    u8 _pad00[0x0A];
    s16 x0A;            /* 0x0A stage number (Quest_start) */
    u8 x0C[4];
    u8 _pad10[0x54 - 0x10];
    u8 x54[4];
    u8 _pad58[0x5C - 0x58];
    V3S x5C[4];
    u8 _pad74[0x8C - 0x74];
    s8 ready[4];        /* 0x8C per player ready flag (game3/game4) */
    u8 _pad90[0x94 - 0x90];
    s8 x94[4];
    u8 _pad98[0xAC - 0x98];
    u16 xAC;            /* 0xAC selected quest number (Quest_start) */
} SELECT_W;
extern SELECT_W select_w;
struct STG_MDLS;
typedef struct STGW {
    u8 x00;             /* 0x00 stage drawn (trans_stage) */
    u8 x01;             /* 0x01 set to 1 by game13/game2 */
    u8 _pad02[2];
    u8 step;            /* 0x04 0 = init (stage_i), 1 = run (stage_m) */
    u8 _pad05[3];
    s16 x08;            /* 0x08 frame counter */
    u8 _pad0A[0x10 - 0x0A];
    f32 pos[3];         /* 0x10 stage model position (trans_stage) */
    f32 x1C;            /* 0x1C UV scroll u of the last scrolled layer */
    f32 x20;            /* 0x20 UV scroll v */
    u8 _pad24[0x28 - 0x24];
    f32 rot[3];         /* 0x28 stage model rotation */
    u8 _pad34[0x3C - 0x34];
    struct STG_MDLS *mdls; /* 0x3C stage model set (flag byte at +0, count s16 at +0x2C, CLAY array at +0x30) */
    u8 _pad40[0x64 - 0x40];
} STGW;
extern STGW stage_work;
extern u8 Plsel_task[];
#endif
