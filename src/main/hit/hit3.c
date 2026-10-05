/* hit3 - SLPM_654.95 0x00290560-0x002905E8 (g_hit_point_cyl): point test
 * against a solid. hit_point_cyl: is point p inside the vertical
 * cylinder of radius r at c (top / bottom are offsets from c.y, tested only
 * when top > 0)? Guess from the code. */
#include "types.h"

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
