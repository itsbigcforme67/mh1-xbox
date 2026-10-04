#ifndef SET_H
#define SET_H
/* Set objects ("set" = placed props/effects in a stage), e.g. set01..set22.
 * Work entries come from pull_set_work() and run a small state machine:
 * mode 0 init (_i), 1 move (_m), 2 die (_d), 3 end (_e).
 * Field names are ours; offsets are from matched code (see each comment). */
#include "types.h"

typedef struct SETW {
    u8 pad0;            /* 0x00 */
    u8 be_flag;         /* 0x01 active (set12_i sets 1, set12_d clears) */
    u8 type;            /* 0x02 set number (12 for set12) */
    u8 arg;             /* 0x03 */
    u8 mode;            /* 0x04 state machine index */
    u8 mode2;           /* 0x05 sub-state (set01 banner: in/hold/out) */
    u8 se0;             /* 0x06 sound args passed to se_req2 */
    u8 se1;             /* 0x07 */
    s16 timer;          /* 0x08 counts down; -1 = forever */
    s16 cnt;            /* 0x0A */
    u8 padC[0x14 - 0x0C];
    s32 work14;         /* 0x14 */
    union {
        char **str;     /* 0x18 set01: points at the message string slot */
        void *work;     /* 0x18 per-object work area (set07) */
    } u;
    u8 pad1C[0x20 - 0x1C];
    void (*move)(struct SETW *);  /* 0x20 */
    f32 pos[3];         /* 0x24 */
    f32 speed;          /* 0x30 set01 banner slide speed */
    u8 pad34[0x38 - 0x34];
    struct PRIM *prim;  /* 0x38 */
    s16 prim_no;        /* 0x3C */
    u8 flag3E;          /* 0x3E */
} SETW;

SETW *pull_set_work(int);
void push_set_work(SETW *);
void se_req2(int, int, int, f32 *, int, int);
#endif
