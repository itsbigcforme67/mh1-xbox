/* flfnt05 - 0x00217DE0-0x00217E48: flnecAscii2Sjis. Whole file in flfnt_nm.c. */
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

/* Half-width character to its Shift-JIS full-width form. */
int flnecAscii2Sjis(int c) {
    c &= 0xFF;
    if (c < 0x21) {
        return 0x8140;
    }
    if (c < 0x60) {
        return c + 0x7FFF + 0x520;
    }
    if (c < 0x80) {
        return c + 0x7FFF + 0x521;
    }
    if (c == 0xA5) {
        return 0x85A3;
    }
    return c + 0x7FFF + 0x4FF;
}
