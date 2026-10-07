/* fl node hierarchy: initial posture of a node tree (SLPM_654.95 0x00174B60-0x00174C84). flInitPostureHierarchyMAYASub (Maya node data).
 * The recursive static takes (node, sx, parent matrix, sy, sz): that parameter order gives the original argument set-up order of its calls. Node layout: see flnode05_nm.c. */
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
int flInitPostureHierarchySISub(FLNODE *, f32, f32 *, f32, f32);
int flInitPostureHierarchyMAYASub(FLNODE *, f32, f32 *, f32, f32);
int flGetMatrixSI(f32 *, f32 *, f32, f32, f32);
int flGetMatrixMAYA(f32 *, f32 *, f32, f32, f32);
int flGetMatrixWithoutScale(f32 *, f32 *);
int flGetHierarchyData2(FLNODE *, u8 *, int);
int flGetHierarchy3(FLNODE *, int, int, int);
int flGetHierarchy3_sub(FLNODE *, FLNODE *, FLNODE *);

int flInitPostureHierarchyMAYASub(FLNODE *n, f32 sx, f32 *p, f32 sy, f32 sz) {
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
        flInitPostureHierarchyMAYASub(n->child, sx * n->cur[0], m2, sy * n->cur[1], sz * n->cur[2]);
    }
    if (n->sib != 0) {
        flInitPostureHierarchyMAYASub(n->sib, sx, p, sy, sz);
    }
    return 1;
}
