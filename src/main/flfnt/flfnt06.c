/* flfnt06 - 0x00217760-0x00217A54: flnecReloadTexture (permuter output, hence the odd style). Whole file in flfnt_nm.c. */
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
void flLockTexture(int, int, void *, int);
void flUnlockTexture(int);
void flReloadTexture(int, void *);
void flnecExpandFont(u8 *src, u8 *dst);

/* Uploads the glyphs queued in load[] (x5C .. x60) into the glyph textures
 * (11 x 11 glyphs of 22 x 22 pixels per texture, 121 per page), then reloads
 * all textures and palettes. */
void flnecReloadTexture(void)
{
  int h;
  int page;
  int g;
  int j;
  int i;
  u8 lock[0x50];
  u8 *new_var;
  int arr[0x23];
  s32 *new_var2;
  int m;
  int q;
  int off;
  s32 *new_var3;
  if (np->x5C != np->x60)
  {
    for (page = np->x5C / 121; page <= (np->x60 / 121); page++)
    {
      h = np->texh[page];
      flLockTexture(0, h, lock, 2);
      for (;;)
      {
        if ((((np->x5C / 121) != page) || (np->x5C >= 0x16B)) || (np->x5C == np->x60))
        {
          break;
        }
        g = np->x5C;
        m = g % 11;
        q = (g % 121) / 11;
        new_var = np->glyph + (np->load[g] * 100);
        off = (514 + (m * 22)) + (q * 5632);
        flnecExpandFont(new_var, (*((u8 **) (lock + 0x10))) + (off / 2));
        np->x5C++;
      }

      flUnlockTexture(h);
    }

  }
  j = 0;
  new_var2 = np->texh;
  arr[j++] = new_var2[0];
  new_var3 = np->texh;
  arr[j++] = new_var3[1];
  arr[j++] = new_var2[2];
  for (i = 0; i < 0x20; i++, j++)
  {
    arr[j] = np->palh[i] << 16;
  }

  flReloadTexture(j, arr);
}
