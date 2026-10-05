#ifndef UKI_H
#define UKI_H
/* The fishing float's work (eft22, ew->work). Offsets from matched
 * eft22 code; eft23 (the fish) reads it through uki_serch. */
#include "types.h"

typedef struct UKI {
    s16 cast;           /* 0x00 cast kind, picks the throw speed (end_init) */
    s16 timer;          /* 0x02 */
    f32 vel[3];         /* 0x04 */
    f32 pos[3];         /* 0x10 */
    f32 suimen;         /* 0x1C water height of this stage */
    f32 grav;           /* 0x20 */
    u16 ang;            /* 0x24 */
    u16 ang2;           /* 0x26 */
    s16 x28;            /* 0x28 */
    s16 slow;           /* 0x2A bait 0x60/0x62/0x63: the float drifts gently */
    u8 line_on;         /* 0x2C line is drawn (twitches while waiting) */
    u8 line_cnt;        /* 0x2D */
} UKI;

#endif
