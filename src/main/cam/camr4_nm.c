/* camr4_nm (not built): whole file, Spline is near-match (259/262 differ, pointer
 * strength reduction). tri_diag matches and is built from camr4.c.
 * camr4 - SLPM_654.95 0x002246xx-0x00224D30 (f_cam_223B50, rail camera):
 * tri_diag solves a tridiagonal system (Thomas algorithm, at most 64
 * unknowns), Spline builds the cubic coefficients of a natural-style spline
 * through n points {x, y, z, dt} (dt = parameter span to the next point):
 * 12 floats per segment (a, b, c, d for x, y and z). Guesses from the code. */
#include "types.h"

f32 flPow(f32, f32);

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

void Spline(f32 *out, f32 *pts, int n) {
    f32 w[9 * 0x10];
    f32 *sp = pts;
    f32 *o = out;
    f32 *p = w;
    f32 hp, hc, r1, r2, r, r3;
    int i;

    p[0] = 0.0f;
    p[0x10] = 2.0f * pts[3];
    p[0x20] = pts[3];
    p[0x30] = 3.0f * (pts[4] - pts[0]);
    p[0x40] = 3.0f * (pts[5] - pts[1]);
    p[0x50] = 3.0f * (pts[6] - pts[2]);
    if (n - 1 >= 2) {
        i = 1;
        p = w + 1;
        sp = pts + 4;
        do {
            i++;
            hp = sp[-1];
            hc = sp[3];
            r1 = hc / hp;
            p[0] = hc;
            r2 = hp / hc;
            p[0x10] = 2.0f * (hp + hc);
            p[0x20] = hp;
            p[0x30] = 3.0f * (r1 * (sp[0] - sp[-4]) + r2 * (sp[4] - sp[0]));
            p[0x40] = 3.0f * (r1 * (sp[1] - sp[-3]) + r2 * (sp[5] - sp[1]));
            p[0x50] = 3.0f * (r1 * (sp[2] - sp[-2]) + r2 * (sp[6] - sp[2]));
            sp += 4;
            p++;
        } while (i < n - 1);
    }
    p = w + n;
    sp = pts + n * 4;
    p[-1] = sp[-5];
    p[0xF] = 2.0f * sp[-5];
    p[0x1F] = 0.0f;
    p[0x2F] = 3.0f * (sp[-4] - sp[-8]);
    p[0x3F] = 3.0f * (sp[-3] - sp[-7]);
    p[0x4F] = 3.0f * (sp[-2] - sp[-6]);
    tri_diag(w + 0x60, w, w + 0x10, w + 0x20, w + 0x30, n);
    tri_diag(w + 0x70, w, w + 0x10, w + 0x20, w + 0x40, n);
    tri_diag(w + 0x80, w, w + 0x10, w + 0x20, w + 0x50, n);
    p = w;
    for (i = 0; i < n - 1; i++) {
        r = 1.0f / pts[3];
        r2 = r * r;
        r3 = 1.0f / flPow(pts[3], 3.0f);
        o[0] = r2 * (p[0x60] + p[0x61]) + r3 * (2.0f * (pts[0] - pts[4]));
        o[3] = r2 * (3.0f * (pts[4] - pts[0])) - r * (2.0f * p[0x60] + p[0x61]);
        o[6] = p[0x60];
        o[9] = pts[0];
        o[1] = r2 * (p[0x70] + p[0x71]) + r3 * (2.0f * (pts[1] - pts[5]));
        o[4] = r2 * (3.0f * (pts[5] - pts[1])) - r * (2.0f * p[0x70] + p[0x71]);
        o[7] = p[0x70];
        o[10] = pts[1];
        o[2] = r2 * (p[0x80] + p[0x81]) + r3 * (2.0f * (pts[2] - pts[6]));
        o[5] = r2 * (3.0f * (pts[6] - pts[2])) - r * (2.0f * p[0x80] + p[0x81]);
        o[8] = p[0x80];
        o[11] = pts[2];
        p++;
        pts += 4;
        o += 12;
    }
}
