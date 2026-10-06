/* fl clay: flPS2GetMLCLAY (SLPM_654.95 0x0016B110-0x0016B180): copies the 0x24 byte header of a clay handle's system buffer into out and
 * points out->x08 / out->x10 at the two data blocks that follow (each padded to 16 bytes). */
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

    h = flPS2GetSystemBuffAdrs(c->x14);
    q = h;
    h += 0x30;
    flMemcpy(out, q, 0x24);
    out->x08 = (s32)h;
    h += (*(s32 *)(q + 0x1C) + 0xF) & ~0xF;
    out->x10 = (s32)h;
}
