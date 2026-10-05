/* camr_nm - near-matches of f_cam_223B50 (SLPM_654.95 0x00223B50-0x00225200),
 * not built. dDivComplex is 13 of 34 instructions off (float register
 * numbers only: the original loads b->re/b->im into f3/f4 after the 0.0
 * constant, d and 1/d took f0-f2). */
#include "types.h"

typedef struct DCMPLX {
    f32 re;
    f32 im;
} DCMPLX;

void dDivComplex(DCMPLX *r, DCMPLX *a, DCMPLX *b) {
    DCMPLX t;
    f32 d;
    f32 inv;

    d = b->re * b->re + b->im * b->im;
    if (d != 0.0f) {
        inv = 1.0f / d;
        t.re = inv * (a->re * b->re + a->im * b->im);
        t.im = inv * (a->im * b->re - a->re * b->im);
        *r = t;
    } else {
        *r = *a;
    }
}

/* ZoomRateCalc 8/34, ZoomBaseAngleRail 1/10 (addu operand order), RollAngleRail
 * 11/28 (same): near-matches. */
f32 ZoomRateCalc(f32 x, f32 *z) {
    if (x <= z[2]) {
        return z[4];
    }
    if (!(x < z[3])) {
        return z[5];
    }
    if (z[3] == z[2]) {
        return 0.5f * (z[4] + z[5]);
    }
    return (z[5] - z[4]) / (z[3] - z[2]) * (x - z[2]) + z[4];
}

f32 ZoomBaseAngleRail(f32 t, f32 *rail, int i) {
    return rail[i + 0x80] * (1.0f - t) + rail[i + 0x81] * t;
}

f32 RollAngleRail(f32 t, s16 *rail, int i) {
    union {
        s32 w;
        s16 h[2];
    } u;

    u.w = (s32)(65536.0f * t) * (rail[i + 0x121] - rail[i + 0x120]);
    u.h[1] += rail[i + 0x120];
    return 0.000095873799f * u.h[1];
}
