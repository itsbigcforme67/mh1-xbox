/* fl library: flPS2SetTextureRegister (SLPM_654.95 0x001795E0-0x00179B80) fills the GS texture registers for a (texture | palette << 16) handle pair:
 * r1 (CLAMP-like word), r2 (mipmap parameters TEX1: K / L / MXL, depends on the render-state bits 0xC00000 / 0x10000), r3 (TEX0: page, width, format,
 * size, palette), r4 (size word, render-state bits 0x60000), r5 / r6 (MIPTBP1 / MIPTBP2: the VRAM addresses and widths of mip levels 1-6).
 * Returns 0 when the texture (or its palette) is not resident. Field names are guesses. */
#include "types.h"

typedef long u64;

typedef struct TEXH {
    int x0;             /* 0x00 */
    int x4;             /* 0x04 bit 2: palettised */
    int size;           /* 0x08 */
    int x0C;            /* 0x0C */
    s16 x10, x12;
    s16 x14;            /* 0x14 first VRAM page */
    s16 x16;            /* 0x16 */
    s16 x18;            /* 0x18 */
    s16 w;              /* 0x1A */
    s16 h;              /* 0x1C */
    u8 _pad1E[2];
    u32 pix;            /* 0x20 pixel storage format */
    int sysmem;         /* 0x24 */
    s16 x28;
    s16 x2A;
    int x2C;            /* 0x2C */
    u32 mips;           /* 0x30 */
    s8 used;            /* 0x34 */
    s8 resident;        /* 0x35 */
    s8 x36;
    s8 x37;
} TEXH;

extern TEXH flTexture[0x100];
extern TEXH flPalette[0x100];
extern int flMipmapK;
extern u32 flMipmapL;

int flPS2GetVramTransAdrs(TEXH *, int);

int flPS2SetTextureRegister(u32 h, u64 *r1, u64 *r2, u64 *r3, u64 *r4, u64 *r5, u64 *r6, int rs) {
    TEXH *t;
    TEXH *p;
    u32 ti;
    u32 pi;
    u32 k;
    u32 mips;
    s16 sh;
    u32 i;
    int hw;
    int adr[6];
    int wd[6];
    int *pa;
    int *pw;

    ti = h & 0xFFFF;
    t = &flTexture[ti - 1];
    if (ti == 0) {
        return 0;
    }
    if (t->x14 == 0) {
        return 0;
    }
    if (t->resident == 0) {
        return 0;
    }
    if (t->x4 & 4) {
        pi = (h & 0xFFFF0000) >> 16;
        if (pi == 0) {
            return 0;
        }
        p = &flPalette[pi - 1];
        if (p->x14 == 0) {
            return 0;
        }
        if (p->resident == 0) {
            return 0;
        }
    }
    k = rs & 0xC00000;
    if (k == 0 || (mips = t->mips) == 1) {
        if ((rs & 0x10000) == 0x10000) {
            *r2 = 0;
        } else {
            *r2 = 0x60;
        }
    } else {
        if (k == 0x400000) {
            if ((rs & 0x10000) == 0x10000) {
                *r2 = ((long)(u32)((flMipmapK * 16) & 0xFFF) << 32) | ((0x80 | ((u64)(mips - 1) << 2)) | ((u64)flMipmapL << 19));
            } else {
                *r2 = ((long)(u32)((flMipmapK * 16) & 0xFFF) << 32) | ((0x120 | ((u64)(mips - 1) << 2)) | ((u64)flMipmapL << 19));
            }
        } else {
            if ((rs & 0x10000) == 0x10000) {
                *r2 = ((long)(u32)((flMipmapK * 16) & 0xFFF) << 32) | ((0xC0 | ((u64)(mips - 1) << 2)) | ((u64)flMipmapL << 19));
            } else {
                *r2 = ((long)(u32)((flMipmapK * 16) & 0xFFF) << 32) | ((0x160 | ((u64)(mips - 1) << 2)) | ((u64)flMipmapL << 19));
            }
        }
    }
    sh = (s16)(t->w / 64);
    if (sh == 0) {
        sh = 1;
    }
    if (t->x4 & 4) {
        p = &flPalette[((h & 0xFFFF0000) >> 16) - 1];
        if (p->x0C == 3) {
            *r1 = 0x80 | ((u64)0x80 << 32);
        } else {
            *r1 = (u64)0x80 << 32;
        }
        *r3 = ((u64)p->pix << 51) | (((u64)p->x14 << 37) | (((u64)t->x18 << 30) | (((u64)t->x16 << 26) | (((u64)t->pix << 20) | (t->x14 | ((u64)sh << 14)))) | ((u64)4 << 32))) | ((u64)0x40000000 << 32);
    } else {
        if (t->x0C == 3) {
            *r1 = 0x80 | ((u64)0x80 << 32);
        } else {
            *r1 = (u64)0x80 << 32;
        }
        *r3 = ((u64)t->x18 << 30) | (((u64)t->x16 << 26) | (((u64)t->pix << 20) | (t->x14 | ((u64)sh << 14)))) | ((u64)4 << 32);
    }
    k = rs & 0x60000;
    switch (k) {
    case 0:
        *r4 = 0;
        break;
    default:
    case 0x20000:
        *r4 = ((u64)t->w << 14) | 0xA | ((u64)t->h << 34);
        break;
    }
    *r5 = 0;
    *r6 = 0;
    if (t->mips > 1) {
        for (i = 0; i < 6; i++) {
            adr[i] = 0;
            wd[i] = 0;
        }
        pa = adr;
        pw = wd;
        hw = t->w >> 1;
        for (i = 0; i < t->mips - 1; i++) {
            *pa = flPS2GetVramTransAdrs(t, i + 1);
            *pw = hw / 64;
            if (*pw == 0) {
                *pw = 1;
            }
            hw >>= 1;
            pa++;
            pw++;
        }
        *r5 = ((u64)wd[2] << 54) | (((u64)adr[2] << 40) | (((u64)wd[1] << 34) | (((u64)adr[1] << 20) | (adr[0] | ((u64)wd[0] << 14)))));
        *r6 = ((u64)wd[5] << 54) | (((u64)adr[5] << 40) | (((u64)wd[4] << 34) | (((u64)adr[4] << 20) | (adr[3] | ((u64)wd[3] << 14)))));
    }
    return 1;
}
