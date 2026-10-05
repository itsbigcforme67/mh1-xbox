/* hit2 - SLPM_654.95 0x0028CE00-0x0028CEB0.
 * Whole file f_hit_28CE00 0x0028CE00-0x00290560 (hit2*.c, near-matches in
 * hit2_nm.c): point/sphere/capsule/plane/line intersection tests. HPK is a
 * capsule packed by hit_cap_pk (ends, radius, axis, centre, bounding radius).
 * *_m variants return a contact point or push-out vector in out. */
#include "hit2.h"

u8 hit_point_sphr(f32 *p, f32 *c, f32 r) {
    f32 dx = c[0] - p[0];
    f32 dy = c[1] - p[1];
    f32 dz = c[2] - p[2];

    return dx * dx + dy * dy + dz * dz <= r * r;
}

u8 hit_sphr_sphr(f32 *a, f32 *b, f32 ra, f32 rb) {
    f32 dx = a[0] - b[0];
    f32 dy = a[1] - b[1];
    f32 dz = a[2] - b[2];
    f32 r = ra + rb;

    return dx * dx + dy * dy + dz * dz <= r * r;
}
