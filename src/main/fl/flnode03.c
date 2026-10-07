/* fl hierarchy motion playback (SLPM_654.95 0x00174550-0x00174594): flPlayMotionExSI. flPlayMotionExSISub follows in flnode03_nm.c. */
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

int flPlayMotionExSI(f32 t, FLNODE *n, int id) {
    flPlayMotionExSISub(t, flFindGroupRoot(n, id), id);
    return 1;
}
