#ifndef PRIM_H
#define PRIM_H
/* Draw primitives: get_prim() hands out a slot number, get_prim_ptr() its
 * work; add_prim() queues it on an ordering table each frame and the
 * renderer calls trans(). Offsets from matched code (set06.c). */
#include "types.h"

typedef struct PRIM {
    u8 _pad00[0x08];
    f32 pos[3];                     /* 0x08 */
    void (*trans)(struct PRIM *);   /* 0x14 draw callback */
    void *owner;                    /* 0x18 */
    s32 no;                         /* 0x1C index within the owner (set07) */
} PRIM;

int get_prim(void);
PRIM *get_prim_ptr(s16);
void add_prim(void *ot, PRIM *, int, int);
extern u8 ot0[];
extern u8 ot1[];

#endif
