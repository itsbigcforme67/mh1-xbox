/* camr6_nm (not built): GetOrthogonalPoint 234/413 instructions differ (saved
 * register choice for out/mode, loop shape); the two inner-product helpers
 * are static copies so the clobber set is known, as in the original file. */
/* camr6 - SLPM_654.95 0x00224xxx (f_cam_223B50): GetOrthogonalPoint finds
 * the parameters t where the camera rail cubic P(t) = A t^3 + B t^2 + C t + D
 * (A at coef+0, B +0xC, C +0x18, D +0x24) is nearest to the point q, i.e.
 * the real roots of (P(t) - q) . P'(t) = 0, written to out[] (count returned).
 * The polynomial has degree 5 (DKA5), 3 (Cardano), 1, or 0 roots when the
 * leading terms are below 1e-10. mode 0 ignores y (XZ plane). Guesses. */
#include "types.h"

typedef struct DCMPLX {
    f32 re;
    f32 im;
} DCMPLX;

#define A (coef)
#define B (coef + 3)
#define C (coef + 6)
#define LEN2(v) ((v)[0] * (v)[0] + (v)[1] * (v)[1] + (v)[2] * (v)[2])

void flvecCopy(f32 *, f32 *);
void SubVector(f32 *, f32 *, f32 *);
f32 flAbs(f32);
static f32 vInnerProductXZ(f32 *a, f32 *b) {
    return a[2] * b[2] + a[0] * b[0];
}

static f32 vInnerProduct(f32 *a, f32 *b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
void DKA5(DCMPLX *, f32 *);
int Cardano(f32 *, f32 *);

int GetOrthogonalPoint(f32 *out, f32 *coef, f32 *q, int mode) {
    f32 E[4];
    f32 qq[4];
    f32 co[6];
    f32 rt[10];
    f32 f;
    int n;
    u32 k;

    flvecCopy(qq, q);
    if (mode == 0) {
        qq[1] = 0.0f;
        SubVector(E, coef + 9, qq);
    if ((double)LEN2(A) > 1.0e-10) goto quint0;
    if ((double)LEN2(B) > 1.0e-10) goto cubic0;
    if ((double)LEN2(C) > 1.0e-10) goto linear0;
    return 0;
linear0:
    f = vInnerProductXZ(C, C);
    *out = -(vInnerProductXZ(C, E) / f);
    return 1;
cubic0:
    co[0] = 2.0f * vInnerProductXZ(B, B);
    co[1] = 3.0f * vInnerProductXZ(B, C);
    co[2] = 2.0f * vInnerProductXZ(B, E) + vInnerProductXZ(C, C);
    co[3] = vInnerProductXZ(C, E);
    return Cardano(out, co);
quint0:
    co[0] = 3.0f * vInnerProductXZ(A, A);
    co[1] = 5.0f * vInnerProductXZ(A, B);
    co[2] = 4.0f * vInnerProductXZ(A, C) + 2.0f * vInnerProductXZ(B, B);
    co[3] = 3.0f * (vInnerProductXZ(A, E) + vInnerProductXZ(B, C));
    co[4] = 2.0f * vInnerProductXZ(B, E) + vInnerProductXZ(C, C);
    co[5] = vInnerProductXZ(C, E);
    DKA5((DCMPLX *)rt, co);
    n = 0;
    for (k = 0; k < 5; k++) {
        f32 *p = &rt[k * 2];
        if (flAbs(p[1]) < 0.001f) {
            *out = p[0];
            n++;
            out++;
        }
    }
    return n;
    }
    SubVector(E, coef + 9, qq);
    if ((double)LEN2(A) > 1.0e-10) goto quint1;
    if ((double)LEN2(B) > 1.0e-10) goto cubic1;
    if ((double)LEN2(C) > 1.0e-10) goto linear1;
    return 0;
linear1:
    f = vInnerProduct(C, C);
    *out = -(vInnerProduct(C, E) / f);
    return 1;
cubic1:
    co[0] = 2.0f * vInnerProduct(B, B);
    co[1] = 3.0f * vInnerProduct(B, C);
    co[2] = 2.0f * vInnerProduct(B, E) + vInnerProduct(C, C);
    co[3] = vInnerProduct(C, E);
    return Cardano(out, co);
quint1:
    co[0] = 3.0f * vInnerProduct(A, A);
    co[1] = 5.0f * vInnerProduct(A, B);
    co[2] = 4.0f * vInnerProduct(A, C) + 2.0f * vInnerProduct(B, B);
    co[3] = 3.0f * (vInnerProduct(A, E) + vInnerProduct(B, C));
    co[4] = 2.0f * vInnerProduct(B, E) + vInnerProduct(C, C);
    co[5] = vInnerProduct(C, E);
    DKA5((DCMPLX *)rt, co);
    n = 0;
    for (k = 0; k < 5; k++) {
        f32 *p = &rt[k * 2];
        if (flAbs(p[1]) < 0.001f) {
            *out = p[0];
            n++;
            out++;
        }
    }
    return n;
}
