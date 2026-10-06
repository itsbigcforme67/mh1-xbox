/* pl library: TIM2/BMP image readers, pixel contexts, pixel plot and colour helpers (plTIM2*, plDrawPixel, plGetColor, plConvertContext, plReport).
 * SLPM_654.95 0x001935C0-0x00194890 as one translation unit (the Tim2 helpers are file-static in the original). A pixel context (PLCTX) describes a
 * raster: flags (4 = palettised, 0x10 / 0x40 = nibble order / raw), width, height, stride, base, bytes per pixel and four channel descriptors
 * (bits, shift, mask) for R, G, B, A. */
#include "types.h"
#include "va.h"

typedef struct PLCH { int bits; int shift; int mask; } PLCH;
typedef struct PLCTX {
    u32 flags;     /* 0x00 */
    int w;         /* 0x04 */
    int h;         /* 0x08 */
    int stride;    /* 0x0C */
    int base;      /* 0x10 */
    int bpp;       /* 0x14 */
    PLCH c[4];     /* 0x18 R, 0x24 G, 0x30 B, 0x3C A */
} PLCTX;

typedef struct PLPIX { s16 x; s16 y; u32 color; } PLPIX;

extern char plReportMessage[];
int vsprintf(char *, const char *, va_list);
int plReport(char *fmt, ...);
extern char lit_110_0035C150[];
int plCalcAddress(int x, int y, PLCTX *c);
int plDrawPixel(PLCTX *c, PLPIX *p);
int plDrawPixel_3(PLCTX *c, int x, int y, u32 color);
u32 plGetColor(int x, int y, PLCTX *c);

static u32 InputTim2AlignRegulation(u8 *p, u32 size);
static u8 *GetTim2PictureHead(u8 *p, int n);
static u8 *GetTim2PictureData(u8 *p, int a, int mip);
static u8 *GetTim2ClutData(u8 *p, int mip);
static int CheckTIM2FileHeader(u8 *p);

int plTIM2GetMipmapTextureNum(u8 *p) {
    u8 *h;

    if (CheckTIM2FileHeader(p) == 0) {
        return 0;
    }
    h = GetTim2PictureHead(p, 0);
    return (h[0x11] - 1) & 0xFF;
}

int plTIM2SetContextFromImage(PLCTX *c, u8 *img) {
    u8 *h;
    int i;
    int w;
    int ht;

    if (CheckTIM2FileHeader(img) == 0) {
        return 0;
    }
    h = GetTim2PictureHead(img, 0);
    if (h[0x11] > 7) {
        return 0;
    }
    w = *(u16 *)(h + 0x14);
    ht = *(u16 *)(h + 0x16);
    for (i = 0; i < h[0x11]; i++) {
        c->flags = 0;
        c->w = w;
        c->h = ht;
        switch (h[0x13]) {
        case 4:
            c->flags |= 0x14;
            c->bpp = 0;
            c->stride = c->w >> 1;
            c->c[0].shift = 0;
            c->c[0].bits = 0;
            c->c[0].mask = 0;
            c->c[1].shift = 0;
            c->c[1].bits = 0;
            c->c[1].mask = 0;
            c->c[2].shift = 0;
            c->c[2].bits = 0;
            c->c[2].mask = 0;
            c->c[3].shift = 0;
            c->c[3].bits = 0;
            c->c[3].mask = 0;
            break;
        case 5:
            c->flags |= 4;
            c->bpp = 1;
            c->stride = c->bpp * c->w;
            c->c[0].shift = 0;
            c->c[0].bits = 0;
            c->c[0].mask = 0;
            c->c[1].shift = 0;
            c->c[1].bits = 0;
            c->c[1].mask = 0;
            c->c[2].shift = 0;
            c->c[2].bits = 0;
            c->c[2].mask = 0;
            c->c[3].shift = 0;
            c->c[3].bits = 0;
            c->c[3].mask = 0;
            break;
        case 1:
            c->bpp = 2;
            c->stride = c->bpp * c->w;
            c->c[0].bits = 5;
            c->c[0].shift = 10;
            c->c[0].mask = 0x1F;
            c->c[1].bits = 5;
            c->c[1].shift = 5;
            c->c[1].mask = 0x1F;
            c->c[2].bits = 5;
            c->c[2].shift = 0;
            c->c[2].mask = 0x1F;
            c->c[3].bits = 1;
            c->c[3].shift = 15;
            c->c[3].mask = 1;
            c->c[0].shift = 0;
            c->c[2].shift = 10;
            break;
        case 2:
            c->bpp = 3;
            c->stride = c->bpp * c->w;
            c->c[0].bits = 8;
            c->c[0].shift = 16;
            c->c[0].mask = 0xFF;
            c->c[1].bits = 8;
            c->c[1].shift = 8;
            c->c[1].mask = 0xFF;
            c->c[2].bits = 8;
            c->c[2].shift = 0;
            c->c[2].mask = 0xFF;
            c->c[3].bits = 0;
            c->c[3].shift = 0;
            c->c[3].mask = 0;
            c->c[0].shift = 0;
            c->c[2].shift = 16;
            break;
        case 3:
            c->bpp = 4;
            c->stride = c->bpp * c->w;
            c->c[0].bits = 8;
            c->c[0].shift = 16;
            c->c[0].mask = 0xFF;
            c->c[1].bits = 8;
            c->c[1].shift = 8;
            c->c[1].mask = 0xFF;
            c->c[2].bits = 8;
            c->c[2].shift = 0;
            c->c[2].mask = 0xFF;
            c->c[3].bits = 8;
            c->c[3].shift = 24;
            c->c[3].mask = 0xFF;
            c->c[0].shift = 0;
            c->c[2].shift = 16;
            break;
        }
        c++;
        w >>= 1;
        ht >>= 1;
    }
    return 1;
}

int plTIM2SetPaletteContextFromImage(PLCTX *c, u8 *img) {
    u8 *h;

    if (CheckTIM2FileHeader(img) == 0) {
        return 0;
    }
    h = GetTim2PictureHead(img, 0);
    if (h[0x11] > 7) {
        return 0;
    }
    c->flags = 0;
    switch (h[0x12]) {
    case 1:
        c->bpp = 2;
        c->c[0].bits = 5;
        c->c[0].shift = 10;
        c->c[0].mask = 0x1F;
        c->c[1].bits = 5;
        c->c[1].shift = 5;
        c->c[1].mask = 0x1F;
        c->c[2].bits = 5;
        c->c[2].shift = 0;
        c->c[2].mask = 0x1F;
        c->c[3].bits = 1;
        c->c[3].shift = 15;
        c->c[3].mask = 1;
        c->c[0].shift = 0;
        c->c[2].shift = 10;
        break;
    case 2:
        c->bpp = 3;
        c->c[0].bits = 8;
        c->c[0].shift = 16;
        c->c[0].mask = 0xFF;
        c->c[1].bits = 8;
        c->c[1].shift = 8;
        c->c[1].mask = 0xFF;
        c->c[2].bits = 8;
        c->c[2].shift = 0;
        c->c[2].mask = 0xFF;
        c->c[3].bits = 0;
        c->c[3].shift = 0;
        c->c[3].mask = 0;
        c->c[0].shift = 0;
        c->c[2].shift = 16;
        break;
    case 3:
        c->bpp = 4;
        c->c[0].bits = 8;
        c->c[0].shift = 16;
        c->c[0].mask = 0xFF;
        c->c[1].bits = 8;
        c->c[1].shift = 8;
        c->c[1].mask = 0xFF;
        c->c[2].bits = 8;
        c->c[2].shift = 0;
        c->c[2].mask = 0xFF;
        c->c[3].bits = 8;
        c->c[3].shift = 24;
        c->c[3].mask = 0xFF;
        c->c[0].shift = 0;
        c->c[2].shift = 16;
        break;
    default:
        return 0;
    }
    switch (h[0x13]) {
    case 4:
        if (*(u16 *)(h + 0xE) != 0x10) {
            return 0;
        }
        c->w = 0x10;
        c->h = 1;
        break;
    case 5:
        if (*(u16 *)(h + 0xE) != 0x100) {
            return 0;
        }
        c->w = 0x100;
        c->h = 1;
        break;
    default:
        return 0;
    }
    c->stride = c->bpp * c->w;
    return 1;
}

u8 *plTIM2GetPixelAddressFromImage(u8 *p, int mip) {
    return GetTim2PictureData(p, 0, mip);
}

u8 *plTIM2GetPaletteAddressFromImage(u8 *p) {
    return GetTim2ClutData(p, 0);
}






/* original bytes: build/raw/CheckTIM2FileHeader.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
static asm int CheckTIM2FileHeader(u8 *p)
{
#include "CheckTIM2FileHeader.inc"
}
#else
static int CheckTIM2FileHeader(u8 *p) {
    if (p[0] != 0x54 || p[1] != 0x49 || p[2] != 0x4D || p[3] != 0x32) {
        int r;
        if (p[0] == 0x43 && p[1] == 0x4C && p[2] == 0x54) {
            r = 0;
        } else {
            r = 0;
        }
        return r;
    }
    switch (p[4]) {
    case 4:
        if (p[5] >= 2) {
    default:
            return 0;
        }
    case 3:
        {
            int r = 1;
            if (*(u16 *)(p + 6) != 1) r = 0;
            return r;
        }
    }
}
#endif

static u32 InputTim2AlignRegulation(u8 *p, u32 size) {
    u32 a = p[5] == 0 ? 0x10 : 0x80;

    if (size % a != 0) {
        size += a - (size - a * (size / a)) % a;
    }
    return size;
}

/* original bytes: build/raw/GetTim2PictureHead.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
static asm u8 * GetTim2PictureHead(u8 *q, int n)
{
#include "GetTim2PictureHead.inc"
}
#else
static u8 *GetTim2PictureHead(u8 *q, int n) {
    u8 *p = q;
    u8 *r;
    int i;

    q += InputTim2AlignRegulation(q, 0x10);
    r = q;
    if (n > 0) {
        for (i = 0; i < *(u16 *)(p + 6); ) {
            q += InputTim2AlignRegulation(p, 0x30);
            q += *(u32 *)r;
            i++;
            r = q;
            if (!(i < n)) break;
        }
    }
    return r;
}
#endif

/* original bytes: build/raw/GetTim2PictureData.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
static asm u8 * GetTim2PictureData(u8 *p, int a, int mip)
{
#include "GetTim2PictureData.inc"
}
#else
static u8 *GetTim2PictureData(u8 *p, int a, int mip) {
    u8 *q;
    u8 *h;
    int m;
    int sz;
    int i;
    int acc;
    u32 *arr[8];
    q = GetTim2PictureHead(p, 0);
    h = q;
    m = h[0x11];
    sz = 0x30;
    if (h[0x11] > 1) {
        for (i = 0; i < m - 1; i++) {
            arr[i] = (u32 *)(h + 0x40 + i * 4);
        }
        sz += (h[0x11] < 5) ? 0x20 : 0x30;
    }
    q += InputTim2AlignRegulation(p, sz);
    if (h[0x11] > 1) {
        acc = 0;
        if (mip > 0) {
            for (i = 0; i < mip; i++) {
                acc += InputTim2AlignRegulation(p, *arr[i]);
            }
        }
        q += acc;
    }
    return q;
}
#endif

/* original bytes: build/raw/GetTim2ClutData.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
static asm u8 * GetTim2ClutData(u8 *p, int mip)
{
#include "GetTim2ClutData.inc"
}
#else
static u8 *GetTim2ClutData(u8 *p, int mip) {
    u8 *q = GetTim2PictureHead(p, 0);
    u8 *h = q;
    int a = 0x30;
    int m = h[0x11];

    if (m > 1) {
        a += (m < 5) ? 0x20 : 0x30;
    }
    q += InputTim2AlignRegulation(p, a);
    if (*(u32 *)(h + 4) != 0) {
        return q + *(u32 *)(h + 8);
    }
    return 0;
}
#endif

int plReport(char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vsprintf(plReportMessage, fmt, ap);
    return 1;
}


void plMemset(s8 *d, s8 v, int n) {
    int i;

    for (i = 0; i < n; i++) {
        *d++ = v;
    }
}

/* original bytes: build/raw/plMemmove.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm void plMemmove(s8 *dst, s8 *src, int n)
{
#include "plMemmove.inc"
}
#else
void plMemmove(s8 *dst, s8 *src, int n) {
    int i;

    if ((u32)src < (u32)dst && (u32)dst < (u32)src + n) {
        int last = n - 1;
        i = 0;
        dst += last;
        src += last;
        for (; i < n; i++) {
            *dst-- = *src--;
        }
    } else {
        for (i = 0; i < n; i++) {
            *dst++ = *src++;
        }
    }
}
#endif

/* original bytes: build/raw/plCalcAddress.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int plCalcAddress(int x, int y, PLCTX *c)
{
#include "plCalcAddress.inc"
}
#else
int plCalcAddress(int x, int y, PLCTX *c) {
    if (x < 0 || y < 0 || x >= c->w || y >= c->h) {
        return 0;
    }
    if (c->bpp == 0) {
        if (c->flags & 0x40) {
            return c->base + c->stride * y + x;
        }
        return c->base + c->stride * y + x / 2;
    }
    return c->base + c->stride * y + x * c->bpp;
}
#endif

int plDrawPixel(PLCTX *c, PLPIX *p) {
    u8 *a;
    u32 col;
    u32 cc;
    u32 old;

    a = (u8 *)plCalcAddress(p->x, p->y, c);
    if (a == 0) {
        return 0;
    }
    if (c->flags & 4) {
        switch (c->bpp) {
        case 4:
            *(u32 *)a = p->color;
            break;
        case 2:
            *(u16 *)a = p->color;
            break;
        case 1:
            *a = p->color;
            break;
        case 0:
            if (c->flags & 0x40) {
                *a = p->color;
            } else {
                old = *a;
                cc = p->color;
                if (((p->x & 1) ^ ((c->flags & 0x10) != 0)) != 0) {
                    old &= 0xF0;
                    cc &= 0xF;
                } else {
                    old &= 0xF;
                    cc = (cc & 0xF) << 4;
                }
                *a = old | cc;
            }
            break;
        }
    } else {
        int r = (p->color >> 16) & 0xFF;
        int g = (p->color >> 8) & 0xFF;
        int b = p->color & 0xFF;
        int al = (p->color >> 24) & 0xFF;

        col = ((al * c->c[3].mask / 255) << c->c[3].shift)
            | (((b * c->c[2].mask / 255) << c->c[2].shift)
            | (((r * c->c[0].mask / 255) << c->c[0].shift)
            | ((g * c->c[1].mask / 255) << c->c[1].shift)));
        switch (c->bpp) {
        case 2:
            *(u16 *)a = col;
            break;
        case 3:
            a[0] = col;
            a[1] = col >> 8;
            a[2] = col >> 16;
            break;
        case 4:
            *(u32 *)a = col;
            break;
        }
    }
    return 1;
}

int plDrawPixel_3(PLCTX *c, int x, int y, u32 color) {
    PLPIX pix;

    pix.x = x;
    pix.y = y;
    pix.color = color;
    return plDrawPixel(c, &pix);
}

u32 plGetColor(int x, int y, PLCTX *c) {
    u32 r;
    u8 *a;
    u32 v;
    u32 n;
    u32 g;
    u32 b;
    u32 al;

    a = (u8 *)plCalcAddress(x, y, c);
    if (a == 0) {
        return 0;
    }
    if (c->flags & 4) {
        switch (c->bpp) {
        case 0:
            if (c->flags & 0x40) {
                v = *a;
            } else {
                n = *a;
                if ((c->flags & 0x10 ? 1 : 0) ^ (x & 1)) {
                    v = n & 0xF;
                } else {
                    v = (n >> 4) & 0xF;
                }
            }
            break;
        case 1:
            v = *a;
            break;
        case 2:
            v = *(u16 *)a;
            break;
        case 4:
            v = *(u32 *)a;
            break;
        }
        return v;
    }
    switch (c->bpp) {
    case 2:
        v = *(u16 *)a;
        break;
    case 3:
        v = a[0] | ((a[2] << 16) | (a[1] << 8));
        break;
    case 4:
        v = *(u32 *)a;
        break;
    }
    if (c->c[3].bits != 0) {
        al = (c->c[3].mask & (v >> c->c[3].shift)) * 0xFF / c->c[3].mask;
    } else {
        al = 0xFF;
    }
    if (c->c[0].bits != 0) {
        r = (c->c[0].mask & (v >> c->c[0].shift)) * 0xFF / c->c[0].mask;
    } else {
        r = 0;
    }
    if (c->c[1].bits != 0) {
        g = (c->c[1].mask & (v >> c->c[1].shift)) * 0xFF / c->c[1].mask;
    } else {
        g = 0;
    }
    if (c->c[2].bits != 0) {
        b = (c->c[2].mask & (v >> c->c[2].shift)) * 0xFF / c->c[2].mask;
    } else {
        b = 0;
    }
    return b | ((g << 8) | ((al << 24) | (r << 16)));
}

int plConvertContext(PLCTX *dst, PLCTX *src) {
    int x;
    int y;

    for (y = 0; y < dst->h; y++) {
        for (x = 0; x < dst->w; x++) {
            plDrawPixel_3(dst, x, y, plGetColor(x, y, src));
        }
    }
    return 1;
}
