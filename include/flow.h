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
    u8 _pad00[0x0C];
    u8 x0C[4];
    u8 _pad10[0x54 - 0x10];
    u8 x54[4];
    u8 _pad58[0x5C - 0x58];
    V3S x5C[4];
    u8 _pad74[0x8C - 0x74];
    s8 ready[4];        /* 0x8C per player ready flag (game3/game4) */
    u8 _pad90[0x94 - 0x90];
    s8 x94[4];
} SELECT_W;
extern SELECT_W select_w;
extern u8 stage_work[];  /* +1 set by game13 */
extern u8 Plsel_task[];
#endif
