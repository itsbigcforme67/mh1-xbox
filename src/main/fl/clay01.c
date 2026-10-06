/* fl clay handle management. SLPM_654.95 0x0016AEC0-0x0016B180 (flCreateClayHandle, flPS2CreateClay, flPS2GetMLCLAY) and flReleaseClayHandle (0x0016D8F0).
 * A clay (model) handle is a 0x180 byte record in flPS2Clay[0x180]; the handle number is index + 1. */
#include "types.h"

typedef struct CLAYH {
    s32 flags;          /* 0x000 create flags (bit 2: use the caller's data without copying) */
    s32 used;           /* 0x004 non-zero while the slot is in use (file size of the clay) */
    s32 x08;            /* 0x008 */
    s32 rs;             /* 0x00C render state */
    s32 size;           /* 0x010 size of the converted data */
    s32 mem;            /* 0x014 system memory handle of the copy */
    s32 x18;
    s32 mem2;           /* 0x01C */
    s32 buf;            /* 0x020 */
    s32 x24;
    u8 x28[0x48 - 0x28];
    s32 shader;         /* 0x048 */
    s32 x4C;
    s32 x50, x54;
    u8 x58[0x180 - 0x58];
} CLAYH;

extern CLAYH flPS2Clay[0x180];
extern s32 flSystemRenderState;
extern s32 flClayNum;

void flMemset(void *, int, int);
void flMemcpy(void *, void *, u32);
int flPS2CreateClay(void *, void *);
int flPS2GetSystemMemoryHandle(int, int);
void *flPS2GetSystemBuffAdrs(int);
void flPS2ReleaseSystemMemory(int);
void flPS2DmaTerminate(int);
int flReleaseClayHandle(u32);

typedef struct CLAYSRC {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
    s32 x18;
    s32 x1C;
} CLAYSRC;

int flCreateClayHandle(CLAYSRC *src, int flags) {
    u32 i;
    CLAYH *c;
    u8 *a;
    u8 *q;

    for (i = 0; i < 0x180; i++) {
        if (flPS2Clay[i].used == 0) {
            break;
        }
    }
    c = &flPS2Clay[i];
    flMemset(c, 0, 0x180);
    c->used = src->x0;
    c->x08 = src->xC;
    c->flags = flags;
    c->x50 = 0;
    c->x54 = 0;
    if (!(flags & 4)) {
        c->size = 0x30;
        c->size += (src->x1C + 0xF) & ~0xF;
        c->size += (src->x18 + 0xF) & ~0xF;
        c->mem = flPS2GetSystemMemoryHandle(c->size, 3);
        a = flPS2GetSystemBuffAdrs(c->mem);
        flMemcpy(a, src, 0x24);
        q = a + 0x30;
        flMemcpy(q, (void *)src->x8, src->x1C);
        q += (src->x1C + 0xF) & ~0xF;
        flMemcpy(q, (void *)src->x10, src->x18);
    }
    c->rs = flSystemRenderState;
    if (flPS2CreateClay(c, src) == 0) {
        flReleaseClayHandle(i + 1);
        return 0;
    }
    return i + 1;
}
