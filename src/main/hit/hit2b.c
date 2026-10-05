/* hit2b - SLPM_654.95 0x0028CFB0-0x0028D0A0.
 * Whole file f_hit_28CE00 0x0028CE00-0x00290560 (hit2*.c, near-matches in
 * hit2_nm.c): point/sphere/capsule/plane/line intersection tests. HPK is a
 * capsule packed by hit_cap_pk (ends, radius, axis, centre, bounding radius).
 * *_m variants return a contact point or push-out vector in out. */
#include "hit2.h"

u8 hit_sphr_sphr3(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb) {
    f32 d = flvecCalcDistance(a, b);
    f32 r = ra + rb;
    f32 t;
    f32 dx;
    f32 dy;
    f32 dz;

    if (d <= r) {
        if (!(d <= 0.001f)) {
            t = (r - d) / d;
            dx = a[0] - b[0];
            dy = a[1] - b[1];
            dz = a[2] - b[2];
            out[0] = t * dx;
            out[1] = t * dy;
            out[2] = t * dz;
        } else {
            out[0] = 0.0f;
            out[1] = 0.0f;
            out[2] = 0.0f;
        }
        return 1;
    }
    return 0;
}
