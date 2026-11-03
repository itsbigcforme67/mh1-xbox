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

static uint8_t *melt(const uint8_t *src, size_t srclen, size_t *outlen, int be)
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
                if (off >= cnt) {           /* source and destination apart: one copy */
                    memcpy(out + n, out + n - off * 2, cnt * 2);
                    n += cnt * 2;
                } else
                    for (k = 0; k < cnt; k++) {   /* overlapping: word by word, as the PS2 */
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

uint8_t *fmt_melt(const uint8_t *src, size_t srclen, size_t *outlen, int be)
{
    uint8_t *r;
    if (fmt_time_hook)
        fmt_time_hook(FMT_T_MELT, 1, 0);
    r = melt(src, srclen, outlen, be);
    if (fmt_time_hook)
        fmt_time_hook(FMT_T_MELT, 0, r ? (long)*outlen : 0);
    return r;
}

/* MH2's .tex packs (loading.tex, q_clear.tex, ...) are a link file with a header: after Meltw "MOMO", u32 count, then count x
 * {u32 offset, u32 size} of TIM2 pictures. Link files start with their count, which is never this magic */
static int link_momo(fmt_blob f)
{
    return f.n >= 8 && !memcmp(f.p, "MOMO", 4);
}

int fmt_link_count(fmt_blob f, int be)
{
    if (link_momo(f))
        return (int)fmt_u32(f.p + 4, be);
    return f.n >= 4 ? (int)fmt_u32(f.p, be) : 0;
}

fmt_blob fmt_link_entry(fmt_blob f, int i, int be)
{
    fmt_blob r = { NULL, 0 };
    uint32_t off, size;
    size_t base = link_momo(f) ? 8 : 4;
    if (i < 0 || i >= fmt_link_count(f, be) || base + 8 * (size_t)i + 8 > f.n)
        return r;
    off = fmt_u32(f.p + base + 8 * i, be);
    size = fmt_u32(f.p + base + 4 + 8 * i, be);
    if ((size_t)off + size > f.n)
        return r;
    r.p = f.p + off;
    r.n = size;
    return r;
}

/* ------------------------------------------------------------ MWo3 overlays
 * Monster Hunter G stores its code overlays packed (tools/mwo_unpack.py has the
 * format): little-endian 16-bit words; a flag word (MSB first) for up to 16
 * tokens; bit 0 = one literal word, bit 1 = back-reference, length = tok >> 11
 * words (0: the next word is the length), distance = tok & 0x7FF words;
 * distance 0 ends the stream. MH1 stores the module raw ("MWo3" at +0). */
uint8_t *fmt_mwo_unpack(uint8_t *src, size_t srclen, size_t *outlen)
{
    size_t i = 0, n = srclen / 2, o = 0, cap;
    uint16_t *out;
    if (!src)
        return NULL;
    if (srclen >= 4 && !memcmp(src, "MWo3", 4)) {
        *outlen = srclen;
        return src;
    }
    cap = srclen * 4 + 0x10000;
    out = malloc(cap * 2);
    if (!out)
        return NULL;
#define W(k) ((uint16_t)(src[2 * (k)] | src[2 * (k) + 1] << 8))
    while (i < n) {
        uint16_t flags = W(i);
        int bit;
        i++;
        for (bit = 15; bit >= 0; bit--) {
            uint16_t tok;
            size_t len, dist, k;
            if (i >= n)
                break;
            tok = W(i);
            i++;
            if (!(flags >> bit & 1)) {
                if (o >= cap)
                    goto bad;
                out[o++] = tok;
                continue;
            }
            len = tok >> 11;
            dist = tok & 0x7FF;
            if (len == 0) {
                if (i >= n)
                    goto bad;
                len = W(i);
                i++;
            }
            if (dist == 0) {
                uint16_t *fit;
                free(src);
                *outlen = o * 2;
                /* the buffer was sized for the worst case (8x the packed size); G's game.bin / lobby.bin
                 * stay loaded, and on the Xbox the unused tail cost ~6.5 MB (docs/xbox.md) */
                fit = realloc(out, o * 2 ? o * 2 : 2);
                return (uint8_t *)(fit ? fit : out);
            }
            if (dist > o || o + len > cap)
                goto bad;
            for (k = 0; k < len; k++, o++)
                out[o] = out[o - dist];
        }
    }
#undef W
bad:
    free(out);
    return NULL;
}
