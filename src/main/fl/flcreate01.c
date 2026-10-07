/* fl texture creation from in-memory images (SLPM_654.95 0x0016FA00-0x00170198): flCreateTextureFromApx_mem / flCreateTextureFromTim2_mem allocate a
 * texture handle, read the image's pixel contexts (one per mip level) with the pl library, copy / convert every level into the texture's system buffer
 * (4/8 bit palettised data may be widened by flPS2Conv4_8_32, 32 bit data gets its alpha halved) and, for palettised formats (0x13 / 0x14), create the
 * palette handle the same way. Returns the texture handle | palette handle (0 on failure). Field names are guesses. */
#include "types.h"

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

extern TEXH flTexture[0x100];
extern TEXH flPalette[0x100];

int flPS2GetTextureHandle(void);
int flPS2GetPaletteHandle(void);
int flPS2GetSystemMemoryHandle(int size, int a);
int flPS2GetSystemBuffAdrs(int h);
int flPS2GetTextureInfoFromContext();
int flPS2GetPaletteInfoFromContext();
int flPS2CreateTextureHandle(int h, int mode);
int flPS2CreatePaletteHandle(int h, int mode);
void flMemcpy();
void flPS2Conv4_8_32();
void flPS2ConvertAlpha();
int flPS2ConvertContext();
int plAPXGetMipmapTextureNum();
int plAPXSetContextFromImage();
int plAPXSetPaletteContextFromImage();
int plAPXGetPixelAddressFromImage();
int plAPXGetPaletteAddressFromImage();
int plTIM2GetMipmapTextureNum();
int plTIM2SetContextFromImage();
int plTIM2SetPaletteContextFromImage();
int plTIM2GetPixelAddressFromImage();
int plTIM2GetPaletteAddressFromImage();

int flCreateTextureFromApx_mem(u8 *img, int arg) {
    int h;
    int ph;
    int mips;
    TEXH *t;
    TEXH *p;
    PLCTX ctx[7];
    PLCTX pctx;
    PLCTX sctx;
    u8 *dst;
    u8 *pd;
    u8 *c;
    int i;
    int w;
    int ht;
    int size;
    int adr;

    ph = 0;
    h = flPS2GetTextureHandle();
    t = &flTexture[(h & 0xFFFF) - 1];
    mips = plAPXGetMipmapTextureNum(img, (h & 0xFFFF) - 1) - 1;
    if (plAPXSetContextFromImage(ctx, img) == 0) {
        return 0;
    }
    flPS2GetTextureInfoFromContext(ctx, mips + 1, h, arg);
    t->sysmem = flPS2GetSystemMemoryHandle(t->size, 2);
    dst = (u8 *)flPS2GetSystemBuffAdrs(t->sysmem);
    w = t->w;
    ht = t->h;
    i = 0;
    if (0 <= mips) {
        c = (u8 *)ctx;
        do {
            switch (((PLCTX *)c)->bpp) {
            case 0:
                size = (w * ht) >> 1;
                adr = plAPXGetPixelAddressFromImage(img, i);
                if (t->x36 == 0) {
                    flMemcpy(dst, adr, size);
                } else {
                    flPS2Conv4_8_32(w, ht, adr, dst, 0);
                }
                break;
            case 1:
                size = w * ht;
                adr = plAPXGetPixelAddressFromImage(img, i);
                if (t->x36 == 0) {
                    flMemcpy(dst, adr, size);
                } else {
                    flPS2Conv4_8_32(w, ht, adr, dst, 1);
                }
                break;
            case 2:
                size = w * ht * 2;
                flMemcpy(dst, plAPXGetPixelAddressFromImage(img, i), size);
                break;
            case 3:
                size = w * ht * 4;
                flMemcpy(dst, plAPXGetPixelAddressFromImage(img, i), size);
                break;
            case 4:
                size = w * ht * 4;
                flMemcpy(dst, plAPXGetPixelAddressFromImage(img, i), size);
                flPS2ConvertAlpha(dst, w, ht);
                break;
            }
            i++;
            w >>= 1;
            ht >>= 1;
            dst += size;
            c += 0x48;
        } while (!(mips < i));
    }
    flPS2CreateTextureHandle(h, arg);
    if (t->pix == 0x14 || t->pix == 0x13) {
        ph = flPS2GetPaletteHandle();
        p = &flPalette[((u32)(ph & 0xFFFF0000) >> 16) - 1];
        plAPXSetPaletteContextFromImage(&pctx, img, ((u32)(ph & 0xFFFF0000) >> 16) - 1);
        flPS2GetPaletteInfoFromContext(&pctx, ph, arg);
        p->sysmem = flPS2GetSystemMemoryHandle(p->size, 2);
        pd = (u8 *)flPS2GetSystemBuffAdrs(p->sysmem);
        adr = plAPXGetPaletteAddressFromImage(img, 0);
        if (t->pix == 0x13) {
            sctx = pctx;
            pctx.base = adr;
            sctx.base = (int)pd;
            flPS2ConvertContext(&pctx, &sctx, 0, 1);
        } else {
            flMemcpy(pd, adr, p->size);
            if (pctx.bpp == 4) {
                flPS2ConvertAlpha(pd, p->w, p->h);
            }
        }
        flPS2CreatePaletteHandle(ph, arg);
    }
    return h | ph;
}

int flCreateTextureFromTim2_mem(u8 *img, int arg) {
    int h;
    int ph;
    int mips;
    TEXH *t;
    TEXH *p;
    PLCTX ctx[7];
    PLCTX pctx;
    u8 *dst;
    u8 *pd;
    u8 *c;
    int i;
    int w;
    int ht;
    int size;
    int adr;

    ph = 0;
    h = flPS2GetTextureHandle();
    t = &flTexture[(h & 0xFFFF) - 1];
    mips = plTIM2GetMipmapTextureNum(img, (h & 0xFFFF) - 1);
    if (plTIM2SetContextFromImage(ctx, img) == 0) {
        return 0;
    }
    flPS2GetTextureInfoFromContext(ctx, mips + 1, h, arg);
    t->sysmem = flPS2GetSystemMemoryHandle(t->size, 2);
    dst = (u8 *)flPS2GetSystemBuffAdrs(t->sysmem);
    w = t->w;
    ht = t->h;
    i = 0;
    if (0 <= mips) {
        c = (u8 *)ctx;
        do {
            switch (((PLCTX *)c)->bpp) {
            case 0:
                size = (w * ht) >> 1;
                adr = plTIM2GetPixelAddressFromImage(img, i);
                if (t->x36 == 0) {
                    flMemcpy(dst, adr, size);
                } else {
                    flPS2Conv4_8_32(w, ht, adr, dst, 0);
                }
                break;
            case 1:
                size = w * ht;
                adr = plTIM2GetPixelAddressFromImage(img, i);
                if (t->x36 == 0) {
                    flMemcpy(dst, adr, size);
                } else {
                    flPS2Conv4_8_32(w, ht, adr, dst, 1);
                }
                break;
            case 2:
                size = w * ht * 2;
                flMemcpy(dst, plTIM2GetPixelAddressFromImage(img, i), size);
                break;
            case 3:
                size = w * ht * 4;
                flMemcpy(dst, plTIM2GetPixelAddressFromImage(img, i), size);
                break;
            case 4:
                size = w * ht * 4;
                flMemcpy(dst, plTIM2GetPixelAddressFromImage(img, i), size);
                flPS2ConvertAlpha(dst, w, ht);
                break;
            }
            i++;
            w >>= 1;
            ht >>= 1;
            dst += size;
            c += 0x48;
        } while (!(mips < i));
    }
    flPS2CreateTextureHandle(h, arg);
    if (t->pix == 0x14 || t->pix == 0x13) {
        ph = flPS2GetPaletteHandle();
        p = &flPalette[((u32)(ph & 0xFFFF0000) >> 16) - 1];
        plTIM2SetPaletteContextFromImage(&pctx, img, ((u32)(ph & 0xFFFF0000) >> 16) - 1);
        flPS2GetPaletteInfoFromContext(&pctx, ph, arg);
        p->sysmem = flPS2GetSystemMemoryHandle(p->size, 2);
        pd = (u8 *)flPS2GetSystemBuffAdrs(p->sysmem);
        flMemcpy(pd, plTIM2GetPaletteAddressFromImage(img), p->size);
        if (pctx.bpp == 4) {
            flPS2ConvertAlpha(pd, p->w, p->h);
        }
        flPS2CreatePaletteHandle(ph, arg);
    }
    return h | ph;
}
