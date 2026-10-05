#ifndef EFT_H
#define EFT_H
/* Effects (eft00..eft24): work from pull_eft_work(), same mode machine as
 * set objects (i/m/d/e). Offsets from matched code (eft09.c). */
#include "types.h"
#include "em.h"

struct PRIM;

typedef struct EFTW {
    u8 pad0;            /* 0x00 */
    u8 be_flag;         /* 0x01 */
    u8 type;            /* 0x02 effect number */
    u8 arg;             /* 0x03 */
    u8 mode;            /* 0x04 */
    u8 mode2;           /* 0x05 */
    u8 stg;             /* 0x06 */
    u8 x07;             /* 0x07 */
    s16 timer;          /* 0x08 */
    u16 ang;            /* 0x0A */
    u8 _pad0C[0x14 - 0x0C];
    s32 work14;         /* 0x14 */
    u8 _pad18[0x20 - 0x18];
    void (*move)(struct EFTW *);    /* 0x20 */
    f32 pos[3];         /* 0x24 */
    u8 _pad30[4];
    EMW *owner;         /* 0x34 */
    struct PRIM *prim;  /* 0x38 */
    s16 prim_no;        /* 0x3C */
    u8 prim2;           /* 0x3E 1: prim from the second pool (get_prim2) */
} EFTW;

EFTW *pull_eft_work(int);
void push_eft_work(EFTW *);
#endif
