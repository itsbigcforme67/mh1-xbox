/* NEAR-MATCH (not linked): flPS2GetClaySize 19 of 212 instructions differ (register allocation of the loop counter vs the flags/vertex-size temp). Original: SLPM_654.95 0x0016B180-0x0016B4D0.
 * triangles, works out the size of the converted vertex data (the buffers are split when a texture/material changes or 0x100000 bytes are
 * exceeded), collects the used material slots and sizes the clay's system buffer (tag area, material data, vertex data). Names are guesses. */
#include "types.h"

typedef struct CLAYV {          /* model type descriptor, 0x4C of a clay */
    s32 x00;
    s32 a;              /* 0x04 */
    s32 b;              /* 0x08 */
    s32 c;              /* 0x0C */
    long (*fn)();       /* 0x10 builds the draw packets */
    s32 mode;           /* 0x14 index into material_data_size */
    s32 inType;         /* 0x18 index into mdl_input_data */
    s32 x1C;
    u32 limit;          /* 0x20 */
} CLAYV;

typedef struct CLAYT {
    s32 flags;          /* 0x000 */
    s32 used;           /* 0x004 */
    s32 x08;
    s32 rs;             /* 0x00C */
    u8 x10[0x18 - 0x10];
    s32 total;          /* 0x018 */
    s32 mem2;           /* 0x01C system buffer handle */
    u8 *base;           /* 0x020 buffer address */
    s32 tagOfs;         /* 0x024 */
    u32 size;           /* 0x028 */
    s32 tblOfs;         /* 0x02C */
    s32 x30;
    s32 cnt;            /* 0x034 material / chain count */
    s32 ntex;           /* 0x038 */
    s32 x3C;
    s32 x40;
    s32 x44;
    s32 matkey;         /* 0x048 */
    CLAYV *v;           /* 0x04C */
    u8 x50[0x60 - 0x50];
    u8 mat[0x20];       /* 0x060 */
} CLAYT;

typedef struct SRCM {
    s32 flags;          /* 0x00 */
    s32 n;              /* 0x04 number of runs */
    u16 *runs;          /* 0x08 */
} SRCM;

extern s32 mdl_input_data[];
extern s32 material_data_size[];

void flMemset();
int flPS2GetSystemMemoryHandle();
int flPS2GetSystemBuffAdrs();

void flPS2GetClaySize(SRCM *src, CLAYT *c) {
    int tmp;
    int total;
    int s1;
    CLAYV *v;
    int cnt;
    int t8;
    int flags;
    int i;
    u32 limit;
    int lastA;
    int curA;
    int t3;
    int lastB;
    int curB;
    int n;
    int m16;
    u16 *p;
    s8 used[0x20];

    total = 0;
    v = c->v;
    s1 = 0;
    flMemset(used, 0, 0x20);
    n = src->n;
    lastA = -1;
    p = src->runs;
    curA = 0;
    lastB = -1;
    curB = 0;
    t8 = 0;
    t3 = 0;
    i = 0;
    c->x40 = 0;
    c->x3C = 0;
    c->x44 = 0;
    limit = v->limit + 0x20;
    if (0 < n) {
        do {
            cnt = *p & 0x3FFF;
            c->x40 += cnt;
            c->x3C += cnt - 2;
            flags = src->flags;
            p++;
            if (flags & 0x100000) {
                curA = *p;
                p++;
            }
            if (flags & 0x200000) {
                curB = *p;
                p++;
            }
            if (flags & 0x1000) {
                p += (u16)cnt;
            }
            if (lastA != curA) {
                if (t3 != 0) {
                    s1 += t8;
                    t3 = 0;
                    total += s1;
                }
                used[curA] = 1;
                lastA = curA;
                lastB = curB;
                s1 = 0x30;
                t8 = 0x20;
                c->x44++;
            } else if (lastB != curB) {
                if (t3 != 0) {
                    s1 += t8;
                    if (s1 + limit >= 0x100000) {
                        total += s1;
                        s1 = 0x10;
                    }
                    t3 = 0;
                }
                lastB = curB;
                s1 += 0x20;
                t8 = 0x20;
            }
            if (cnt > 0) {
                tmp = mdl_input_data[v->inType * 2];
                do {
                    if (t8 + tmp >= v->limit) {
                        s1 += t8;
                        if (s1 + limit >= 0x100000) {
                            total += s1;
                            s1 = 0x10;
                        }
                        s1 += 0x20;
                        t8 = 0x20;
                        t3 = 0;
                    }
                    cnt = (cnt - 1) & 0xFFFF;
                    t3++;
                    t8 += tmp;
                } while (cnt != 0);
            }
            i++;
        } while (i < n);
    }
    s1 += t8;
    total += s1;
    c->total = total;
    c->cnt = 0;
    for (flags = 0; flags < 0x20; flags++) {
        if (used[flags] != 0) {
            c->mat[c->cnt++] = flags;
        }
    }
    c->x30 = (c->cnt * 4 + 0xF) & ~0xF;
    c->size = (c->cnt * material_data_size[v->mode] + 0xF) & ~0xF;
    c->size += 0x20;
    c->tblOfs = (c->total + 0xF) & ~0xF;
    c->tagOfs = c->tblOfs + ((c->x30 + 0xF) & ~0xF);
    c->mem2 = flPS2GetSystemMemoryHandle(((total + 0xF) & ~0xF) + ((c->x30 + 0xF) & ~0xF) + ((c->size + 0xF) & ~(m16 = 0xF)), 3);
    c->base = (u8 *)flPS2GetSystemBuffAdrs(c->mem2);
}
