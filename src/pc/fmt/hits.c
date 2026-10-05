/*
 * hits.c - "HITS" collision files (GroundHitInit 0x114C70 /
 * WallHitInit 0x114B50; docs/formats/stage.md 4).
 *
 * "HITS", u32 size, then from +8 ("base"): s32 cell size x, z, cells x, z,
 * two zero words, u32 cell table offset, u32 polygon area offset. Each
 * cell is a -1 terminated list of polygon offsets; polygons are 56 bytes:
 * u32 attr, f32 v0[3] v1[3] v2[3], f32 normal[3], f32 d.
 */
#include "fmt.h"

#include <string.h>

int fmt_hits_ground_y(fmt_blob f, float x, float z, float y_max, float *y, int be)
{
    const uint8_t *b;
    int32_t csx, csz, nx, nz, cx, cz;
    uint32_t ct, pb, cell;
    int found = 0;

    if (f.n < 0x30 || memcmp(f.p, "HITS", 4) != 0)
        return 0;
    b = f.p + 8;
    csx = fmt_s32(b, be);
    csz = fmt_s32(b + 4, be);
    nx = fmt_s32(b + 8, be);
    nz = fmt_s32(b + 12, be);
    ct = fmt_u32(b + 0x18, be);
    pb = fmt_u32(b + 0x1C, be);
    if (csx <= 0 || csz <= 0)
        return 0;
    cx = (int32_t)(x / csx);
    cz = (int32_t)(z / csz);
    if (cx < 0 || cz < 0 || cx >= nx || cz >= nz)
        return 0;
    /* cell order: z-major or x-major is not confirmed, so try both */
    for (cell = 0; cell < 2; cell++) {
        uint32_t ci = cell ? (uint32_t)(cx * nz + cz) : (uint32_t)(cz * nx + cx);
        const uint8_t *l = b + fmt_u32(b + ct + 4 * ci, be);
        for (; fmt_s32(l, be) != -1; l += 4) {
            const uint8_t *p = b + pb + fmt_u32(l, be);
            float v[9], ny = fmt_f32(p + 4 + 40, be), d0, d1, d2, w0, w1, w2, h;
            int k;
            if (ny <= 0.1f)
                continue;
            for (k = 0; k < 9; k++)
                v[k] = fmt_f32(p + 4 + 4 * k, be);
            /* barycentric in XZ */
            d0 = (v[3] - x) * (v[8] - z) - (v[6] - x) * (v[5] - z);
            d1 = (v[6] - x) * (v[2] - z) - (v[0] - x) * (v[8] - z);
            d2 = (v[0] - x) * (v[5] - z) - (v[3] - x) * (v[2] - z);
            if (!((d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0)))
                continue;
            if (d0 + d1 + d2 == 0)
                continue;
            w0 = d0 / (d0 + d1 + d2);
            w1 = d1 / (d0 + d1 + d2);
            w2 = d2 / (d0 + d1 + d2);
            h = w0 * v[1] + w1 * v[4] + w2 * v[7];
            if (h <= y_max && (!found || h > *y)) {
                *y = h;
                found = 1;
            }
        }
    }
    return found;
}
