/* hit2e - SLPM_654.95 0x00290420-0x00290560.
 * Whole file f_hit_28CE00 0x0028CE00-0x00290560 (hit2*.c, near-matches in
 * hit2_nm.c): point/sphere/capsule/plane/line intersection tests. HPK is a
 * capsule packed by hit_cap_pk (ends, radius, axis, centre, bounding radius).
 * *_m variants return a contact point or push-out vector in out. */
#include "hit2.h"

void p_point_line2(f32 *p0, f32 *p1, f32 *dir, f32 *pt, f32 *out, f32 len) {
    f32 v[3];
    f32 t;

    v[0] = pt[0] - p0[0];
    v[1] = pt[1] - p0[1];
    v[2] = pt[2] - p0[2];
    t = flvecInnerProduct(v, dir);
    if (t < 0.0f) {
        flvecCopy(out, p0);
    } else if (!(t <= len)) {
        flvecCopy(out, p1);
    } else {
        v[0] = t * dir[0];
        v[1] = t * dir[1];
        v[2] = t * dir[2];
        out[0] = p0[0] + v[0];
        out[1] = p0[1] + v[1];
        out[2] = p0[2] + v[2];
    }
}
