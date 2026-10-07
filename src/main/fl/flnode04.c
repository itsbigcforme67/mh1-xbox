/* fl hierarchy scaled transform (SLPM_654.95 0x001746A0-0x001747F4): flCalcTransSI, flCalcTransSISub (recursive node walk that cancels the parent scale). */
#include "types.h"

typedef struct FLNODE FLNODE;
struct FLNODE {
    u8 x0[0x40];
    u8 mat[0x40];       /* 0x40 */
    u8 x80[0xC6 - 0x80];
    u16 grp;            /* 0xC6 */
    FLNODE *parent;     /* 0xC8 */
    FLNODE *sib;        /* 0xCC */
    FLNODE *child;      /* 0xD0 */
    int handle;         /* 0xD4 */
    int data;           /* 0xD8 */
    f32 init[10];       /* 0xDC */
    f32 cur[10];        /* 0x104 */
    u8 x12C[0x15C - 0x12C];
    u8 work[1];         /* 0x15C */
};

extern int base_addr_0038A25C;
FLNODE *flFindGroupRoot(FLNODE *, int);
void flMotionSetBaseAddress(int);
void flGetFcurveValue(int, f32 *, f32 *, f32, u8 *);
void flGetMatrixWithoutScale(void *, void *);
void flmatInit(void *);
void flmatMakeScale(void *, f32, f32, f32);
void flmatMul33_2(void *, void *);
void flmatMul2(void *, void *);

int flPlayMotionExSISub(f32 t, FLNODE *n, int id);

void flCalcTransSISub(FLNODE *n, FLNODE *p, f32 sx, f32 sy, f32 sz);

void flCalcTransSI(FLNODE *n, FLNODE *p) {
    flCalcTransSISub(n, p, 1.0f, 1.0f, 1.0f);
}

void flCalcTransSISub(FLNODE *n, FLNODE *p, f32 sx, f32 sy, f32 sz) {
    f32 m[16];
    f32 cx, cy, cz;

    cx = *(f32 *)((u8 *)n + 0x104) * sx;
    cy = *(f32 *)((u8 *)n + 0x108) * sy;
    cz = *(f32 *)((u8 *)n + 0x10C) * sz;
    flmatMakeScale(n, cx, cy, cz);
    flmatInit(m);
    m[0] = 1.0f / sx;
    m[5] = 1.0f / sy;
    m[10] = 1.0f / sz;
    m[12] = *(f32 *)((u8 *)n + 0x70);
    m[13] = *(f32 *)((u8 *)n + 0x74);
    m[14] = *(f32 *)((u8 *)n + 0x78);
    flmatMul33_2(n, n->mat);
    flmatMul2(n, m);
    flmatMul2(n, p);
    if (n->child != 0) {
        flCalcTransSISub(n->child, n, cx, cy, cz);
    }
    if (n->sib != 0) {
        flCalcTransSISub(n->sib, p, sx, sy, sz);
    }
}
