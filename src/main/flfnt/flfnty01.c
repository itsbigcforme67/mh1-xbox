/* SLPM_654.95 0x00216DE0-0x00216F90: flfntFontPuts .. flfntFontPuts. See flfnt_nm.c. */
#include "types.h"

typedef unsigned __int128 u128;
typedef unsigned long u64;

typedef struct FNP {
    s32 tex;            /* 0x00 texture handle (tex[0]) */
    s32 w;              /* 0x04 default glyph width */
    s32 h;              /* 0x08 default glyph height */
    u8 *glyph;          /* 0x0C glyph bitmap area */
    s16 x;              /* 0x10 pen x */
    s16 y;              /* 0x12 pen y */
    f32 z;              /* 0x14 depth (0..1000) */
    s16 px;             /* 0x18 draw pen x */
    s16 py;             /* 0x1A draw pen y */
    u8 sw;              /* 0x1C size w */
    u8 sh;              /* 0x1D size h */
    s16 pal;            /* 0x1E palette 0..31 */
    u8 *palbuf;         /* 0x20 palette data (0x40 bytes each) */
    s32 cnt[5];         /* 0x24 requests per layer */
    u8 *list[5];        /* 0x38 request lists */
    u32 strlen;         /* 0x4C bytes used in the string buffer */
    u8 *str;            /* 0x50 string buffer */
    s32 halftype;       /* 0x54 */
    s32 x58;            /* 0x58 */
    s32 x5C;            /* 0x5C */
    s32 x60;            /* 0x60 glyph loads pending */
    u16 load[0x16B];    /* 0x64 glyph indices to load */
    u16 cache[0x1E80];  /* 0x33A glyph index -> texture slot (0xFF none) */
    s32 next;           /* 0x403C next free slot */
    s32 texh[3];        /* 0x4040 texture handles */
    s32 palh[0x20];     /* 0x404C palette handles */
    s32 dbuf;           /* 0x40CC DMA packet start */
    u32 dcur;           /* 0x40D0 DMA packet write position */
    s32 dflag;          /* 0x40D4 */
    s32 dnum;           /* 0x40D8 */
    s32 dx;             /* 0x40DC */
} FNP;

extern FNP *np;
typedef struct { s32 a, b; } P8;

void *memset(void *, int, int);
s32 flfntMakeHandle(void);
int vsprintf(char *, const char *, char *);

void flfntCacheFlush(void);
void flLockPalette(int, int, void *, int);
void flUnlockPalette(int, void *);
typedef struct FREQ {
    s16 x;              /* 0x00 */
    s16 y;              /* 0x02 */
    f32 z;              /* 0x04 */
    u8 sw;              /* 0x08 */
    u8 sh;              /* 0x09 */
    s16 pal;            /* 0x0A */
    char *str;          /* 0x0C */
} FREQ;
void flSetRenderState(int, int);
void SetTrnslMode(int, int);
void flfntDrawStart(void);
void flfntDrawTerm(void);
void flfntFontPuts(char *, FREQ *);
extern u8 system_w[];
int flCreateTextureHandle(void *, int);
int flCreatePaletteHandle(void *, int);
extern u8 tex_583[];
extern u8 pal_584[];
int flnecCheckString(void);
void flnecReloadTexture(void);
void flPS2GetSystemTmpBuff(int, int, void *);
void flPS2DmaAddQueue2(int, u64, void *, void *);
extern u8 flPs2State[];
extern u8 flPs2VIF1Control[];
int flfntSjis2Jis(u32 c);
void flfntFontPutc(int idx, FREQ *r);
int flnecAscii2Sjis(int c);
int flfntSjis2Index(u32 c);
void flfntPaletteTrans();
void flnecCheckFont(int idx);
void flLockTexture(int, int, void *, int);
void flUnlockTexture(int);
void flReloadTexture(int, void *);
void flnecExpandFont(u8 *src, u8 *dst);





















/* Texture/palette records of the fl layer (0x38 bytes each; only the GS fields used here). */
typedef struct FLTEX {
    u8 _pad00[0x14];
    s16 x14;            /* 0x14 TBP0 (texture) / CBP (palette) */
    s16 x16;            /* 0x16 TW (log2 width) */
    s16 x18;            /* 0x18 TH (log2 height) */
    u8 _pad1A[6];
    u32 x20;            /* 0x20 PSM (texture) / CPSM (palette) */
} FLTEX;
extern FLTEX flTexture[];
extern FLTEX flPalette[];







#define EXP1(o, i) \
    do { \
        u8 b = (i); \
        (o)[0] = ((b >> 6) & 3) | (((b >> 4) & 3) << 4); \
        (o)[1] = ((b >> 2) & 3) | ((b & 3) << 4); \
    } while (0)


/* Draws one request: Shift-JIS characters are 2 bytes (full width), others
 * are half width (3/5 of the size unless halftype is set). */
void flfntFontPuts(char *str, FREQ *r) {
    u32 c;
    int full;
    int idx;

    np->px = r->x;
    np->py = r->y;
    flfntPaletteTrans(r);
    for (;;) {
        c = *(u8 *)str++;
        if (c == 0) {
            return;
        }
        if (c == 0xA) {
            np->px = r->x;
            np->py += r->sh;
            continue;
        }
        if (c >= 0x80 && c <= 0x9F || c >= 0xE0 && c < 0x100) {
            int c2 = *(u8 *)str;
            if (c2 == 0) {
                return;
            }
            c <<= 8;
            c |= c2;
            str++;
            full = 0;
        } else {
            c = flnecAscii2Sjis(c & 0xFF);
            full = 1;
        }
        if (c != 0x8140) {
            idx = flfntSjis2Index(c);
            if (idx >= 0) {
                flfntFontPutc(idx, r);
            }
        }
        if (full == 0) {
            np->px += r->sw;
        } else if (np->halftype != 0) {
            np->px += (s16)(r->sw / 2);
        } else {
            np->px += (s16)(r->sw * 2 / 3);
        }
    }
}
