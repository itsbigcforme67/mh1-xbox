/* SLPM_654.95 0x00174810-0x00174938: flGetHierarchy3 .. flGetHierarchy3. See flnode05_nm.c. */
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
