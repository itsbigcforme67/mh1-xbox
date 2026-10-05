/* hit2d - SLPM_654.95 0x00290140-0x00290280.
 * Whole file f_hit_28CE00 0x0028CE00-0x00290560 (hit2*.c, near-matches in
 * hit2_nm.c): point/sphere/capsule/plane/line intersection tests. HPK is a
 * capsule packed by hit_cap_pk (ends, radius, axis, centre, bounding radius).
 * *_m variants return a contact point or push-out vector in out. */
#include "hit2.h"

void hit_cap_cap3_sub(f32 *v, f32 *out, f32 r, f32 d) {
    f32 t;

    if (d != 0.0f) {
        t = r - d;
        if (d < 0.001f) {
            d = 0.001f;
        }
        t = t / d;
        out[0] = t * v[0];
        out[1] = t * v[1];
        out[2] = t * v[2];
    }
}

u8 hit_sphr_pln(f32 *c, f32 *p, f32 *n, f32 *out, f32 r) {
    f32 v[3];
    f32 nn[3];
    f32 d;

    v[0] = p[0] - c[0];
    v[1] = p[1] - c[1];
    v[2] = p[2] - c[2];
    flvecCopy(nn, n);
    flvecNormalize(nn);
    d = flvecInnerProduct(nn, v);
    if (d <= 0.0f && !(-d <= r)) {
        return 0;
    }
    d = d + r;
    out[0] = d * nn[0];
    out[1] = d * nn[1];
    out[2] = d * nn[2];
    return 1;
}
