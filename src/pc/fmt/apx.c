/*
 * apx.c - APX texture decoder (docs/formats/graphics.md 7;
 * flCreateTextureFromApx_mem 0x16FA00, plAPXSetContextFromImage 0x217E90).
 *
 * Header 0x20 bytes: u32 total, u32 pixel bytes, u32 palette bytes,
 * u16 bpp (4/8/16/24/32), u16 w, u16 h, u16 mips, u16 palette bpp
 * (16/24/32), u16 palette count. Pixels (linear, mip 0 first) at +0x20,
 * palette after the pixels in linear order. 4-bit: low nibble = left.
 * 32-bit colours are R,G,B,A bytes with alpha 0-255 on the PS2; on the
 * Wii each 32-bit palette word is byte-swapped (A,B,G,R in memory).
 */
#include "fmt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void col16(uint8_t *o, unsigned v)
{
    o[0] = (uint8_t)((v & 31) << 3);
    o[1] = (uint8_t)(((v >> 5) & 31) << 3);
    o[2] = (uint8_t)(((v >> 10) & 31) << 3);
    o[3] = (v & 0x8000) ? 255 : 0;
}

static void col32(uint8_t *o, const uint8_t *p, int be)
{
    uint32_t v = fmt_u32(p, be);   /* PS2: R | G<<8 | B<<16 | A<<24 */
    o[0] = (uint8_t)v;
    o[1] = (uint8_t)(v >> 8);
    o[2] = (uint8_t)(v >> 16);
    o[3] = (uint8_t)(v >> 24);
}

int fmt_apx_is_bare(fmt_blob f, int be)
{
    /* +0 of a bare APX is its total size; a link file starts with a count */
    if (f.n >= 0x40 && !memcmp(f.p, "TIM2", 4))
        return 1;
    return f.n >= 0x20 && fmt_u32(f.p, be) == f.n;
}

/* TIM2 (MH2's textures, Sony's format). Seen in DATA.BIN (docs/mh2_port.md): version 4, align 16 or 128, one picture, one
 * mip level, 4-bit, 8-bit or 32-bit images, palette 16 or 32 bit. Palette not flagged 0x80 is in the GS's CSM1 order. Alpha is full
 * range in the files (0-255, not the GS's 0-0x80): of a sample of 779 pictures none tops out at 0x80, 762 reach 0xFF. */
static int tim2_decode(apx_image *img, fmt_blob f)
{
    const uint8_t *h = f.p + (f.p[5] ? 0x80 : 0x10), *px, *pal;
    uint32_t cs, is, i, w, h2, npx, c32[256];
    unsigned hs, ncol, ct, it;
    if ((size_t)(h - f.p) + 0x30 > f.n)
        return -1;
    cs = fmt_u32(h + 4, FMT_LE);
    is = fmt_u32(h + 8, FMT_LE);
    hs = fmt_u16(h + 0xC, FMT_LE);
    ncol = fmt_u16(h + 0xE, FMT_LE);
    ct = h[0x12];
    it = h[0x13];
    w = fmt_u16(h + 0x14, FMT_LE);
    h2 = fmt_u16(h + 0x16, FMT_LE);
    px = h + hs;
    pal = px + is;
    if (!w || !h2 || w > 4096 || h2 > 4096 || (size_t)(pal - f.p) + cs > f.n)
        return -1;
    npx = w * h2;
    if (is < (it == 3 ? npx * 4 : it == 5 ? npx : it == 4 ? npx / 2 : 0xFFFFFFFFu))
        return -1;
    img->w = (int)w;
    img->h = (int)h2;
    img->rgba = malloc((size_t)npx * 4);
    img->src_bytes = (int)(is + cs);
    if (!img->rgba)
        return -1;
    if (it == 3) {
        for (i = 0; i < npx; i++) {
            col32(img->rgba + 4 * i, px + 4 * i, FMT_LE);
        }
        return 0;
    }
    memset(c32, 0, sizeof c32);
    for (i = 0; i < ncol && i < 256; i++) {
        uint8_t c[4];
        if ((ct & 0x7F) == 3)
            col32(c, pal + 4 * i, FMT_LE);
        else
            col16(c, fmt_u16(pal + 2 * i, FMT_LE));
        memcpy(&c32[i], c, 4);
    }
    if (it == 5 && !(ct & 0x80) && ncol == 256)     /* CSM1: blocks 8-15 and 16-23 of each 32 swapped */
        for (i = 0; i < 256; i += 32) {
            uint32_t t[8];
            memcpy(t, &c32[i + 8], 32);
            memcpy(&c32[i + 8], &c32[i + 16], 32);
            memcpy(&c32[i + 16], t, 32);
        }
    {
        uint32_t *o = (uint32_t *)img->rgba;
        if (it == 5)
            for (i = 0; i < npx; i++)
                o[i] = c32[px[i]];
        else
            for (i = 0; i < npx; i++) {
                unsigned b = px[i >> 1];
                o[i] = c32[i & 1 ? b >> 4 : b & 15];
            }
    }
    return 0;
}

static int apx_decode(apx_image *img, fmt_blob f, int be)
{
    uint32_t pix, i;
    unsigned bpp, pbpp, w, h;
    const uint8_t *px, *pal;
    uint8_t clut[256][4];

    memset(img, 0, sizeof *img);
    if (f.n >= 0x40 && !memcmp(f.p, "TIM2", 4))
        return tim2_decode(img, f);
    if (f.n < 0x20)
        return -1;
    pix = fmt_u32(f.p + 4, be);
    bpp = fmt_u16(f.p + 0xC, be);
    w = fmt_u16(f.p + 0xE, be);
    h = fmt_u16(f.p + 0x10, be);
    pbpp = fmt_u16(f.p + 0x14, be);
    px = f.p + 0x20;
    pal = px + pix;
    if (!w || !h || w > 4096 || h > 4096)
        return -1;
    img->w = (int)w;
    img->h = (int)h;
    img->rgba = malloc((size_t)w * h * 4);
    if (getenv("RT_APX_TRACE"))
        fprintf(stderr, "apx: %ux%u bpp %u mips %u pixel bytes %u\n", w, h, bpp, fmt_u16(f.p + 0x12, be), (unsigned)pix);
    img->src_bytes = (int)(w * h * bpp / 8) + (bpp == 4 ? 16 : bpp == 8 ? 256 : 0) * (int)pbpp / 8;

    if (bpp == 4 || bpp == 8) {
        unsigned n = bpp == 4 ? 16 : 256;
        if ((size_t)(pal - f.p) + n * (pbpp / 8) > f.n)
            goto bad;
        for (i = 0; i < n; i++) {
            if (pbpp == 32)
                col32(clut[i], pal + 4 * i, be);
            else if (pbpp == 24) {
                memcpy(clut[i], pal + 3 * i, 3);
                clut[i][3] = 255;
            } else
                col16(clut[i], fmt_u16(pal + 2 * i, be));
        }
        {   /* one 32-bit store per texel (memcpy of 4 bytes and a bpp test per texel were slow on the Xbox) */
            uint32_t c32[256], *o = (uint32_t *)img->rgba;
            uint32_t npx = w * h;
            memcpy(c32, clut, n * 4);
            if (bpp == 8)
                for (i = 0; i < npx; i++)
                    o[i] = c32[px[i]];
            else {
                for (i = 0; i + 1 < npx; i += 2) {
                    unsigned b = px[i >> 1];
                    o[i] = c32[b & 15];
                    o[i + 1] = c32[b >> 4];
                }
                if (i < npx)
                    o[i] = c32[px[i >> 1] & 15];
            }
        }
    } else if (bpp == 32) {
        for (i = 0; i < w * h; i++)
            col32(img->rgba + 4 * i, px + 4 * i, be);
    } else if (bpp == 24) {
        for (i = 0; i < w * h; i++) {
            memcpy(img->rgba + 4 * i, px + 3 * i, 3);
            img->rgba[4 * i + 3] = 255;
        }
    } else if (bpp == 16) {
        for (i = 0; i < w * h; i++)
            col16(img->rgba + 4 * i, fmt_u16(px + 2 * i, be));
    } else {
        goto bad;
    }
    return 0;
bad:
    free(img->rgba);
    img->rgba = NULL;
    return -1;
}

int fmt_apx_decode(apx_image *img, fmt_blob f, int be)
{
    int r;
    if (fmt_time_hook)
        fmt_time_hook(FMT_T_TEXDEC, 1, 0);
    r = apx_decode(img, f, be);
    if (fmt_time_hook)
        fmt_time_hook(FMT_T_TEXDEC, 0, r == 0 ? (long)img->w * img->h * 4 : 0);
    return r;
}
