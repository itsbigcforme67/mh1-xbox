/* Near-matches of apx01.c (not built): GetAPXPixelMipmapAdrs is 12 instructions off (the original compares
 * `mip >= count` into $at and keeps the walking pointer in t5, the counter in t2), GetAPXPaletteAdrs 50 off
 * (the original calls GetAPXFileHeader and GetAPXPixelMipmapAdrs without re-passing the image argument: a0 is
 * never saved across the first call; no source form found that does that). */
#include "types.h"
#include "apx.h"

u8 *GetAPXPixelMipmapAdrs(void *img, int mip) {
    APXHDR *h;
    u8 *p;
    int w;
    int ht;
    int i;
    int bpp;

    if (mip >= plAPXGetMipmapTextureNum(img)) {
        return 0;
    }
    h = GetAPXFileHeader(img);
    w = h->w;
    ht = h->h;
    p = (u8 *)h + 0x20;
    for (i = 0; i < mip; i++) {
        switch (h->bpp) {
        case 4:
            p += (w * ht) / 2;
            break;
        case 8:
            p += w * ht;
            break;
        case 16:
            p += w * ht * 2;
            break;
        case 24:
            p += w * ht * 3;
            break;
        case 32:
            p += w * ht * 4;
            break;
        }
        w >>= 1;
        ht >>= 1;
    }
    return p;
}

u8 *GetAPXPaletteAdrs(void *img, int idx) {
    APXHDR *h = GetAPXFileHeader(img);
    u8 *p = GetAPXPixelMipmapAdrs(img, 0) + h->pixbytes;
    int n;
    int i;

    switch (h->bpp) {
    case 4:
        n = 0x10;
        break;
    case 8:
        n = 0x100;
        break;
    }
    for (i = 0; i < idx; i++) {
        switch (h->palbpp) {
        case 16:
            p += n * 2;
            break;
        case 24:
            p += n * 3;
            break;
        case 32:
            p += n * 4;
            break;
        }
    }
    return p;
}
