/*
 * rt_main.c - small main-program (SLPM_654.95) functions that the ported
 * game C calls but that are not decompiled yet, written natively from the
 * asm. Replace each with the decompiled C once it matches.
 */
#include "rt.h"
#include "types.h"

#include <stdio.h>

f32 flvecCalcDistance(f32 *a, f32 *b);
void flvecCopy(f32 *dst, f32 *src);

/* clr_flash (0x15C4B0, f_stage): clears the screen-flash state. */
s16 flash_flag, flash_timer;
void clr_flash(void)
{
    flash_flag = 0;
    flash_timer = 0;
}

/* hit_cap_pk (0x28CD20): packs a capsule {p0, p1, r} for the hit tests:
 * +0 p0, +0xC p1, +0x18 r, +0x1C p1 - p0 (not normalised), +0x28 centre,
 * +0x34 bounding radius = |p0 - centre| + r. */
void hit_cap_pk(f32 *cap, f32 *pk)
{
    flvecCopy(pk, cap);
    flvecCopy(pk + 3, cap + 3);
    pk[6] = cap[6];
    pk[7] = cap[3] - cap[0];
    pk[8] = cap[4] - cap[1];
    pk[9] = cap[5] - cap[2];
    pk[10] = 0.5f * (cap[0] + cap[3]);
    pk[11] = 0.5f * (cap[1] + cap[4]);
    pk[12] = 0.5f * (cap[2] + cap[5]);
    pk[13] = flvecCalcDistance(cap, pk + 10);
    pk[13] = pk[13] + cap[6];
}

/* hit_point_cyl (0x290560): not ported yet (only set13 kind 4, stage 0x15). */
int hit_point_cyl(f32 *p, f32 *c, f32 r, f32 y0, f32 y1)
{
    static int once;
    if (!once++)
        fprintf(stderr, "rt: hit_point_cyl not ported yet (returns 0)\n");
    return 0;
}
