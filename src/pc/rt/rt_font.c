/*
 * rt_font.c - the fl font system (main f_flfntsjis2 0x216490-0x217760) and
 * the game's print helpers (f_font 0x161970-0x162640), written from the
 * asm for the PC.
 *
 * The font is AFS_DATA entry 0x6D2 (font_set loads it): 7808 glyphs in
 * JIS row/cell order (index = (jis_hi - 0x21) * 94 + jis_lo - 0x21, from
 * flfntSjis2Index), 100 bytes each: 20 x 20 pixels at 2 bits, MSB first,
 * row-major (checked by rendering 'A' and a kana as text). Value 0 is
 * the background, 1-3 the palette's three colours (flfntSetPalData(n, bg,
 * c1, c2, c3), 0xRRGGBB; bg is transparent unless its top byte is set,
 * c1-c3 are transparent when black and the top byte is 0). ASCII is drawn
 * with the font's own half-width glyphs (flnecAscii2Sjis: 0x8520 + c);
 * the pen moves by w for 2-byte characters and by w/2 (half type, the
 * game's setting) or 2w/3 for ASCII.
 *
 * Prints go to five stacks chosen by the z value (font_set_stack_no: z =
 * 1 + 200 n); trans() draws stack 0..4 after the matching prim layers
 * (rt_game_draw_2d). Coordinates are 640 x 448 (flfntFontPutc scales x
 * by the frame width / 640). Each glyph+palette pair becomes a small host
 * texture on first use.
 */
#include "rt.h"
#include "types.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NGLYPH 0x1E80
#define NSTACK 5
#define STACK_MAX 128
#define STRBUF 0x8000

typedef struct {
    s16 x, y;
    f32 z;
    u8 w, h;
    s16 pal;
    u32 str;            /* offset into strbuf */
} FNT_ENT;

static u8 *font_data;           /* the glyph file */
static FNT_ENT stack[NSTACK][STACK_MAX];
static int nstack[NSTACK], mark[NSTACK];
static char strbuf[STRBUF];
static u32 strpos, strmark;
static s16 loc_x, loc_y, pen_x, pen_y;
static f32 cur_z = 10.0f;
static u8 size_w = 20, size_h = 20;
static s16 cur_pal;
static int halftype;
static u32 pal[32][4];          /* RGBA bytes as a word: R | G << 8 | B << 16 | A << 24 */
/* glyph textures per glyph and palette: glyph_tex[glyph] -> 32 slots,
 * allocated when the glyph is first drawn (a flat table was 1 MB) */
static gfx_texture ***glyph_tex; /* [NGLYPH] */
static int in_draw;

extern int font_reset_flag;

uint8_t *rt_file_load(int idx, size_t *n);
void rt_2d_restore_texture(void);

/* ------------------------------------------------------------ set-up */
void flfntCacheFlush(void)
{
    int i;
    int k;
    if (glyph_tex)
        for (i = 0; i < NGLYPH; i++)
            if (glyph_tex[i]) {
                for (k = 0; k < 32; k++)
                    gfx_release_texture(glyph_tex[i][k]);
                free(glyph_tex[i]);
                glyph_tex[i] = NULL;
            }
}

void flfntStackReset(void)
{
    int i;
    for (i = 0; i < NSTACK; i++)
        nstack[i] = mark[i] = 0;
    strpos = strmark = 0;
    loc_x = loc_y = 0;
    cur_z = 10.0f;
    size_w = size_h = 0x16;
    cur_pal = 0;
    halftype = 1;
}

void flfntSetPalData(int n, u32 a, u32 b, u32 c, u32 d)
{
    u32 in[4] = { a, b, c, d };
    u8 al = (a & 0xFF000000) ? 0xFF : 0;
    int i;
    n &= 0x1F;
    for (i = 0; i < 4; i++) {
        u32 v = in[i];
        u8 r = (u8)(v >> 16), g = (u8)(v >> 8), bl = (u8)v, aa;
        if (i == 0 || al)
            aa = al;
        else
            aa = (r | g | bl) ? 0xFF : 0;
        pal[n][i] = r | g << 8 | bl << 16 | (u32)aa << 24;
    }
    if (glyph_tex)                  /* this palette's glyphs are rebuilt on next use */
        for (i = 0; i < NGLYPH; i++)
            if (glyph_tex[i] && glyph_tex[i][n]) {
                gfx_release_texture(glyph_tex[i][n]);
                glyph_tex[i][n] = NULL;
            }
}

/* flfntCreate + MakeHandle: default palette 0 = white with grey edges */
int flfntGetSystemMemorySize(void) { return 0xD0000; }
void flfntCreate(void *mem)
{
    (void)mem;
    if (!glyph_tex)
        glyph_tex = calloc(NGLYPH, sizeof *glyph_tex);
    flfntSetPalData(0, 0, 0xFF333333, 0xFF999999, 0xFFFFFFFF);
}

void flfntInit(void)
{
    flfntStackReset();
    flfntCacheFlush();
}

/* font_set (fontst2_nm.c) loads the font with load_file_mdl(np->area,
 * 0x6D2); np is not used here: the PC loads it itself. */
extern u32 nfcol_tbl[][3];
extern u32 nfrvcol_tbl[][4];
void rt_font_init(void)
{
    size_t n = 0;
    int i;
    if (!font_data) {
        font_data = rt_file_load(0x6D2, &n);
        if (font_data && n < NGLYPH * 100) {
            fprintf(stderr, "rt_font: font file too small (%zu)\n", n);
            free(font_data);
            font_data = NULL;
        }
    }
    flfntCreate(NULL);
    flfntInit();
    for (i = 1; i < 0xE; i++)               /* font_set's palettes */
        flfntSetPalData(i, 0, nfcol_tbl[i - 1][0], nfcol_tbl[i - 1][1], nfcol_tbl[i - 1][2]);
    for (; i < 0x10; i++)
        flfntSetPalData(i, nfrvcol_tbl[i - 0xE][0], nfrvcol_tbl[i - 0xE][1], nfrvcol_tbl[i - 0xE][2], nfrvcol_tbl[i - 0xE][3]);
    font_reset_flag = 0;
}
void font_set(void) { rt_font_init(); }

void flfntSetSize(int w, int h) { size_w = (u8)w; size_h = (u8)h; }
void flfntLocate(int x, int y) { loc_x = (s16)x; loc_y = (s16)y; }
void flfntSetZ(f32 z) { cur_z = z; }
void flfntSetHalftype(int t) { halftype = t; }
void flfntSetPalette(int n)
{
    s16 v = (s16)n;
    int k = v & 0x1F;
    if (v < 0 && k)
        k -= 0x20;
    cur_pal = (s16)k;
}

/* ------------------------------------------------------------ printing */
void flfntPrintf(const char *fmt, ...)
{
    va_list ap;
    int s = (int)(5.0f * (cur_z / 1000.0f)), n;
    FNT_ENT *e;
    if (s < 0) s = 0;
    if (s > 4) s = 4;
    if (getenv("RT_FONT_TRACE") && atoi(getenv("RT_FONT_TRACE")) > 1)
        fprintf(stderr, "font: print z %.1f stack %d n %d pos %u draw %d\n", cur_z, s, nstack[s], strpos, in_draw);
    if (nstack[s] >= STACK_MAX || strpos >= STRBUF - 2)
        return;
    e = &stack[s][nstack[s]++];
    e->x = loc_x;
    e->y = loc_y;
    e->z = cur_z;
    e->w = size_w;
    e->h = size_h;
    e->pal = cur_pal;
    e->str = strpos;
    va_start(ap, fmt);
    n = vsnprintf(strbuf + strpos, STRBUF - strpos, fmt, ap);
    va_end(ap);
    if (n < 0)
        n = 0;
    strpos += (u32)n;
    if (strpos >= STRBUF - 1)
        strpos = STRBUF - 2;
    strbuf[strpos++] = 0;
}

/* SJIS -> glyph index (flfntSjis2Jis / flfntSjis2Index) */
static int sjis2index(unsigned c)
{
    unsigned hi = (c >> 8) & 0xFF, lo = c & 0xFF, jis;
    if (hi >= 0x81 && hi < 0xA0)
        hi -= 0x81;
    else if (hi >= 0xE0 && hi < 0xF0)
        hi -= 0xC1;
    hi <<= 1;
    if (lo >= 0x40 && lo < 0x7F)
        lo -= 0x40;
    else if (lo >= 0x80 && lo < 0x9F)
        lo -= 0x41;
    else if (lo >= 0x9F && lo < 0xFD) {
        lo -= 0x9F;
        hi++;
    }
    jis = ((hi + 1) << 8) + lo + 0x2021;
    {
        int i = (int)(((jis >> 8) & 0xFF) - 0x21) * 94 + (int)((jis & 0xFF) - 0x21);
        return i < NGLYPH ? i : -1;
    }
}

/* flnecAscii2Sjis (0x217DE0) */
static unsigned ascii2sjis(unsigned c)
{
    c &= 0xFF;
    if (c < 0x21) return 0x8140;
    if (c < 0x60) return c + 0x851F;
    if (c < 0x80) return c + 0x8520;
    if (c == 0xA5) return 0x85A3;
    return c + 0x84FE;
}

static gfx_texture *glyph(int idx, int p)
{
    gfx_texture **slot;
    if (!glyph_tex[idx] && !(glyph_tex[idx] = calloc(32, sizeof **glyph_tex)))
        return NULL;
    slot = &glyph_tex[idx][p & 31];
    if (!*slot) {
        u8 rgba[20 * 20 * 4];
        const u8 *g = font_data + idx * 100;
        int i;
        for (i = 0; i < 400; i++) {
            unsigned v = (g[i >> 2] >> (6 - 2 * (i & 3))) & 3;
            memcpy(rgba + 4 * i, &pal[p & 31][v], 4);
        }
        *slot = gfx_create_texture(20, 20, rgba);
    }
    return *slot;
}

static void putc_glyph(int idx, const FNT_ENT *e)
{
    float x0 = pen_x, y0 = pen_y, x1 = pen_x + e->w, y1 = pen_y + e->h;
    float pos[12] = { x0, y0, x1, y0, x0, y1, x1, y0, x1, y1, x0, y1 };
    float st[12] = { 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1 };
    gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)glyph(idx, e->pal));
    gfx_draw_2d(640, 448, 6, pos, st, NULL);
}

/* flfntFontPuts (0x216DE0) */
static void font_puts(const char *s, const FNT_ENT *e)
{
    const u8 *p = (const u8 *)s;
    pen_x = e->x;
    pen_y = e->y;
    for (;;) {
        unsigned c = *p++, code;
        int ascii;
        if (c == 0)
            break;
        if (c == '\n') {
            pen_x = e->x;
            pen_y += e->h;
            continue;
        }
        if ((c >= 0x80 && c < 0xA0) || (c >= 0xE0 && c < 0x100)) {
            if (*p == 0)
                break;
            code = c << 8 | *p++;
            ascii = 0;
        } else {
            code = ascii2sjis(c);
            ascii = 1;
        }
        if (code != 0x8140) {
            int idx = sjis2index(code);
            if (idx >= 0)
                putc_glyph(idx, e);
        }
        if (!ascii)
            pen_x += e->w;
        else if (halftype)
            pen_x += e->w >> 1;
        else
            pen_x += (s16)(2 * e->w / 3);
    }
}

/* flfntDraw (0x216AC0): stack n; only while the host draws a frame */
void flfntDraw(int n)
{
    int i;
    if (!in_draw || !font_data || n < 0 || n >= NSTACK)
        return;
    if (getenv("RT_FONT_TRACE"))
        for (i = 0; i < nstack[n]; i++)
            fprintf(stderr, "font: stack %d at %d,%d size %d pal %d \"%s\"\n", n, stack[n][i].x, stack[n][i].y,
                    stack[n][i].w, stack[n][i].pal, strbuf + stack[n][i].str);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    gfx_set_render_state(GFX_RS_ALPHA_REF, 0x40);
    gfx_set_render_state(GFX_RS_FILTER, 0);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 1);
    for (i = 0; i < nstack[n]; i++)
        font_puts(strbuf + stack[n][i].str, &stack[n][i]);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 0);
    rt_2d_restore_texture();
}

void flfntDrawAll(void)
{
    int i;
    for (i = NSTACK - 1; i >= 0; i--)
        flfntDraw(i);
}

/* The host: prints made while a tick runs stay until the next tick;
 * prints made while drawing (HUD prim callbacks) are dropped after the
 * frame so the next frame does not draw them twice. */
void rt_font_tick_begin(void)
{
    int i;
    for (i = 0; i < NSTACK; i++)
        nstack[i] = 0;
    strpos = 0;
    font_reset_flag = 0;
}
/* the boot screens draw while their tick runs (rt_boot.c, recorded) */
void rt_font_set_draw(int on) { in_draw = on; }
void rt_font_frame_begin(void)
{
    int i;
    for (i = 0; i < NSTACK; i++)
        mark[i] = nstack[i];
    strmark = strpos;
    in_draw = 1;
}
void rt_font_frame_end(void)
{
    int i;
    for (i = 0; i < NSTACK; i++)
        nstack[i] = mark[i];
    strpos = strmark;
    in_draw = 0;
}

/* ------------------------------------------------------------ f_font prints */
static char tmpstr[0x400];
#define FMT_TMP(fmt) do { va_list ap; va_start(ap, fmt); vsnprintf(tmpstr, sizeof tmpstr, fmt, ap); va_end(ap); } while (0)

void font_set_palette(int n);

/* font_print (0x161970) */
void font_print(const char *fmt, ...)
{
    FMT_TMP(fmt);
    flfntPrintf("%s", tmpstr);
}

/* font_print2 (0x161A00): at most n characters (2-byte ones count once) */
void font_print2(int n, const char *fmt, ...)
{
    s16 k = (s16)n;
    int i = 0;
    if (k == 0)
        return;
    FMT_TMP(fmt);
    if (k != -1 && tmpstr[0]) {
        for (;;) {
            u8 c = (u8)tmpstr[i];
            if ((c >= 0x80 && c < 0xA0) || c >= 0xE0)
                i++;
            i++;
            if (i >= 0x400) {
                tmpstr[0x3FF] = 0;
                break;
            }
            if (--k == 0) {
                tmpstr[i] = 0;
                break;
            }
            if (!tmpstr[i])
                break;
        }
    }
    flfntPrintf("%s", tmpstr);
}

/* font_print_ex (0x161B80): locate, palette, print */
void font_print_ex(int x, int y, int p, const char *fmt, ...)
{
    flfntLocate(x, y);
    font_set_palette(p);
    FMT_TMP(fmt);
    flfntPrintf("%s", tmpstr);
}

/* font_print_double (0x1623F0) / _double2 (0x1624B0): a shadow copy at
 * +3,+2 (+2,+2) in palette a, then the text in palette b */
void font_print_double(int x, int y, int a, int b, const char *s)
{
    flfntLocate((s16)(x + 3), (s16)(y + 2));
    font_set_palette(a);
    flfntPrintf("%s", s);
    flfntLocate(x, y);
    font_set_palette(b);
    flfntPrintf("%s", s);
}
void font_print_double2(int x, int y, int a, int b, const char *s)
{
    flfntLocate((s16)(x + 2), (s16)(y + 2));
    font_set_palette(a);
    flfntPrintf("%s", s);
    flfntLocate(x, y);
    font_set_palette(b);
    flfntPrintf("%s", s);
}

/* font_print_uf (0x162570): the string as is */
void font_print_uf(const char *s) { flfntPrintf("%s", s); }

/* font_sp_ck (0x161DE0): one '~' code at s: ~Cnn palette (code 0x100|nn),
 * ~Ann work string nn (0x200|nn), ~~ ; mode 1 gives length adjustments
 * (strlen_sp). Returns the position after the code. */
void font_work_set(u8 *dst, s16 kind);
const char *font_sp_ck(const char *s, s16 *code, int mode)
{
    int n = 0;
    s++;
    switch (*s) {
    case '~':
        *code = (s16)mode == 1 ? -1 : 0;
        break;
    case 'C':
        if ((s16)mode == 1) {
            *code = -4;
            s += 2;
            break;
        }
        if ((s16)mode != 0)
            break;
        s++;
        if (*s >= '0' && *s <= '9') {
            n = *s++ - '0';
            if (*s >= '0' && *s <= '9') {
                n = n * 10 + (*s++ - '0');
                flfntSetPalette(n);
            }
        }
        *code = (s16)(n | 0x100);
        return s;
    case 'A':
        s++;
        if (*s >= '0' && *s <= '9') {
            n = *s++ - '0';
            if (*s >= '0' && *s <= '9')
                n = n * 10 + (*s++ - '0');
        }
        if ((s16)mode == 1) {
            char buf[0x80];
            font_work_set((u8 *)buf, (s16)n);
            *code = (s16)(strlen(buf) - 4);
        } else if ((s16)mode == 0)
            *code = (s16)(n | 0x200);
        return s;
    }
    return s;
}

/* font_print_sp (0x162020): print with ~ codes and newlines, keeping the
 * pen position across palette changes */
void font_print_sp(const char *fmt, ...)
{
    static char tmp[0x400];
    char bufs[2][0x80];
    int cur = 0, len = 0, i;
    s16 x0, x, y, code;
    const char *p;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(tmp, sizeof tmp, fmt, ap);
    va_end(ap);
    x0 = x = loc_x;
    y = loc_y;
    p = tmp;
    for (i = 0; i < 0x3FF && *p; i++) {
        u8 c = (u8)*p;
        if (c == '~') {
            bufs[cur][len] = 0;
            flfntPrintf("%s", bufs[cur]);
            flfntLocate(x, y);
            len = 0;
            x = loc_x;
            y = loc_y;
            p = font_sp_ck(p, &code, 0);
            if ((code & 0xFF00) == 0x200) {
                char *w = bufs[cur ^ 1];
                cur ^= 1;
                font_work_set((u8 *)w, (s16)(code & 0xFF));
                strncat(w, p, 0x7F - strlen(w));
                p = w;
            }
            continue;
        }
        if (len < 0x7E)
            bufs[cur][len++] = (char)c;
        if ((c >= 0x80 && c < 0xA0) || c >= 0xE0) {
            p++;
            if (len < 0x7F)
                bufs[cur][len++] = *p;
            x += size_w;
        } else if (c == '\n') {
            bufs[cur][len - 1] = 0;
            len = 0;
            x = x0;
            y += size_h;
            flfntPrintf("%s", bufs[cur]);
            flfntLocate(x0, y);
        } else if (halftype)
            x += size_w >> 1;
        else
            x += (s16)(2 * size_w / 3);
        p++;
    }
    bufs[cur][len] = 0;
    flfntPrintf("%s", bufs[cur]);
}

/* strlen_sp (0x1625A0): string length counting ~ codes as their width */
int strlen_sp(const char *s)
{
    int n = (int)strlen(s);
    s16 code = 0;
    while (*s) {
        const char *t = strstr(s, "~");
        if (!t)
            return n;
        s = font_sp_ck(t, &code, 1);
        n += code;
    }
    return n;
}
