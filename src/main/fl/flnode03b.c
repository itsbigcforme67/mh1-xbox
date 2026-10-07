/* fl motion: flPlayMotionExSISub (SLPM_654.95 0x001745A0-0x0017469C), file-static recursive part of flPlayMotionExSI: copies the initial values of the
 * node into its current values, evaluates the f-curves of its motion (flGetFcurveValue takes the curve data address, init, current values, t, work),
 * builds the node matrix and recurses into child and sibling. Node fields: +0xCC next sibling, +0xD0 first child, +0xD4 motion handle,
 * +0xD8 curve offset, +0xDC initial values (10 floats), +0x104 current values, +0x15C curve work. */
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

int flPlayMotionExSISub(f32 t, FLNODE *n, int id);

int flPlayMotionExSISub(f32 t, FLNODE *n, int id) {
    int i;
    u8 *q;

    flMotionSetBaseAddress(n->handle);
    i = 0;
    if (n->grp == (u16)id) {
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
        if (n->handle != 0) {
            flGetFcurveValue(n->data + base_addr_0038A25C, n->init, n->cur, t, n->work);
        }
        flGetMatrixWithoutScale(n->mat, n->cur);
        if (n->child != 0) {
            flPlayMotionExSISub(t, n->child, id);
        }
    }
    if (n->sib != 0) {
        flPlayMotionExSISub(t, n->sib, id);
    }
    return 1;
}
