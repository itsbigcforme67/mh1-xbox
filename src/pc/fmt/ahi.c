/*
 * ahi.c - AHI bone hierarchy (docs/formats/motion.md 1).
 *
 * Header {u32 flags, u32 chunk count, u32 size}, then chunks
 * {u32 type, u32 count, u32 size}: type 0 = tree list, 0x40000001 = bone:
 * +0 number, +4 parent, +8 child, +0xC sibling, +0x10 scale[4],
 * +0x20 rotation[4] (radians), +0x30 translation[4], +0x40 ?, +0x44 group.
 */
#include "fmt.h"

#include <stdlib.h>
#include <string.h>

int fmt_ahi_load(ahi_skel *s, fmt_blob f, int be)
{
    uint32_t n, i;
    size_t o = 12;
    int maxb = -1;

    memset(s, 0, sizeof *s);
    if (f.n < 12)
        return -1;
    n = fmt_u32(f.p + 4, be);
    s->bone = calloc(n + 1, sizeof(ahi_bone));
    for (i = 0; i < n && o + 12 <= f.n; i++) {
        uint32_t type = fmt_u32(f.p + o, be), size = fmt_u32(f.p + o + 8, be);
        if (size < 12 || o + size > f.n)
            break;
        if (type != 0 && size >= 12 + 0x48) {
            const uint8_t *p = f.p + o + 12;
            int b = fmt_s32(p, be), k;
            if (b >= 0 && b < (int)n) {
                ahi_bone *bn = &s->bone[b];
                bn->parent = fmt_s32(p + 4, be);
                bn->group = fmt_s32(p + 0x44, be);
                for (k = 0; k < 3; k++) {
                    bn->s[k] = fmt_f32(p + 0x10 + 4 * k, be);
                    bn->r[k] = fmt_f32(p + 0x20 + 4 * k, be);
                    bn->t[k] = fmt_f32(p + 0x30 + 4 * k, be);
                }
                if (b > maxb)
                    maxb = b;
            }
        }
        o += size;
    }
    s->nbone = maxb + 1;
    return s->nbone > 0 ? 0 : -1;
}

void fmt_ahi_free(ahi_skel *s)
{
    free(s->bone);
    memset(s, 0, sizeof *s);
}
