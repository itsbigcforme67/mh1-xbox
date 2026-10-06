/* Near-match (not linked): the rest of 0x00174810-0x00175120. flGetHierarchy3 11/74 off (the original starts the loop counters in the branch delay slots and
 * uses different counters for its two loops), flInitPostureHierarchySISub / MAYASub 2 off each (argument register copy order before the sibling call),
 * flGetHierarchyData2 2 off, flGetFcurveValue 7 off (loop counter / curve pointer registers swapped in the second case).
 * A node is 0x190 bytes: +0x40 matrix, +0x80 inverse (skin) matrix, +0xC2 node count, +0xC4 matrix index, +0xC6 group id, +0xC8 parent, +0xCC next sibling,
 * +0xD0 first child, +0xD4 motion handle, +0xD8 curve offset, +0xDC initial values (10 floats), +0x104 current values, +0x15C curve work,
 * +0x174 flags, +0x178/+0x17C, +0x180 sibling index, +0x182 child index. */
#include "types.h"

typedef struct FLNODE FLNODE;
struct FLNODE {
    u8 x0[0x40];
    f32 mat[16];        /* 0x40 */
    f32 inv[16];        /* 0x80 */
    u8 xC0[2];
    s16 cnt;            /* 0xC2 */
    s16 mi;             /* 0xC4 */
    u16 grp;            /* 0xC6 */
    FLNODE *parent;     /* 0xC8 */
    FLNODE *sib;        /* 0xCC */
    FLNODE *child;      /* 0xD0 */
    int handle;         /* 0xD4 */
    int data;           /* 0xD8 */
    f32 init[10];       /* 0xDC */
    f32 cur[10];        /* 0x104 */
    u8 x12C[0x174 - 0x12C];
    int flags;          /* 0x174 */
    int x178;           /* 0x178 */
    int x17C;           /* 0x17C */
    s16 sibIdx;         /* 0x180 */
    s16 childIdx;       /* 0x182 */
    u8 x184[0x190 - 0x184];
};

typedef struct MOT {
    u16 flags;
    s16 cnt;
    s32 off;
} MOT;

typedef struct CURVE {
    u8 x0;
    u8 idx;
    u8 x2[6];
} CURVE;

extern s32 base_addr_0038A25C;
extern f32 flPS2INITMATRIX[];
u8 *flPS2GetSystemBuffAdrs(int);
f32 flFCVGetValue2(f32, CURVE *, s16 *);
void flmatInit(void *);
void flmatMakeScale(void *, f32, f32, f32);
void flmatRotXYZ33(void *, f32, f32, f32);
void flmatMul(void *, void *, void *);
void flmatMul2(void *, void *);
void flmatInvert(void *, void *);
int flInitPostureHierarchySI(FLNODE *);
int flInitPostureHierarchyMAYA(FLNODE *);
int flInitPostureHierarchySISub(FLNODE *, f32 *, f32, f32, f32);
int flInitPostureHierarchyMAYASub(FLNODE *, f32 *, f32, f32, f32);
int flGetMatrixSI(f32 *, f32 *, f32, f32, f32);
int flGetMatrixMAYA(f32 *, f32 *, f32, f32, f32);
int flGetMatrixWithoutScale(f32 *, f32 *);
int flGetHierarchyData2(FLNODE *, u8 *, int);
int flGetHierarchy3(FLNODE *, int, int, int);
int flGetHierarchy3_sub(FLNODE *, FLNODE *, FLNODE *);

int flGetHierarchy3(FLNODE *nodes, int h, int unused, int mode) {
    FLNODE *m;
    int j;
    int i;
    int cnt;
    u8 *d;
    FLNODE *n;

    d = flPS2GetSystemBuffAdrs(h);
    cnt = *(s16 *)(d + 2);
    i = 0;
    if (0 < cnt) {
        n = nodes;
        do {
            n->cnt = cnt;
            n->sibIdx = -1;
            n->childIdx = -1;
            n->sib = 0;
            n->child = 0;
            n->handle = 0;
            n->data = 0;
            n->x178 = 0;
            n->x17C = 0;
            flmatInit(n->mat);
            n++;
            i++;
        } while (i < cnt);
    }
    if (0 < cnt) {
        j = 0;
        m = nodes;
        do {
            flGetHierarchyData2(m, d, j);
            j++;
            m++;
        } while (j < cnt);
    }
    flGetHierarchy3_sub(nodes, nodes, 0);
    switch (mode) {
    case 0:
        flInitPostureHierarchySI(nodes);
        break;
    case 1:
        flInitPostureHierarchyMAYA(nodes);
        break;
    }
    return 1;
}

int flInitPostureHierarchySISub(FLNODE *n, f32 *p, f32 sx, f32 sy, f32 sz) {
    int i;
    u8 *q;
    f32 m[16];

    i = 0;
    q = (u8 *)n;
    do {
        i += 5;
        *(f32 *)(q + 0x104) = *(f32 *)(q + 0xDC);
        *(f32 *)(q + 0x108) = *(f32 *)(q + 0xE0);
        *(f32 *)(q + 0x10C) = *(f32 *)(q + 0xE4);
        *(f32 *)(q + 0x110) = *(f32 *)(q + 0xE8);
        *(f32 *)(q + 0x114) = *(f32 *)(q + 0xEC);
        q += 0x14;
    } while (i < 10);
    flGetMatrixSI(n->mat, n->cur, sx, sy, sz);
    flmatMul(m, n->mat, p);
    flmatInvert(n->inv, m);
    if (n->child != 0) {
        flInitPostureHierarchySISub(n->child, m, sx * n->cur[0], sy * n->cur[1], sz * n->cur[2]);
    }
    if (n->sib != 0) {
        flInitPostureHierarchySISub(n->sib, p, sx, sy, sz);
    }
    return 1;
}

int flInitPostureHierarchyMAYASub(FLNODE *n, f32 *p, f32 sx, f32 sy, f32 sz) {
    int i;
    u8 *q;
    f32 m2[16];
    f32 m1[16];

    i = 0;
    q = (u8 *)n;
    do {
        i += 5;
        *(f32 *)(q + 0x104) = *(f32 *)(q + 0xDC);
        *(f32 *)(q + 0x108) = *(f32 *)(q + 0xE0);
        *(f32 *)(q + 0x10C) = *(f32 *)(q + 0xE4);
        *(f32 *)(q + 0x110) = *(f32 *)(q + 0xE8);
        *(f32 *)(q + 0x114) = *(f32 *)(q + 0xEC);
        q += 0x14;
    } while (i < 10);
    flGetMatrixWithoutScale(n->mat, n->cur);
    flGetMatrixMAYA(m1, n->cur, sx, sy, sz);
    flmatMul(m2, m1, p);
    flmatInvert(n->inv, m2);
    if (n->child != 0) {
        flInitPostureHierarchyMAYASub(n->child, m2, sx * n->cur[0], sy * n->cur[1], sz * n->cur[2]);
    }
    if (n->sib != 0) {
        flInitPostureHierarchyMAYASub(n->sib, p, sx, sy, sz);
    }
    return 1;
}

int flGetHierarchyData2(FLNODE *n, u8 *p, int i) {
    p += 0x10;
    p += i << 6;

    n->flags = *(s16 *)(p + 0xA);
    n->mi = *(s16 *)p;
    n->grp = *(u16 *)(p + 2);
    n->childIdx = *(s16 *)(p + 6);
    n->sibIdx = *(s16 *)(p + 8);
    n->cur[0] = n->init[0] = *(f32 *)(p + 0x10);
    n->cur[1] = n->init[1] = *(f32 *)(p + 0x14);
    n->cur[2] = n->init[2] = *(f32 *)(p + 0x18);
    n->cur[3] = n->init[3] = *(f32 *)(p + 0x20);
    n->cur[4] = n->init[4] = *(f32 *)(p + 0x24);
    n->cur[5] = n->init[5] = *(f32 *)(p + 0x28);
    n->cur[6] = n->init[6] = *(f32 *)(p + 0x30);
    n->cur[7] = n->init[7] = *(f32 *)(p + 0x34);
    n->cur[8] = n->init[8] = *(f32 *)(p + 0x38);
    n->data = 0;
    n->handle = 0;
    return 1;
}

int flGetFcurveValue(f32 t, MOT *mot, f32 *unused, f32 *v, s16 *hint) {
    int j;
    CURVE *c;
    u16 idx;

    if (mot != 0) {
        switch (mot->flags & 0xF000) {
            if (c) {
            }
        case 0x1000:
            j = 0;
            c = (CURVE *)(mot->off + base_addr_0038A25C);
            for (; j < mot->cnt; j++) {
                idx = c->idx;
                v[idx] = flFCVGetValue2(t, c, hint + idx);
                c++;
            }
            break;
        case 0x2000:
            j = 0;
            c = (CURVE *)(mot->off + base_addr_0038A25C);
            for (; j < mot->cnt; j++) {
                idx = c->idx;
                v[idx] = flFCVGetValue2(t, c, hint + idx);
                switch (idx) {
                case 3:
                case 4:
                case 5:
                    v[idx] = v[idx] * 0.0003834952f;
                    break;
                default:
                    v[idx] = v[idx] / 16.0f;
                    break;
                }
                c++;
            }
            break;
        }
    }
    return 1;
}
