/* flfnt (whole file, near-matches included) 0x00216490-0x00217E48: Capcom "fl" bitmap font library (flfnt).
 * np (a gp-relative pointer) is the font work: a request list per depth layer
 * (5 layers, 0x80 requests of 0x10 bytes), a string buffer, a cache that maps
 * a JIS glyph index to a slot of the glyph texture, and texture/palette handles. */
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
void flLockTexture(int, int, void *, int);
void flUnlockTexture(int);
void flReloadTexture(void *);
void flnecExpandFont(u8 *src, u8 *dst);

void flfntCreate(u8 *mem) {
    np = (FNP *)mem;
    memset(mem, 0, 0xD0000);
    np->palbuf = mem + 0x4100;
    np->glyph = mem + 0x4900;
    np->list[0] = mem + 0xC3300;
    np->list[1] = mem + 0xC3B00;
    np->list[2] = mem + 0xC4300;
    np->list[3] = mem + 0xC4B00;
    np->list[4] = mem + 0xC5300;
    np->str = mem + 0xC5B00;
    np->w = 0x16;
    np->h = 0x16;
    np->tex = flfntMakeHandle();
}

int flfntGetSystemMemorySize(void) {
    return 0xD0000;
}

void flfntStackReset(void) {
    np->cnt[0] = 0;
    np->cnt[1] = 0;
    np->cnt[2] = 0;
    np->cnt[3] = 0;
    np->cnt[4] = 0;
    np->x = 0;
    np->y = 0;
    np->z = 10.0f;
    np->sw = np->w;
    np->sh = np->h;
    np->pal = 0;
    np->strlen = 0;
    np->halftype = 1;
    np->x60 = np->x5C;
}

void flfntInit(void) {
    flfntStackReset();
    flfntCacheFlush();
}

void flfntCacheFlush(void) {
    int i;

    for (i = 0; i < 0x1E80; i++) {
        np->cache[i] = 0xFF;
    }
    np->next = 0;
    np->x5C = 0;
    np->x60 = 0;
}

/* Palette slot (4 entries of 4 bytes) from four 0xBBGGRR colours; the first
 * colour's top byte non-zero makes every entry opaque-ish (0x80). */
void flfntSetPalData(int idx, u32 c0, u32 c1, u32 c2, u32 c3) {
    u8 col[12];
    u8 lock[0x50];
    u8 *src;
    u16 *dst;
    int i;
    int off;
    u8 a;
    int slot;

    slot = idx % 32;
    off = slot << 6;
    a = (c0 & 0xFF000000) ? 0x80 : 0;
    dst = (u16 *)(np->palbuf + off);
    col[0] = c0;
    col[1] = c0 >> 8;
    col[2] = c0 >> 16;
    col[3] = c1;
    col[4] = c1 >> 8;
    col[5] = c1 >> 16;
    col[6] = c2;
    col[7] = c2 >> 8;
    col[8] = c2 >> 16;
    col[9] = c3;
    col[10] = c3 >> 8;
    col[11] = c3 >> 16;
    src = col;
    for (i = 0; i < 4; i++) {
        u8 r = src[0];
        u8 g = src[1];
        u8 b = src[2];
        ((u8 *)dst)[0] = b;
        ((u8 *)dst)[1] = g;
        dst++;
        ((u8 *)dst)[0] = r;
        if (a | (i == 0)) {
            ((u8 *)dst)[1] = a;
        } else if (b == 0 && g == 0 && r == 0) {
            ((u8 *)dst)[1] = 0;
        } else {
            ((u8 *)dst)[1] = 0x80;
        }
        dst++;
        src += 3;
    }
    i = np->palh[slot];
    if (i != 0) {
        u8 *d2;
        flLockPalette(0, i, lock, 2);
        src = np->palbuf + off;
        d2 = *(u8 **)(lock + 0x10);
        ((s32 *)d2)[0] = ((s32 *)src)[0];
        ((s32 *)d2)[1] = ((s32 *)src)[1];
        src += 8;
        d2 += 8;
        ((s32 *)d2)[0] = ((s32 *)src)[0];
        ((s32 *)d2)[1] = ((s32 *)src)[1];
        flUnlockPalette(i, src);
    }
}

void flfntSetSize(int w, int h) {
    np->sw = w;
    np->sh = h;
}

void flfntLocate(int x, int y) {
    np->x = x;
    np->y = y;
}

void flfntSetZ(f32 z) {
    np->z = z;
}

void flfntSetPalette(int pal) {
    np->pal = (s16)pal % 32;
}

void flfntSetHalftype(int t) {
    np->halftype = t;
}

void flfntPrintf(char *fmt, ...) {
    int layer = (int)(5.0f * (np->z / 1000.0f));
    int n;
    FREQ *r;
    int k;
    char *ap;

    if (layer < 0) {
        layer = 0;
    }
    if (layer >= 5) {
        layer = 4;
    }
    n = np->cnt[layer];
    if (n < 0x80) {
        k = 1;
        np->cnt[layer] = np->cnt[layer] + 1;
        r = (FREQ *)(np->list[layer] + n * 0x10);
        r->x = np->x;
        r->y = np->y;
        r->z = np->z;
        r->sw = np->sw;
        r->sh = np->sh;
        r->pal = np->pal;
        r->str = (char *)np->str + np->strlen;
        ap = (char *)&fmt + 0x48 - (k >= 8 ? 0 : (8 - k) * 8);
        np->strlen += vsprintf(r->str, fmt, ap);
        if (np->strlen >= 0x8000) {
            np->strlen = 0x7FFF;
        }
        np->str[np->strlen] = 0;
        np->strlen++;
    }
}

void flfntDraw(int layer) {
    FREQ *r;
    int i;

    if (layer < 5) {
        flSetRenderState(0x61, 0x1000000);
        system_w[0x2C] = 0xFF;
        system_w[0x2D] = 0xFF;
        SetTrnslMode(1, 0);
        flSetRenderState(0x5F, 4);
        flSetRenderState(0x60, 0x40);
        flSetRenderState(0x6C, 0);
        flfntDrawStart();
        r = (FREQ *)np->list[layer];
        for (i = 0; i < np->cnt[layer]; i++) {
            flfntFontPuts(r->str, r);
            r++;
        }
        flfntDrawTerm();
        flSetRenderState(0x6C, 1);
        flSetRenderState(0x60, 0);
        SetTrnslMode(4, 5);
    }
}

void flfntDrawAll(void) {
    FREQ *r;
    int j;
    int i;

    flSetRenderState(0x61, 0x1000000);
    system_w[0x2C] = 0xFF;
    system_w[0x2D] = 0xFF;
    SetTrnslMode(1, 0);
    flSetRenderState(0x5F, 4);
    flSetRenderState(0x60, 0x40);
    flSetRenderState(0x6C, 0);
    flfntDrawStart();
    for (i = 4; i >= 0; i--) {
        r = (FREQ *)np->list[i];
        for (j = 0; j < np->cnt[i]; j++) {
            flfntFontPuts(r->str, r);
            r++;
        }
    }
    flfntDrawTerm();
    flSetRenderState(0x6C, 1);
    flSetRenderState(0x60, 0);
    SetTrnslMode(4, 5);
}

s32 flfntMakeHandle(void) {
    int i;

    for (i = 0; i < 3; i++) {
        np->texh[i] = flCreateTextureHandle(tex_583, 4);
    }
    for (i = 0; i < 0x20; i++) {
        *(u8 **)(pal_584 + 0x10) = np->palbuf + i * 0x40;
        np->palh[i] = flCreatePaletteHandle(pal_584, 4);
    }
    flfntSetPalData(0, 0, 0xFF333333, 0xFF999999, -1);
    flfntCacheFlush();
    return np->texh[0];
}

void flfntPaletteTrans() {
    /* does nothing */
}

void flfntDrawStart(void) {
    np->x58 = -1;
    if (flnecCheckString() != 0) {
        flfntCacheFlush();
        flnecCheckString();
    }
    flnecReloadTexture();
    np->dbuf = (*(u32 *)(flPs2State + 0x410) + 0xF) & ~0xF;
    np->dcur = np->dbuf;
    if (*(u32 *)(flPs2State + 0x40C) - np->dcur >= 0x10) {
        np->dcur += 0x10;
        np->dflag = 0;
        np->dnum = 0;
        np->dx = -1;
    }
}

void flfntDrawTerm(void) {
    u32 *head;
    u32 *tail;
    u64 t;

    if (np->dflag != 0) {
        head = (u32 *)np->dbuf;
        head[0] = (((u32)np->dcur - (u32)head) >> 4) - 1 | 0x10000000;
        head[1] = 0;
        head[2] = 0;
        head[3] = 0;
        tail = (u32 *)np->dcur;
        tail[0] = 0xF0000001;
        tail[1] = 0;
        tail[2] = 0;
        tail[3] = 0;
        *(u128 *)(tail + 4) = 0;
        np->dcur += 0x20;
        flPS2GetSystemTmpBuff(np->dcur - np->dbuf, 0x10, head);
        t = np->dbuf;
        t <<= 36;
        t >>= 36;
        flPS2DmaAddQueue2(0, t, tail, flPs2VIF1Control);
    }
}

int flfntSjis2Index(u32 c) {
    int j = flfntSjis2Jis(c);
    int hi = (j >> 8) - 0x21;
    int idx = (j & 0xFF) - 0x21;
    idx += hi * 0x5E;
    if (idx >= 0x1E80) {
        idx = -1;
    }
    return idx;
}

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

void flnecCheckFont(int idx) {
    FNP *p = np;
    u16 *slot = &p->cache[(u16)idx];

    if (*slot == 0xFF) {
        *slot = p->next;
        np->next++;
        np->load[np->x60] = idx;
        np->x60++;
    }
}

int flnecCheckString(void) {
    int layer;
    int k;
    FREQ *r;
    u8 *s;
    int c;
    int idx;

    for (layer = 0; layer < 5; layer++) {
        r = (FREQ *)np->list[layer];
        for (k = 0; k < np->cnt[layer]; k++, r++) {
            s = (u8 *)r->str;
            for (;;) {
                c = *s++;
                if (c == 0) {
                    break;
                }
                if (c == 0xA) {
                    continue;
                }
                if (c >= 0x80 && c < 0xA0 || c >= 0xE0 && c < 0x100) {
                    int c2 = *s;
                    if (c2 == 0) {
                        break;
                    }
                    c = c << 8 | c2;
                    s++;
                } else {
                    c = flnecAscii2Sjis(c & 0xFF);
                }
                if (c == 0x8140) {
                    continue;
                }
                idx = flfntSjis2Index(c);
                if (idx >= 0 && idx < 0x1E80) {
                    flnecCheckFont(idx & 0xFFFF);
                    if (np->x60 >= 0x16B) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

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
        if (c >= 0x80 && c < 0xA0 || c >= 0xE0 && c < 0x100) {
            int c2 = *(u8 *)str;
            if (c2 == 0) {
                return;
            }
            c = c << 8 | c2;
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
            np->px += (s16)(r->sw >> 1);
        } else {
            np->px += (s16)(r->sw * 2 / 3);
        }
    }
}

/* Uploads the glyphs queued in load[] (x5C .. x60) into the glyph textures
 * (11 x 11 glyphs of 22 x 22 pixels per texture, 121 per page), then reloads
 * all textures and palettes. */
void flnecReloadTexture(void) {
    int h;
    int page;
    int g;
    int j;
    int i;
    u8 lock[0x50];
    int arr[0x23];
    int m;
    int q;
    int off;

    if (np->x5C != np->x60) {
        for (page = np->x5C / 121; page <= np->x60 / 121; page++) {
            h = np->texh[page];
            flLockTexture(0, h, lock, 2);
            for (;;) {
                if (np->x5C / 121 != page || np->x5C >= 0x16B || np->x5C == np->x60) {
                    break;
                }
                g = np->x5C;
                m = g % 11;
                q = g % 121 / 11;
                off = 514 + m * 22 + q * 5632;
                flnecExpandFont(np->glyph + np->load[g] * 100, *(u8 **)(lock + 0x10) + off / 2);
                np->x5C++;
            }
            flUnlockTexture(h);
        }
    }
    arr[0] = np->texh[0];
    arr[1] = np->texh[1];
    arr[2] = np->texh[2];
    for (i = 0, j = 3; i < 0x20; i++, j++) {
        arr[j] = np->palh[i] << 16;
    }
    flReloadTexture(arr);
}

#define EXP1(o, i) \
    do { \
        u8 b = (i); \
        (o)[0] = ((b >> 6) & 3) | (((b >> 4) & 3) << 4); \
        (o)[1] = ((b >> 2) & 3) | ((b & 3) << 4); \
    } while (0)

/* 2-bit-per-pixel glyph to 4 bit per pixel, 0x16 rows of 0x80 bytes. */
void flnecExpandFont(u8 *src, u8 *dst) {
    int i;
    int j;

    for (i = 0; i < 0x14; i++) {
        EXP1(dst, src[0]);
        dst += 2;
        EXP1(dst, src[1]);
        dst += 2;
        src += 2;
        EXP1(dst, src[0]);
        dst += 2;
        EXP1(dst, src[1]);
        dst += 2;
        src += 2;
        EXP1(dst, src[0]);
        dst += 2;
        src += 1;
        dst[0] = 0;
        dst += 0x76;
    }
    for (i = 0x14; i < 0x16; i++) {
        for (j = 0; j < 6; j += 5) {
            dst[0] = 0;
            dst[1] = 0;
            dst += 2;
            dst[0] = 0;
            dst[1] = 0;
            dst += 2;
            dst[0] = 0;
            dst += 1;
        }
        dst[0] = 0;
        dst += 0x76;
    }
}
