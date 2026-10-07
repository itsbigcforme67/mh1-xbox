/* fl library matrix register file helpers (SLPM_654.95 0x00173450-0x00173534): flmatrStore / flmatrLoad copy a 4x4 matrix to / from the
 * flMATRIX register file (64 bytes per register), flvecrRotTransPers projects a vector through the view and projection registers. */
#include "types.h"

typedef struct FM { int m[16]; } FM;
extern u8 flMATRIX[];
extern u8 flACRVIEWPROJ[];

void flmatMul();
void flmatMul2();
void flvecApplyMatTrans();

void flmatrStore(int n, FM *s) {
    *(FM *)(flMATRIX + n * 64) = *s;
}

void flmatrLoad(FM *d, int n) {
    *d = *(FM *)(flMATRIX + n * 64);
}

void flvecrRotTransPers(f32 *out, f32 *in) {
    FM m;

    flmatMul(&m, flMATRIX, flMATRIX + 0x840);
    flmatMul2(&m, flACRVIEWPROJ);
    flvecApplyMatTrans(out, in, &m);
}
