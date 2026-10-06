/* fl hierarchy build, node layout see flnode05_nm.c (SLPM_654.95 0x00174C90-0x00174EB8): flGetMatrixWithoutScale, flGetMatrixSI, flGetMatrixMAYA. */
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

int flGetMatrixWithoutScale(f32 *mat, f32 *v) {
    flmatInit(mat);
    flmatRotXYZ33(mat, v[3], v[4], v[5]);
    mat[12] = v[6];
    mat[13] = v[7];
    mat[14] = v[8];
    return 1;
}

int flGetMatrixSI(f32 *mat, f32 *v, f32 sx, f32 sy, f32 sz) {
    f32 r[16];
    f32 s[16];

    flmatMakeScale(mat, v[0] * sx, v[1] * sy, v[2] * sz);
    flmatInit(r);
    flmatRotXYZ33(r, v[3], v[4], v[5]);
    flmatInit(s);
    s[0] = 1.0f / sx;
    s[5] = 1.0f / sy;
    s[10] = 1.0f / sz;
    s[12] = v[6];
    s[13] = v[7];
    s[14] = v[8];
    flmatMul2(mat, r);
    flmatMul2(mat, s);
    return 1;
}

int flGetMatrixMAYA(f32 *mat, f32 *v, f32 sx, f32 sy, f32 sz) {
    f32 r[16];
    f32 s[16];

    flmatMakeScale(mat, v[0], v[1], v[2]);
    flmatInit(r);
    flmatRotXYZ33(r, v[3], v[4], v[5]);
    flmatInit(s);
    s[0] = 1.0f / sx;
    s[5] = 1.0f / sy;
    s[10] = 1.0f / sz;
    s[12] = v[6];
    s[13] = v[7];
    s[14] = v[8];
    flmatMul2(mat, r);
    flmatMul2(mat, s);
    return 1;
}
