/* hit3 - SLPM_654.95 0x00290560-0x002905E8 (g_hit_point_cyl): point test
 * against a solid. hit_point_cyl: is point p inside the vertical
 * cylinder of radius r at c (top / bottom are offsets from c.y, tested only
 * when top > 0)? Guess from the code. */
#include "types.h"

f32 flAbs(f32);
f32 flvecInnerProduct(f32 *, f32 *);
void flvecNormalize(f32 *);

int hit_point_cyl(f32 *p, f32 *c, f32 r, f32 top, f32 bot) {
    f32 dx, dz;

    if (!(top <= 0.0f)) {
        if (!(p[1] <= c[1] + top) || (p[1] < c[1] + bot)) return 0;
    }
    dx = c[0] - p[0];
    dz = c[2] - p[2];
    if (dx * dx + dz * dz <= r * r) return 1;
    return 0;
}

u8 hit_point_cbd(f32 *p, f32 *a, f32 *b, f32 h, f32 w) {
    f32 n[3];
    f32 d[3];
    f32 v[3];

    if (p[1] < a[1] || !(p[1] <= a[1] + h)) return 0;
    d[0] = b[0] - a[0];
    d[1] = b[1] - a[1];
    d[2] = b[2] - a[2];
    n[0] = -h * d[2];
    n[1] = 0.0f;
    n[2] = d[0] * h;
    flvecNormalize(n);
    v[0] = p[0] - a[0];
    v[1] = p[1] - a[1];
    v[2] = p[2] - a[2];
    if (!(flAbs(flvecInnerProduct(n, v)) <= w)) return 0;
    if (flvecInnerProduct(d, v) < 0.0f) return 0;
    v[0] = p[0] - b[0];
    v[1] = p[1] - b[1];
    v[2] = p[2] - b[2];
    return !(flvecInnerProduct(d, v) > 0.0f);
}
