#ifndef PLST_H
#define PLST_H
/* Stage item / unique-object records used by the player pick-up code (pl62, pl63, St_pick_ck2). Layout from offsets. */
#include "types.h"
typedef struct ST_ITEM {
    f32 pos[3];     /* 0x00 */
    f32 r;          /* 0x0C pick-up radius */
    u16 id;         /* 0x10 */
    u16 num;        /* 0x12 */
    u16 x14;        /* 0x14 */
    u16 _pad16;
} ST_ITEM;
typedef struct ST_UNIQ {
    u16 x00;        /* 0x00 */
    u16 kind;       /* 0x02 */
    f32 pos[3];     /* 0x04 */
    f32 r;          /* 0x10 */
    u16 x14;        /* 0x14 */
    u16 _pad16;
} ST_UNIQ;
#endif
