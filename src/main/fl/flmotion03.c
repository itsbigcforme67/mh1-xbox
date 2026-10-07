/* fl library motion set handles and pose blend (SLPM_654.95 0x00173A50-0x00173E54, one source file): flMotionSetBaseAddress (file-static in the
 * original: remembers the system buffer address of the current motion handle for the f-curve code), copies of a motion set / init motion set
 * into system memory (Create*) and their releases, flCalcTransVelocity (translation difference of two samples of one motion) and flBlendMotionEx
 * (finds the group roots of two node trees and blends them with the file-static flBlendMotionExSub). The static callee takes (a, b, w0, id, w1)
 * and flGetMotionMatrix is called with t third: those parameter orders give the original argument set-up order. Names are guesses. */
#include "types.h"

extern int base_addr_0038A25C;

int flPS2GetSystemBuffAdrs();
int flPS2GetSystemMemoryHandle();
void flPS2ReleaseSystemMemory();
void flFCVSetBaseAddress();

void flMotionSetBaseAddress(int h) {
    base_addr_0038A25C = flPS2GetSystemBuffAdrs(h);
    flFCVSetBaseAddress(base_addr_0038A25C);
}

int flCreateInitMotionSetHandle(u8 *set) {
    u8 *src;
    u8 *dst;
    int h;
    u32 i;

    src = *(u8 **)(set + 8);
    h = flPS2GetSystemMemoryHandle(*(u32 *)(src + 0xC), 4);
    dst = (u8 *)flPS2GetSystemBuffAdrs(h);
    {
        u8 *s = src;

        for (i = 0; i < *(u32 *)(src + 0xC); i++) {
            *dst++ = *s++;
        }
    }
    return h;
}

int flCreateMotionSetHandle(u8 *set) {
    u8 *src;
    u32 *dst;
    u32 i;
    int h;

    src = *(u8 **)(set + 0x14);
    h = flPS2GetSystemMemoryHandle(*(u32 *)(src + 0x18), 4);
    dst = (u32 *)flPS2GetSystemBuffAdrs(h);
    {
        u32 *s = (u32 *)src;

        for (i = 0; i < (*(u32 *)(src + 0x18) >> 2); i++) {
            *dst++ = *s++;
        }
    }
    return h;
}

int flReleaseInitMotionSetHandle(int h) {
    flPS2ReleaseSystemMemory(h);
    return 1;
}

int flReleaseMotionSetHandle(int h) {
    flPS2ReleaseSystemMemory(h);
    return 1;
}

int flGetMotionMatrix(int, f32 *, f32, f32 *, f32 *, s16 *);

int flCalcTransVelocity(f32 *out, u8 *n, f32 t0, f32 t1) {
    f32 m1[16];
    f32 m2[16];
    f32 v[12];
    int p;

    flMotionSetBaseAddress(*(int *)(n + 0xD4));
    p = *(int *)(n + 0xD8) + base_addr_0038A25C;
    flGetMotionMatrix(p, (f32 *)(n + 0xDC), t0, v, m1, (s16 *)(n + 0x15C));
    flGetMotionMatrix(p, (f32 *)(n + 0xDC), t1, v, m2, (s16 *)(n + 0x15C));
    out[0] = m2[12] - m1[12];
    out[1] = m2[13] - m1[13];
    out[2] = m2[14] - m1[14];
    return 1;
}

typedef struct BNODE BNODE;
struct BNODE {
    u8 x0[0x40];
    u8 mat[0x40];       /* 0x40 */
    u8 x80[0xC6 - 0x80];
    u16 grp;            /* 0xC6 group id */
    BNODE *parent;      /* 0xC8 */
    BNODE *sib;         /* 0xCC */
    BNODE *child;       /* 0xD0 */
    u8 xD4[0x104 - 0xD4];
    f32 cur[3];         /* 0x104 translation of the current values */
};

void flmatBlend(void *, void *, void *, f32, f32);
BNODE *flFindGroupRoot(BNODE *, int);

static int flBlendMotionExSub(BNODE *a, BNODE *b, f32 w0, int id, f32 w1);

int flBlendMotionEx(f32 w0, f32 w1, BNODE *a, BNODE *b, int id) {
    BNODE *ra;

    ra = flFindGroupRoot(a, id);
    flBlendMotionExSub(ra, flFindGroupRoot(b, id), w0, id, w1);
    return 1;
}

static int flBlendMotionExSub(BNODE *a, BNODE *b, f32 w0, int id, f32 w1) {
    BNODE *top;
    u16 g;

    top = a;
    g = id;
loop:
    if (a->grp == g) {
        a->cur[0] = a->cur[0] * w0 + b->cur[0] * w1;
        a->cur[1] = a->cur[1] * w0 + b->cur[1] * w1;
        a->cur[2] = a->cur[2] * w0 + b->cur[2] * w1;
        flmatBlend(a->mat, a->mat, b->mat, w0, w1);
        if (a->child != 0) {
            a = a->child;
            b = b->child;
            goto loop;
        }
    }
    if (a->sib != 0 && a != top) {
        a = a->sib;
        b = b->sib;
        goto loop;
    }
    while (a != top) {
        while (a->sib != 0) {
            a = a->sib;
            b = b->sib;
            if (a->grp == g) {
                goto loop;
            }
        }
        a = a->parent;
        b = b->parent;
    }
    return 1;
}
