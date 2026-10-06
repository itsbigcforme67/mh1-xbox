#ifndef APX_H
#define APX_H
/* Capcom APX texture file (docs/formats/graphics.md section 7), see src/main/tex/apx01.c. */
#include "types.h"

typedef struct APXCH {
    int bits;
    int shift;
    int mask;
} APXCH;

typedef struct APXCTX {
    int flags;      /* 0x00 */
    int w;          /* 0x04 */
    int h;          /* 0x08 */
    int pitch;      /* 0x0C */
    int x10;
    int bytes;      /* 0x14 bytes per pixel / palette entry */
    APXCH c0;       /* 0x18 */
    APXCH c1;       /* 0x24 */
    APXCH c2;       /* 0x30 */
    APXCH c3;       /* 0x3C */
} APXCTX;

typedef struct APXHDR {
    u32 size;       /* 0x00 */
    u32 pixbytes;   /* 0x04 */
    u32 palbytes;   /* 0x08 */
    u16 bpp;        /* 0x0C */
    u16 w;          /* 0x0E */
    u16 h;          /* 0x10 */
    u16 mips;       /* 0x12 */
    u16 palbpp;     /* 0x14 */
    u16 palnum;     /* 0x16 */
} APXHDR;

APXHDR *GetAPXFileHeader(void *img);
u8 *GetAPXPixelMipmapAdrs(void *img, int mip);
int plAPXGetMipmapTextureNum(void *img);
int plAPXGetPaletteNum(void *img);
u8 *GetAPXPaletteAdrs(void *img, int idx);

#endif
