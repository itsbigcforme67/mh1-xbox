/*
 * gfx_pal.c - shared by the graphics backends: an RGBA image with up to 256
 * colours as 8-bit indices + palette (the Xbox backend stores such
 * textures as NV2A P8, 1 byte per texel), and the GPU bytes the Xbox
 * backend would use for a texture (for the PC's memory report).
 */
#include "gfx.h"
#include <string.h>

int gfx_to_indexed(const uint32_t *src, int n, uint32_t *pal, uint8_t *idx)
{
    enum { HS = 1024 };
    uint32_t key[HS];
    int16_t val[HS];
    int i, np = 0;
    memset(val, -1, sizeof val);
    for (i = 0; i < n; i++) {
        uint32_t c = src[i], h = (c * 2654435761u) >> 22;
        while (val[h] >= 0 && key[h] != c)
            h = (h + 1) & (HS - 1);
        if (val[h] < 0) {
            if (np == 256)
                return -1;
            key[h] = c;
            val[h] = (int16_t)np;
            pal[np++] = c;
        }
        if (idx)
            idx[i] = (uint8_t)val[h];
    }
    return np;
}

static int pow2(int v) { return v > 0 && (v & (v - 1)) == 0; }

long gfx_xbox_texture_bytes(int w, int h, const uint8_t *rgba)
{
    uint32_t pal[256];
    if (pow2(w) && pow2(h) && w * h >= 256 && gfx_to_indexed((const uint32_t *)rgba, w * h, pal, NULL) >= 0)
        return (long)w * h + 1024;
    return (long)w * h * 4;
}

/* One triangle against the plane w >= cw (Sutherland-Hodgman): up to 4
 * output vertices, each lerp(corner a[i], corner b[i], t[i]) (t 0 = the
 * corner itself); returns the count (0, 3 or 4). For backends that divide
 * by w in the vertex program (gfx_nv2a.c). */
int gfx_clip_tri(const float w[3], float cw, int a[4], int b[4], float t[4])
{
    int n = 0, e;
    for (e = 0; e < 3; e++) {
        int z = (e + 1) % 3, ein = w[e] >= cw, zin = w[z] >= cw;
        if (ein) {
            a[n] = b[n] = e;
            t[n++] = 0;
        }
        if (ein != zin) {
            a[n] = e;
            b[n] = z;
            t[n++] = (cw - w[e]) / (w[z] - w[e]);
        }
    }
    return n;
}
