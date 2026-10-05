/* camr4 - SLPM_654.95 0x00224790-0x002248B8 (f_cam_223B50, rail camera):
 * tri_diag solves a tridiagonal system (Thomas algorithm, at most 64
 * unknowns). Spline (cubic spline coefficients through points {x, y, z, dt},
 * 12 floats per segment) is in camr4_nm.c, 259/262 instructions off (the
 * original addresses the work arrays from one base pointer). */
#include "types.h"

void tri_diag(f32 *x, f32 *a, f32 *b, f32 *c, f32 *d, int n) {
    f32 g[0x40];
    f32 f;
    f32 ai;
    int i;

    f = b[0];
    if (f != 0.0f) f = 1.0f / f;
    x[0] = d[0] * f;
    for (i = 1; i < n; i++) {
        g[i - 1] = f * c[i - 1];
        ai = a[i];
        f = b[i] - ai * g[i - 1];
        if (f != 0.0f) f = 1.0f / f;
        x[i] = f * (d[i] - ai * x[i - 1]);
    }
    for (i = n - 2; i >= 0; i--) {
        x[i] = x[i] - g[i] * x[i + 1];
    }
}
