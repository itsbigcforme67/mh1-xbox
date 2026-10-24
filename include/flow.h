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

/* game_w, raw access by offset (MH1 offsets; in G GWX maps the ones G moved, see GAME_W in game.h) */
#define GW8(o)   FLD8(game_w, GWX(o))
#define GWS8(o)  FLDS8(game_w, GWX(o))
#define GW16(o)  FLD16(game_w, GWX(o))
#define GWS16(o) FLDS16(game_w, GWX(o))
#define GW32(o)  FLD32(game_w, GWX(o))

typedef struct SYSTEM_W { u8 _pad00[0x32]; u8 x32; u8 x33; u8 _pad34; u8 x35; u8 _pad36[9]; u8 x3F; s16 x40; /* 0x35 set while loading */ } SYSTEM_W;
typedef struct OPTION_W { u8 _pad00[3]; s8 x03; /* vibration off */ s8 x04; u8 _pad05[2]; s8 x07; u8 _pad08[0x18]; } OPTION_W;
extern SYSTEM_W system_w;
extern OPTION_W option_w;
/* select_w: character select results (only what game11 and game3/4 read) */
typedef struct SELECT_W {
    u8 _pad00[0x0A];
    s16 x0A;            /* 0x0A stage number (Quest_start) */
    u8 x0C[4];
#ifdef VERSION_MHG
    /* G select_w is 0xF0 bytes (0x35EC30) with 10-entry per-player arrays: the full map is include/swx.h (SWX) */
    u8 _pad10[0x66 - 0x10];
    u8 x54[4];          /* 0x66 (MH1 0x54) */
    u8 _pad6A[0x70 - 0x6A];
    V3S x5C[4];         /* 0x70 (MH1 0x5C) */
    u8 _pad88[0xAC - 0x88];
    s8 ready[4];        /* 0xAC per player ready flag (MH1 0x8C) */
    u8 _padB0[0xB6 - 0xB0];
    s8 x94[4];          /* 0xB6 (MH1 0x94) */
    u8 _padBA[0xD0 - 0xBA];
    u16 xAC;            /* 0xD0 selected quest number (MH1 0xAC) */
    u8 _padD2[0xF0 - 0xD2];
#else
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
#endif
} SELECT_W;
extern SELECT_W select_w;
struct STG_MDLS;
typedef struct STGW {
    u8 x00;             /* 0x00 stage drawn (trans_stage) */
    u8 x01;             /* 0x01 set to 1 by game13/game2 */
    u8 stage;           /* 0x02 stage number (st_model_load) */
    u8 _pad03;
    u8 step;            /* 0x04 0 = init (stage_i), 1 = run (stage_m) */
    u8 _pad05[3];
    s16 x08;            /* 0x08 frame counter */
    u8 _pad0A[0x10 - 0x0A];
    f32 pos[3];         /* 0x10 stage model position (trans_stage) */
    f32 x1C;            /* 0x1C UV scroll u of the last scrolled layer */
    f32 x20;            /* 0x20 UV scroll v */
    u8 _pad24[0x28 - 0x24];
    f32 rot[3];         /* 0x28 stage model rotation */
    s16 x34;            /* 0x34 set to 1 by st_model_load */
    u8 _pad36[2];
    s16 x38;            /* 0x38 start mdlw index (st_model_load) */
    u8 _pad3A[2];
    struct STG_MDLS *mdls; /* 0x3C stage model set (flag byte at +0, count s16 at +0x2C, CLAY array at +0x30) */
    u8 _pad40[0x48 - 0x40];
    u8 *x48;            /* 0x48 stage data: f32 floor height at +0x1C (f_sphr GetGroundHitArea outside the hit grid) */
    u8 _pad4C[0x4D - 0x4C];
    u8 x4D;             /* 0x4D G: version of the stage file's environment block (func_0010FD10; 0 none, 1 fog + lights, 2 + sun) */
    u8 _pad4E[0x64 - 0x4E];
#ifdef VERSION_MHG      /* G's stage_work is 0x100 bytes */
    u8 _pad64[0xC8 - 0x64];
    f32 sun_pos[3];     /* 0xC8 G: sun position (stage_set_set from sun_pos_tbl; stage_spr_disp) */
    s16 sun_tb[2];      /* 0xD4 G: the screen gradient's top / bottom line */
    u32 sun_col[8];     /* 0xD8 G: the gradient's colours 0xAARRGGBB (4 top, 4 bottom: by the view's angle to the sun) */
    u8 _padF8[0x100 - 0xF8];
#endif
} STGW;
extern STGW stage_work;
extern u8 Plsel_task[];
#endif
