/* fl texture handle creation. SLPM_654.95 0x00187D10-0x00188420: flCreateTextureHandle, flPS2GetTextureInfoFromContext, flPS2CreateTextureHandle,
 * flPS2VramTrans (file-static), flPS2GetVramTransAdrs. A texture handle (TEXH, 0x38 bytes) lives in the table flTexture; its mip levels are
 * uploaded to the GS by queueing one load image packet per level. */
#include "types.h"

typedef struct TEXH {
    int x0;             /* 0x00 */
    int x4;             /* 0x04 */
    int size;           /* 0x08 total byte size */
    int x0C;            /* 0x0C */
    s16 x10, x12, x14, x16, x18;
    s16 w;              /* 0x1A */
    s16 h;              /* 0x1C */
    u8 _pad1E[2];
    int pix;            /* 0x20 pixel storage format */
    int sysmem;         /* 0x24 system memory handle of the pixel data */
    s16 x28;            /* 0x28 */
    s16 x2A;            /* 0x2A */
    int x2C;            /* 0x2C */
    u32 mips;           /* 0x30 */
    s8 used;            /* 0x34 */
    s8 resident;        /* 0x35 */
    s8 x36;             /* 0x36 */
    s8 x37;             /* 0x37 */
} TEXH;

typedef struct TEXCTX {
    int x0;             /* 0x00 */
    int w;              /* 0x04 */
    int h;              /* 0x08 */
    int x0C;            /* 0x0C */
    int cache;          /* 0x10 */
    int fmt;            /* 0x14 */
} TEXCTX;

extern TEXH flTexture[0x100];
extern TEXH flPalette[0x100];
extern int flPTNum;
extern int flCTNum;
extern u8 flPs2VIF1Control[];
extern char lit_401_0035BEB0[], lit_402_0035BEF0[], lit_554_0035BF30[], lit_555_0035BF70[];

int flPS2GetTextureHandle(void);

int flPS2GetTextureInfoFromContext(TEXCTX *c, int a, int h, int b);
int flPS2GetVramFreeArea();
int flPS2GetSystemMemoryHandle(int size, int a);
int flPS2GetSystemBuffAdrs(int h);
void flPS2ConvertTextureFromContext(TEXCTX *c, TEXH *t, int a);
int flPS2CreateTextureHandle(int h, int mode);
static void flPS2VramTrans(TEXH *t);
int flPS2VIF1CalcEndLoadImageSize(int size);
int flPS2VIF1CalcLoadImageSize(int size);
long flPS2GetSystemTmpBuff(int size, int align);
void flPS2VIF1MakeEndLoadImage(long p, int a);
long flPS2VIF1MakeLoadImage(long p, int a, int adrs, int size, long vram, long bw, long fmt, long z, long a0, long w, long h);
int flPS2DmaAddQueue2(int mode, unsigned long tag, long x, void *c);
int flPS2GetTextureBuffWidth();
int flPS2GetTextureVramBlock(TEXH *t);
int flPS2GetTextureSize(int pix, int w, int h, int mips);
void flLogOut();
int flPS2GetVramTransAdrs(TEXH *t, u32 level);
int flPS2GetPaletteHandle(void);
int flPS2GetPaletteInfoFromContext();
int flPS2CreatePaletteHandle(int h, int mode);
int flPS2GetPaletteVramBlock(TEXH *t);

int flCreateTextureHandle(TEXCTX *c, int b) {
    int h;
    TEXH *t;

    h = flPS2GetTextureHandle();
    if (h == 0) {
        return 0;
    }
    t = &flTexture[(h & 0xFFFF) - 1];
    flPS2GetTextureInfoFromContext(c, 1, h, b);
    if (c->cache == 0) {
        flPS2GetVramFreeArea(&h, 1);
        t->sysmem = flPS2GetSystemMemoryHandle(t->size, 2);
        flCTNum++;
    } else {
        flPS2ConvertTextureFromContext(c, t, 0);
        flPS2CreateTextureHandle(h, b);
    }
    return h;
}

/* original bytes: build/raw/flPS2GetTextureInfoFromContext.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int flPS2GetTextureInfoFromContext(TEXCTX *c, int a, int h, int b)
{
#include "flPS2GetTextureInfoFromContext.inc"
}
#else
int flPS2GetTextureInfoFromContext(TEXCTX *c, int a, int h, int b) {
    TEXH *t = &flTexture[(h & 0xFFFF) - 1];
    int n;

    t->used = 1;
    t->x0 = b;
    t->x4 = c->x0;
    t->w = c->w;
    t->h = c->h;
    t->x36 = 0;
    t->sysmem = 0;
    t->x2C = 0;
    t->x37 = 0;
    t->mips = a;
    switch (c->fmt) {
    case 0:
        t->pix = 0x14;
        t->x0C = 0;
        break;
    case 1:
        t->pix = 0x13;
        t->x0C = 1;
        break;
    case 2:
        t->pix = 2;
        t->x0C = 2;
        break;
    case 3:
        t->pix = 1;
        t->x0C = 2;
        break;
    case 4:
        t->pix = 0;
        t->x0C = 4;
        break;
    }
    n = c->w;
    switch (n) {
    case 0x400:
    case 0x200:
    case 0x100:
    case 0x80:
    case 0x40:
    case 0x20:
        break;
    default:
        flLogOut(lit_401_0035BEB0, n);
        return 0;
    }
    t->x16 = flPS2GetTextureBuffWidth(t->w, n);
    n = c->h;
    switch (n) {
    case 0x400:
    case 0x200:
    case 0x100:
    case 0x80:
    case 0x40:
    case 0x20:
        break;
    default:
        flLogOut(lit_402_0035BEF0, n);
        return 0;
    }
    t->x18 = flPS2GetTextureBuffWidth(t->h, n);
    t->x10 = flPS2GetTextureVramBlock(t);
    t->x12 = 0x20;
    t->size = flPS2GetTextureSize(t->pix, t->w, t->h, t->mips);
    return 1;
}
#endif

int flPS2CreateTextureHandle(int h, int mode) {
    TEXH *t = &flTexture[(h & 0xFFFF) - 1];
    long p;

    for (;;) {
        switch (mode) {
        case 1:
        case 4:
            goto done;
        default:
        case 5:
        case 2:
        case 3:
            if (flPS2GetVramFreeArea(&h, 1) == 0) {
                t->x0 = 4;
                mode = 4;
                continue;
            }
            flPS2VramTrans(t);
            p = flPS2GetSystemTmpBuff(flPS2VIF1CalcEndLoadImageSize(t->size), 0x10);
            flPS2VIF1MakeEndLoadImage(p, 1);
            flPS2DmaAddQueue2(0, (unsigned long)p & 0xFFFFFFFUL, p, flPs2VIF1Control);
            goto done;
        }
    }
done:
    flCTNum++;
    return 1;
}

static void flPS2VramTrans(TEXH *t) {
    u8 *adr;
    u32 i;
    int w;
    int h;
    int sz;
    long tmp;
    int v;
    long r;

    adr = (u8 *)flPS2GetSystemBuffAdrs(t->sysmem);
    w = t->w;
    h = t->h;
    for (i = 0; i < t->mips; i++) {
        switch (t->pix) {
        case 0x14:
            sz = (w * h) >> 1;
            break;
        case 0x13:
            sz = w * h;
            break;
        case 2:
            sz = w * h * 2;
            break;
        case 1:
            sz = w * h * 4;
            break;
        case 0:
            sz = w * h * 4;
            break;
        }
        tmp = flPS2GetSystemTmpBuff(flPS2VIF1CalcLoadImageSize(sz), 0x10);
        v = flPS2GetVramTransAdrs(t, i);
        if (t->x36 == 0) {
            r = flPS2VIF1MakeLoadImage(tmp, 1, (int)adr, sz, (s16)v, (s16)(w / 64), (s16)t->pix, 0, 0, (s16)w, (s16)h);
        } else {
            r = flPS2VIF1MakeLoadImage(tmp, 1, (int)adr, sz, (s16)v, (s16)(t->x28 / 64), 0, 0, 0, t->x28, t->x2A);
        }
        flPS2DmaAddQueue2(0, (unsigned long)tmp & 0xFFFFFFFUL | 0x10000000, r, flPs2VIF1Control);
        w >>= 1;
        h >>= 1;
        adr += sz;
    }
}

/* original bytes: build/raw/flPS2GetVramTransAdrs.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int flPS2GetVramTransAdrs(TEXH *t, u32 level)
{
#include "flPS2GetVramTransAdrs.inc"
}
#else
int flPS2GetVramTransAdrs(TEXH *t, u32 level) {
    int sz;
    int base;
    int i;
    int w;
    int h;

    if (level >= t->mips) {
        return -1;
    }
    w = t->w;
    h = t->h;
    base = t->x14;
    i = 0;
    if (0 < (int)level) {
        do {
            switch (t->pix) {
            case 0x14:
                sz = (w * h) >> 1;
                break;
            case 0x13:
                sz = w * h;
                break;
            case 2:
                sz = w * h * 2;
                break;
            case 1:
                sz = w * h * 4;
                break;
            case 0:
                sz = w * h * 4;
                break;
            default:
                return -1;
            }
            base += (sz + 0xFF) / 256;
            i++;
            w >>= 1;
            h >>= 1;
        } while (i < (int)level);
    }
    return base;
}
#endif

int flPS2GetTextureHandle(void) {
    int i;
    TEXH *t = flTexture;

    for (i = 0; i < 0x100; i++) {
        if (t->used == 0) {
            break;
        }
        t++;
    }
    return i + 1;
}

u32 flCreatePaletteHandle(TEXCTX *c, int b) {
    u32 h;
    int i;
    TEXH *t;

    h = flPS2GetPaletteHandle();
    if (h == 0) {
        return 0;
    }
    i = ((h & 0xFFFF0000) >> 16) - 1;
    t = &flPalette[i];
    flPS2GetPaletteInfoFromContext(c, h, b, i);
    if (c->cache == 0) {
        flPS2GetVramFreeArea(&h, 1);
        t->sysmem = flPS2GetSystemMemoryHandle(t->size, 2);
        flPTNum++;
    } else {
        if (c->w == 0x100) {
            flPS2ConvertTextureFromContext(c, t, 1);
        } else {
            flPS2ConvertTextureFromContext(c, t, 0);
        }
        flPS2CreatePaletteHandle(h, b);
    }
    return h >> 16;
}

/* original bytes: build/raw/flPS2GetPaletteInfoFromContext.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int flPS2GetPaletteInfoFromContext(TEXCTX *c, u32 h, int b)
{
#include "flPS2GetPaletteInfoFromContext.inc"
}
#else
int flPS2GetPaletteInfoFromContext(TEXCTX *c, u32 h, int b) {
    TEXH *t = &flPalette[((h & 0xFFFF0000) >> 16) - 1];

    if (c->h != 1) {
        flLogOut(lit_554_0035BF30, 1);
        return 0;
    }
    switch (c->fmt) {
    case 2:
        t->pix = 2;
        t->x0C = 2;
        break;
    case 3:
        t->pix = 1;
        t->x0C = 3;
        break;
    case 4:
        t->pix = 0;
        t->x0C = 4;
        break;
    default:
        flLogOut(lit_555_0035BF70, 1);
        return 0;
    }
    if (c->w == 0x100) {
        t->w = 0x10;
        t->h = 0x10;
    } else {
        t->w = 8;
        t->h = 2;
    }
    t->x4 = c->x0;
    t->x0 = b;
    t->used = 1;
    t->x16 = flPS2GetTextureBuffWidth(t->w, 1);
    t->x18 = flPS2GetTextureBuffWidth(t->h);
    t->x36 = 0;
    t->sysmem = 0;
    t->x2C = 0;
    t->x37 = 0;
    t->mips = 1;
    t->x10 = flPS2GetPaletteVramBlock(t);
    t->x12 = 1;
    t->size = flPS2GetTextureSize(t->pix, t->w, t->h, t->mips);
    return 1;
}
#endif

int flPS2CreatePaletteHandle(int h, int mode) {
    TEXH *t = &flPalette[(((u32)h & 0xFFFF0000) >> 16) - 1];
    long p;

    for (;;) {
        switch (mode) {
        case 1:
        case 4:
            goto done;
        default:
        case 5:
        case 2:
        case 3:
            if (flPS2GetVramFreeArea(&h, 1) == 0) {
                t->x0 = 4;
                mode = 4;
                continue;
            }
            flPS2VramTrans(t);
            p = flPS2GetSystemTmpBuff(flPS2VIF1CalcEndLoadImageSize(t->size), 0x10);
            flPS2VIF1MakeEndLoadImage(p, 1);
            flPS2DmaAddQueue2(0, (unsigned long)p & 0xFFFFFFFUL, p, flPs2VIF1Control);
            goto done;
        }
    }
done:
    flPTNum++;
    return 1;
}
