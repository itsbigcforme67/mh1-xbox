/* apx01 - Capcom APX texture file helpers 0x00217E50-0x002185xx (plAPX*, GetAPX*).
 * An APX image starts with a 0x20-byte header (docs/formats/graphics.md section 7): +4 pixel bytes,
 * +0xC bits per pixel (4/8/16/24/32), +0xE width, +0x10 height, +0x12 mip count, +0x14 palette bits per
 * entry (16/24/32), +0x16 palette count. The pixel data follows at +0x20, mip 0 first, then the palettes.
 * APXCTX (0x48 bytes per mip level or palette) is a texture description the caller passes to the
 * driver; field names are guesses from the values stored (bits, shift, mask per colour channel). */
#include "types.h"

#include "apx.h"

int plAPXGetMipmapTextureNum(void *img) {
    return GetAPXFileHeader(img)->mips;
}

int plAPXGetPaletteNum(void *img) {
    return GetAPXFileHeader(img)->palnum;
}

int plAPXSetContextFromImage(APXCTX *ctx, void *img) {
    int n = plAPXGetMipmapTextureNum(img);
    APXHDR *h = GetAPXFileHeader(img);
    int w = h->w;
    int ht = h->h;
    int i;

    for (i = 0; i < n; i++) {
        ctx->flags = 0;
        ctx->w = w;
        ctx->h = ht;
        switch (h->bpp) {
        case 4:
            ctx->flags |= 0x14;
            ctx->bytes = 0;
            ctx->pitch = ctx->w >> 1;
            ctx->c0.shift = 0;
            ctx->c0.bits = 0;
            ctx->c0.mask = 0;
            ctx->c1.shift = 0;
            ctx->c1.bits = 0;
            ctx->c1.mask = 0;
            ctx->c2.shift = 0;
            ctx->c2.bits = 0;
            ctx->c2.mask = 0;
            ctx->c3.shift = 0;
            ctx->c3.bits = 0;
            ctx->c3.mask = 0;
            break;
        case 8:
            ctx->flags |= 4;
            ctx->bytes = 1;
            ctx->pitch = ctx->bytes * ctx->w;
            ctx->c0.shift = 0;
            ctx->c0.bits = 0;
            ctx->c0.mask = 0;
            ctx->c1.shift = 0;
            ctx->c1.bits = 0;
            ctx->c1.mask = 0;
            ctx->c2.shift = 0;
            ctx->c2.bits = 0;
            ctx->c2.mask = 0;
            ctx->c3.shift = 0;
            ctx->c3.bits = 0;
            ctx->c3.mask = 0;
            break;
        case 16:
            ctx->bytes = 2;
            ctx->pitch = ctx->bytes * ctx->w;
            ctx->c0.bits = 5;
            ctx->c0.shift = 10;
            ctx->c0.mask = 0x1F;
            ctx->c1.bits = 5;
            ctx->c1.shift = 5;
            ctx->c1.mask = 0x1F;
            ctx->c2.bits = 5;
            ctx->c2.shift = 0;
            ctx->c2.mask = 0x1F;
            ctx->c3.bits = 1;
            ctx->c3.shift = 0xF;
            ctx->c3.mask = 1;
            ctx->c0.shift = 0;
            ctx->c2.shift = 10;
            break;
        case 24:
            ctx->bytes = 3;
            ctx->pitch = ctx->bytes * ctx->w;
            ctx->c0.bits = 8;
            ctx->c0.shift = 0x10;
            ctx->c0.mask = 0xFF;
            ctx->c1.bits = 8;
            ctx->c1.shift = 8;
            ctx->c1.mask = 0xFF;
            ctx->c2.bits = 8;
            ctx->c2.shift = 0;
            ctx->c2.mask = 0xFF;
            ctx->c3.bits = 0;
            ctx->c3.shift = 0;
            ctx->c3.mask = 0;
            ctx->c0.shift = 0;
            ctx->c2.shift = 0x10;
            break;
        case 32:
            ctx->bytes = 4;
            ctx->pitch = ctx->bytes * ctx->w;
            ctx->c0.bits = 8;
            ctx->c0.shift = 0x10;
            ctx->c0.mask = 0xFF;
            ctx->c1.bits = 8;
            ctx->c1.shift = 8;
            ctx->c1.mask = 0xFF;
            ctx->c2.bits = 8;
            ctx->c2.shift = 0;
            ctx->c2.mask = 0xFF;
            ctx->c3.bits = 8;
            ctx->c3.shift = 0x18;
            ctx->c3.mask = 0xFF;
            ctx->c0.shift = 0;
            ctx->c2.shift = 0x10;
            break;
        }
        ctx++;
        w >>= 1;
        ht >>= 1;
    }
    return 1;
}

int plAPXSetPaletteContextFromImage(APXCTX *ctx, void *img) {
    int n = plAPXGetPaletteNum(img);
    APXHDR *h = GetAPXFileHeader(img);
    int i;

    for (i = 0; i < n; i++) {
        ctx->flags = 0;
        switch (h->palbpp) {
        case 16:
            ctx->bytes = 2;
            ctx->c0.bits = 5;
            ctx->c0.shift = 10;
            ctx->c0.mask = 0x1F;
            ctx->c1.bits = 5;
            ctx->c1.shift = 5;
            ctx->c1.mask = 0x1F;
            ctx->c2.bits = 5;
            ctx->c2.shift = 0;
            ctx->c2.mask = 0x1F;
            ctx->c3.bits = 1;
            ctx->c3.shift = 0xF;
            ctx->c3.mask = 1;
            ctx->c0.shift = 0;
            ctx->c2.shift = 10;
            break;
        case 24:
            ctx->bytes = 3;
            ctx->c0.bits = 8;
            ctx->c0.shift = 0x10;
            ctx->c0.mask = 0xFF;
            ctx->c1.bits = 8;
            ctx->c1.shift = 8;
            ctx->c1.mask = 0xFF;
            ctx->c2.bits = 8;
            ctx->c2.shift = 0;
            ctx->c2.mask = 0xFF;
            ctx->c3.bits = 0;
            ctx->c3.shift = 0;
            ctx->c3.mask = 0;
            ctx->c0.shift = 0;
            ctx->c2.shift = 0x10;
            break;
        case 32:
            ctx->bytes = 4;
            ctx->c0.bits = 8;
            ctx->c0.shift = 0x10;
            ctx->c0.mask = 0xFF;
            ctx->c1.bits = 8;
            ctx->c1.shift = 8;
            ctx->c1.mask = 0xFF;
            ctx->c2.bits = 8;
            ctx->c2.shift = 0;
            ctx->c2.mask = 0xFF;
            ctx->c3.bits = 8;
            ctx->c3.shift = 0x18;
            ctx->c3.mask = 0xFF;
            ctx->c0.shift = 0;
            ctx->c2.shift = 0x10;
            break;
        default:
            return 0;
        }
        switch (h->bpp) {
        case 4:
            ctx->w = 0x10;
            ctx->h = 1;
            break;
        case 8:
            ctx->w = 0x100;
            ctx->h = 1;
            break;
        default:
            return 0;
        }
        ctx->pitch = ctx->bytes * ctx->w;
        ctx++;
    }
    return 1;
}

u8 *plAPXGetPixelAddressFromImage(void *img, int mip) {
    return GetAPXPixelMipmapAdrs(img, mip);
}

u8 *plAPXGetPaletteAddressFromImage(void *img, int idx) {
    return GetAPXPaletteAdrs(img, idx);
}

APXHDR *GetAPXFileHeader(void *img) {
    return (APXHDR *)img;
}

/* original bytes kept (does not match as C yet): build/raw/GetAPXPixelMipmapAdrs.inc, see config/c_rawfuncs.txt;
 * the C attempt is in apx_nm.c */
asm u8 *GetAPXPixelMipmapAdrs(void *img, int mip) {
#include "GetAPXPixelMipmapAdrs.inc"
}

/* original bytes kept (see above): build/raw/GetAPXPaletteAdrs.inc */
asm u8 *GetAPXPaletteAdrs(void *img, int idx) {
#include "GetAPXPaletteAdrs.inc"
}
