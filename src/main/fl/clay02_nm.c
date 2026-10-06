/* fl clay near-match (not linked): flPS2GetMLCLAY (0x0016B110), 21/28 off (orig keeps the buffer pointer in s0 advanced by 0x30 and a copy in s1). */
#include "types.h"

typedef struct CLAYS {
    u8 x00[0x14];
    s32 x14;
} CLAYS;

void flMemcpy(void *, void *, u32);
u8 *flPS2GetSystemBuffAdrs(int);

typedef struct MLCLAY {
    u8 x00[8];
    s32 x08;
    u8 x0C[4];
    s32 x10;
} MLCLAY;

void flPS2GetMLCLAY(CLAYS *c, MLCLAY *out) {
    u8 *q;
    u8 *h;

    q = flPS2GetSystemBuffAdrs(c->x14);
    h = q;
    h += 0x30;
    flMemcpy(out, q, 0x24);
    out->x08 = (s32)h;
    h += (*(s32 *)(q + 0x1C) + 0xF) & ~0xF;
    out->x10 = (s32)h;
}
