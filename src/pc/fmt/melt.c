/*
 * melt.c - "Meltw" decompressor (main 0x11F230, docs/formats/graphics.md 2.1)
 * and link files (GetLinkFileAddress 0x11F320).
 *
 * The stream is 16-bit words. A flag word is consumed MSB first; flag 0 =
 * copy one literal word, flag 1 = token word w: count = w >> 11, offset =
 * w & 0x7FF, or (count 0) offset = w and count = next word. offset != 0:
 * copy count words from offset words back; offset 0 and count != 0: write
 * count zero words; both 0: end.
 */
#include "fmt.h"

#include <stdlib.h>
#include <string.h>

uint8_t *fmt_melt(const uint8_t *src, size_t srclen, size_t *outlen, int be)
{
    size_t cap = srclen * 4 + 64, n = 0, i = 0;
    uint8_t *out = malloc(cap);
    unsigned flags = 0, mask = 0;

    if (!out)
        return NULL;
    for (;;) {
        if (mask == 0) {
            if (i + 2 > srclen)
                goto bad;
            flags = fmt_u16(src + i, be);
            i += 2;
            mask = 0x8000;
        }
        if (flags & mask) {
            unsigned w, cnt, off, k;
            if (i + 2 > srclen)
                goto bad;
            w = fmt_u16(src + i, be);
            i += 2;
            cnt = w >> 11;
            if (cnt) {
                off = w & 0x7FF;
            } else {
                if (i + 2 > srclen)
                    goto bad;
                off = w;
                cnt = fmt_u16(src + i, be);
                i += 2;
            }
            if (!off && !cnt)
                break;
            if (n + cnt * 2 > cap) {
                while (n + cnt * 2 > cap)
                    cap *= 2;
                out = realloc(out, cap);
            }
            if (off) {
                if (off * 2 > n)
                    goto bad;
                for (k = 0; k < cnt; k++) {   /* may overlap */
                    out[n] = out[n - off * 2];
                    out[n + 1] = out[n + 1 - off * 2];
                    n += 2;
                }
            } else {
                memset(out + n, 0, cnt * 2);
                n += cnt * 2;
            }
        } else {
            if (i + 2 > srclen)
                goto bad;
            if (n + 2 > cap)
                out = realloc(out, cap *= 2);
            out[n++] = src[i++];
            out[n++] = src[i++];
        }
        mask >>= 1;
    }
    *outlen = n;
    {   /* the buffer grew in steps (4x the input first): give the rest back */
        uint8_t *fit = realloc(out, n ? n : 1);
        return fit ? fit : out;
    }
bad:
    free(out);
    return NULL;
}

int fmt_link_count(fmt_blob f, int be)
{
    return f.n >= 4 ? (int)fmt_u32(f.p, be) : 0;
}

fmt_blob fmt_link_entry(fmt_blob f, int i, int be)
{
    fmt_blob r = { NULL, 0 };
    uint32_t off, size;
    if (i < 0 || i >= fmt_link_count(f, be) || 4 + 8 * (size_t)i + 8 > f.n)
        return r;
    off = fmt_u32(f.p + 4 + 8 * i, be);
    size = fmt_u32(f.p + 8 + 8 * i, be);
    if ((size_t)off + size > f.n)
        return r;
    r.p = f.p + off;
    r.n = size;
    return r;
}
