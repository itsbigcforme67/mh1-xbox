/* fl clay near-match (not linked): flPS2GetMLCLAY (0x0016B110), 21/28 off (orig keeps the buffer pointer in s0 advanced by 0x30 and a copy in s1). */
#include "types.h"

typedef struct CLAYS {
    u8 x00[0x14];
    s32 x14;
} CLAYS;

void flMemcpy(void *, void *, int);
u8 *flPS2GetSystemBuffAdrs(int);

typedef struct MLCLAY {
    u8 x00[8];
    s32 x08;
    u8 x0C[4];
    s32 x10;
} MLCLAY;

void flPS2GetMLCLAY(CLAYS *c, MLCLAY *out) {
    u8 *h;
    u8 *q;

    q = h = flPS2GetSystemBuffAdrs(c->x14);
    flMemcpy(out, h, 0x24);
    h += 0x30;
    out->x08 = (s32)h;
    out->x10 = (s32)h + ((*(s32 *)(q + 0x1C) + 0xF) & ~0xF);
}
