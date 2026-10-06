/* fl clay: flReleaseClayHandle (SLPM_654.95 0x0016D8F0): frees a clay handle (1-based index into flPS2Clay[0x180]) and its memory. */
#include "types.h"

typedef struct CLAYR {
    s32 x00;
    s32 used;           /* 0x004 */
    u8 x08[0x14 - 0x08];
    s32 mem;            /* 0x014 */
    s32 x18;
    s32 mem2;           /* 0x01C */
    u8 x20[0x180 - 0x20];
} CLAYR;

extern CLAYR flPS2Clay[0x180];
extern s32 flClayNum;

void flMemset(void *, int, int);
void flPS2DmaTerminate(int);
void flPS2ReleaseSystemMemory(int);

int flReleaseClayHandle(u32 h) {
    CLAYR *c;
    int i;

    i = h - 1;
    c = &flPS2Clay[i];
    if (h == 0) {
        return 0;
    }
    if (h > 0x180) {
        return 0;
    }
    if (c->used == 0) {
        return 0;
    }
    flPS2DmaTerminate(h);
    c->used = 0;
    if (c->mem != 0) {
        flPS2ReleaseSystemMemory(c->mem);
    }
    if (c->mem2 != 0) {
        flPS2ReleaseSystemMemory(c->mem2);
    }
    flMemset(c, 0, 0x180);
    flClayNum--;
    return 1;
}
