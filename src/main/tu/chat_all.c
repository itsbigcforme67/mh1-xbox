/* chat_all: the whole source file as ONE all-C translation unit (statics defined before their callers), best known C for every function.
 * NOT built by tools/build.py: src/main/chat/f_chat.c is the linked version, in which these functions are raw asm stubs (config/c_rawfuncs.txt):
 * DispFrameMessageA, disp_chat_log_sub, ItemListWindow, PlayerStatusWindow, equip_exp_core, slash_level_bar, ng_word_sub.
 * Everything else here compiles to the original bytes (tools/check.py on this file, tools/b_tu/ to work on it). Not in build_pc.sh. */
/* chat_nm - f_chat (SLPM_654.95 0x001755D0-0x0017BF80, main.bin): sprite/frame helpers, chat log, pit-menu
 * windows (status, equipment), reibun (preset phrases). Near-match C, not built; matching runs are
 * built from it as chatNN.c. Field meanings are guesses. */
#include "types.h"
#include "menu.h"
#include "ud.h"
#include "pl.h"

#define F8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FS8(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define F16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define FS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define F32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define FS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PM ((u8 *)&PitMenu)
void KinshiYogo_chk(char *);
struct PIT_CHAT;
static void chat_log_add(int, s8 *, struct PIT_CHAT *);
int Get_chat_line_num(void);
int Plaza_get_chat_line_num(void);

extern u16 System_timer;
f32 flSin(f32);
void flps0004(void *);
void flps0008(void *);
void SetTextureStage(int);
void SetFilterMode(int);
void SetTrnslMode(int, int);
void reload_tex(int, int);
void flfntLocate(s16, s16);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(void *, ...);
void font_print_sp(void *, ...);
void Put_sprite_rotate(void *, int);
struct FRL;
void DispFrameListA(struct FRL *, char *, int, int);
void DispFrameList(void *, char *, int);
void DispFrameListOptionArrowC(void *, int);
struct FRM;
void DispFrameMessageA(struct FRM *, char *, int);
void DispFrameMessage(void *, void *);
void PutButtonICON(u8 *, u8);
static void disp_cursorC(s16, s16, s16, s16, int, int);
void Disp_help_mess(int, int);
u8 Equip_moji_color_rare(u8);
int Equip_moji_color_rare_i(u8);

typedef struct S2 { s16 a, b; } S2;
typedef struct PFLP4 { s16 p[4]; u32 col; } PFLP4;
typedef struct PFLP8 { s16 p[4]; u32 col; s16 uv[4]; } PFLP8;

/* the highlight bar of row n of a list at y with rows h high, from x0 to
 * x1 (rect {x0, y0, x1, y1}; asm 0x2755D0) */
static void disp_cursorC(s16 x, s16 x1, s16 y, s16 h, int n, int col) {
    PFLP4 q;
    u16 t;

    q.p[0] = x;
    q.p[2] = x1;
    q.p[1] = y - 3 + h * n;
    q.p[3] = q.p[1] + h + 3;
    t = (System_timer & 0x3F) << 10;
    q.col = (col & 0xFFFFFF) | (((s8)(48.0f * flSin(0.0000958738f * (f32)t)) + 0xBF) << 24);
    flps0004(&q);
}

void Name_ID_change(void) {
    PitMenu.x14 ^= 1;
    se_req(7, 0x11, 0);
}

extern u8 pf_menu_sub[];
extern u8 btn_menu_sub[8];
extern u8 lit_2047[];

void Disp_name_or_id(s16 v) {
    FS16(pf_menu_sub, 2) = v;
    DispFrameMessage(pf_menu_sub, lit_2047);
    FS16(btn_menu_sub, 2) = v;
    PutButtonICON(btn_menu_sub, 1);
}

extern u8 button_icon_uv[][8];

void PutButtonICON(u8 *p, u8 n) {
    PFLP8 q;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    while (n > 0) {
        q.p[0] = 0.8f * (f32)FS16(p, 0);
        q.p[1] = FS16(p, 2);
        q.p[2] = 0.8f * (f32)FS16(p, 4);
        q.p[3] = FS16(p, 4);
        *(S2 *)&q.uv[0] = *(S2 *)(button_icon_uv[F8(p, 6)] + 0);
        *(S2 *)&q.uv[2] = *(S2 *)(button_icon_uv[F8(p, 6)] + 4);
        q.col = (F8(p, 7) << 24) | 0xFFFFFF;
        flps0008(&q);
        n--;
        p += 8;
    }
}

extern u8 equip_color_rare_idx[];
extern u32 equip_color_rare_tbl[];
extern u8 moji_color_rare_2099[5];

int Equip_icon_color_rare(int a, int alpha, int sub) {
    if (sub & 0xFF) {
        a = (a + 5) & 0xFF;
    }
    return ((alpha & 0xFF) << 24) | equip_color_rare_tbl[equip_color_rare_idx[a & 0xFF]];
}

u8 Equip_moji_color_rare(u8 a) {
    return moji_color_rare_2099[a];
}

void DispFrameList(void *a, char *b, int c) {
    DispFrameListA(a, b, c, 0xB2);
}

extern u8 lit_2244[];

typedef struct FRL {
    s16 x;          /* 0x00 left */
    s16 y;          /* 0x02 top */
    u8 colw;        /* 0x04 character cell width (font size) */
    u8 h;           /* 0x05 row height (font size) */
    u8 cols;        /* 0x06 columns */
    u8 rows;        /* 0x07 rows */
    s16 pal;        /* 0x08 font palette */
    u16 mode;       /* 0x0A frame style (low 2 bits) */
    s32 *list;      /* 0x0C text lines */
    int col;        /* 0x10 cursor colour */
    int x14;        /* 0x14 */
} FRL;

void DispFrameListA(FRL *fr, char *title, int cur, int alpha) {
    PFLP8 r;
    PFLP4 ln;
    s16 i;
    s16 line;
    f32 x0;
    f32 colw;
    f32 xx;
    f32 xn;
    s16 y0;
    s16 h;
    u8 uvx0;
    u8 uvx1;
    s16 rows;
    int m;
    s32 *tl;
    s16 j;
    s16 py;

    SetFilterMode(1);
    SetTrnslMode(4, 5);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    x0 = (f32)fr->x;
    colw = (f32)fr->colw;
    m = fr->mode & 3;
    switch (m) {
    default:
        uvx0 = 0xC0;
        uvx1 = 0xD4;
        y0 = fr->y - 1;
        h = fr->h + 2;
        break;
    case 1:
        uvx0 = 0xE4;
        uvx1 = 0xF8;
        y0 = fr->y - 2;
        h = fr->h + 4;
        break;
    case 2:
        uvx0 = 0x9C;
        uvx1 = 0xB0;
        y0 = fr->y - 2;
        h = fr->h + 4;
        break;
    }
    rows = fr->rows;
    if (title != 0) {
        rows++;
    }
    r.col = ((alpha & 0xFF) << 24) | 0xFFFFFF;
    r.p[1] = y0;
    r.p[3] = 0;
    for (line = 0; line < rows; line++) {
        r.p[1] = r.p[1] + r.p[3];
        r.p[3] = h;
        r.uv[1] = 0xBC;
        r.uv[3] = 0xD0;
        if (line == 0) {
            r.p[1] -= 8;
            r.p[3] += 8;
            r.uv[1] -= 8;
        }
        if (line >= rows - 1) {
            r.p[3] += 8;
            r.uv[3] += 8;
        }
        xx = x0;
        for (i = 0; i < fr->cols; i++) {
            xn = xx;
            xx += colw;
            r.uv[0] = uvx0;
            r.uv[2] = uvx1;
            if (i == 0) {
                xn -= 8.0f;
                r.uv[0] -= 8;
            }
            if (i >= fr->cols - 1) {
                xx += 8.0f;
                r.uv[2] += 8;
            }
            r.p[0] = 0.8f * xn;
            r.p[2] = (s16)(0.8f * xx) - r.p[0];
            flps0008(&r);
        }
    }
    if (title != 0) {
        ln.p[0] = 0.8f * x0;
        ln.p[2] = 0.8f * (x0 + colw * (f32)fr->cols);
        ln.p[1] = y0;
        ln.p[3] = y0 + h;
        ln.col = 0x30FFFFFF;
        flps0004(&ln);
    }
    if (cur >= 0) {
        if (title != 0) {
            cur++;
        }
        disp_cursorC(0.8f * (x0 - 2.0f), 0.8f * (2.0f + (x0 + colw * (f32)fr->cols)), fr->y, h, cur, fr->col);
    }
    if (fr->list != 0) {
        py = fr->y;
        SetTrnslMode(4, 5);
        flfntSetSize(fr->colw, fr->h);
        font_set_palette(fr->pal);
        if (title != 0) {
            flfntLocate(fr->x, py);
            font_print_sp(lit_2244, title);
            py += h;
        }
        tl = fr->list;
        for (j = fr->rows; j > 0; j--) {
            if (*tl == 0) {
                break;
            }
            flfntLocate(fr->x, py);
            font_print_sp(lit_2244, *tl);
            tl++;
            py += h;
        }
    } else if (title != 0) {
        SetTrnslMode(4, 5);
        flfntSetSize(fr->colw, fr->h);
        font_set_palette(fr->pal);
        flfntLocate(fr->x, fr->y);
        font_print_sp(lit_2244, title);
    }
}

void DispFrameListOptionArrow(void *fr) {
    u16 t = (System_timer & 0x3F) << 10;
    DispFrameListOptionArrowC(fr, (((s8)(48.0f * flSin(0.0000958738f * (f32)t)) + 0xAF) << 8) | 0xF0200020);
}

void DispFrameListOptionArrowC(void *fr, int col) {
    PFLP8 q;

    q.p[2] = 0xE;
    q.p[1] = FS16(fr, 2);
    q.p[3] = F8(fr, 5);
    q.col = col;
    q.p[0] = 0.8f * ((f32)FS16(fr, 0) - 8.0f);
    *(u32 *)&q.uv[0] = 0x1A00A6;
    *(u32 *)&q.uv[2] = 0x2E0094;
    flps0008(&q);
    q.p[0] = 0.8f * ((8.0f + (f32)(FS16(fr, 0) + (F8(fr, 4) * F8(fr, 6)))) - 18.0f);
    q.uv[0] = 0x94;
    q.uv[2] = 0xA6;
    flps0008(&q);
}

void DispFrameMessage(void *a, void *b) {
    DispFrameMessageA(a, b, 0xB2);
}

typedef struct FRM {
    s16 x;          /* 0x00 left */
    s16 y;          /* 0x02 top */
    u8 colw;        /* 0x04 character cell width (font size) */
    u8 h;           /* 0x05 row height (font size) */
    u8 cols;        /* 0x06 columns */
    u8 rows;        /* 0x07 rows */
    s16 pal;        /* 0x08 font palette */
    u16 mode;       /* 0x0A frame style: low 2 bits, 0x8000 = bordered */
    u32 col;        /* 0x0C background colour (bordered style) */
} FRM;

#define SX(v) ((s16)(s32)(0.8f * (v)))

void DispFrameMessageA(FRM *fr, char *text, int alpha) {
    PFLP8 q;
    PFLP4 r;
    f32 x, cw, xl, f, f2;
    u8 u0, u1b;
    s32 xs, n, i, j, rows, cols, last;
    s16 y0, lh, yb, ty;
    u16 fl;
    char buf[0x80];
    char *d;

    SetFilterMode(1);
    SetTrnslMode(4, 5);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    x = (f32)fr->x;
    cw = (f32)fr->colw;
    fl = fr->mode;
    switch (fl & 3) {
    case 1:
        u0 = 228;
        u1b = 248;
        y0 = fr->y - 2;
        lh = fr->h + 4;
        break;
    case 3:
        u0 = 156;
        u1b = 176;
        y0 = fr->y - 2;
        lh = fr->h + 4;
        break;
    default:
        u0 = 192;
        u1b = 212;
        y0 = fr->y - 1;
        lh = fr->h + 2;
        break;
    }
    cols = fr->cols;
    rows = fr->rows;
    if (fl & 0x8000) {
        r.col = fr->col;
        if (!(fl & 1)) {
            r.p[0] = SX(x);
            r.p[2] = SX(x + cw * (f32)cols);
            r.p[1] = y0 - 2;
            r.p[3] = y0;
            flps0004(&r);
            r.p[1] = y0 + lh * rows;
            r.p[3] = r.p[1] + 2;
            flps0004(&r);
            r.p[0] = SX(x - 2.5f);
            r.p[2] = SX(2.5f + (x + cw * (f32)cols));
        } else {
            r.p[0] = SX(x - 1.4f);
            r.p[2] = SX(1.4f + (x + cw * (f32)cols));
        }
        r.p[1] = y0;
        r.p[3] = y0 + lh * rows;
        flps0004(&r);
        q.col = ((u32)(alpha & 0xFF) << 24) | 0xFFFFFF;
        xl = 0.8f * (x - 8.0f);
        xs = SX(x);
        yb = y0 + lh * rows;
        q.p[0] = (s16)(s32)xl;
        q.p[2] = xs - q.p[0];
        q.p[1] = y0 - 8;
        q.p[3] = 8;
        q.uv[0] = u0 - 8;
        q.uv[2] = u0;
        q.uv[1] = 180;
        q.uv[3] = 188;
        flps0008(&q);
        q.uv[0] = u0;
        q.uv[2] = u1b;
        f = x;
        if (!(fl & 1)) {
            q.p[3] = 6;
            q.uv[3] = 186;
        }
        for (n = cols; n > 0; n--) {
            q.p[0] = SX(f);
            f += cw;
            q.p[2] = SX(f) - q.p[0];
            flps0008(&q);
        }
        q.p[0] = SX(f);
        q.p[2] = 6;
        q.p[3] = 8;
        q.uv[0] = u0 + 20;
        q.uv[2] = u0 + 28;
        flps0008(&q);
        q.p[0] = (s16)(s32)xl;
        q.p[2] = xs - q.p[0];
        q.p[1] = yb;
        q.p[3] = 8;
        q.uv[0] = u0 - 8;
        q.uv[2] = u0;
        q.uv[1] = 208;
        q.uv[3] = 216;
        flps0008(&q);
        q.uv[0] = u0;
        q.uv[2] = u1b;
        f = x;
        if (!(fl & 1)) {
            q.p[1] += 2;
            q.p[3] = 6;
            q.uv[1] += 2;
        }
        for (n = cols; n > 0; n--) {
            q.p[0] = SX(f);
            f += cw;
            q.p[2] = SX(f) - q.p[0];
            flps0008(&q);
        }
        q.p[0] = SX(f);
        q.p[2] = 6;
        q.p[1] = yb;
        q.p[3] = 8;
        q.uv[0] = u0 + 20;
        q.uv[2] = u0 + 28;
        flps0008(&q);
        q.p[0] = (s16)(s32)xl;
        q.p[1] = y0;
        q.p[3] = lh;
        q.uv[0] = u0 - 8;
        q.uv[1] = 188;
        q.uv[3] = 208;
        if (!(fl & 1)) {
            q.p[2] = 4;
            q.uv[2] = u0 - 2;
        } else {
            q.p[2] = 5;
            q.uv[2] = u0 - 1;
        }
        for (n = rows; n > 0; n--) {
            flps0008(&q);
            q.p[1] += lh;
        }
        q.p[1] = y0;
        q.p[3] = lh;
        q.uv[2] = u0 + 28;
        q.uv[1] = 188;
        q.uv[3] = 208;
        if (!(fl & 1)) {
            q.p[0] = SX(2.5f + (x + cw * (f32)cols));
            q.p[2] = 4;
            q.uv[0] = u0 + 22;
        } else {
            q.p[0] = SX(1.4f + (x + cw * (f32)cols));
            q.p[2] = 5;
            q.uv[0] = u0 + 21;
        }
        for (n = rows; n > 0; n--) {
            flps0008(&q);
            q.p[1] += lh;
        }
    } else {
        q.col = ((u32)(alpha & 0xFF) << 24) | 0xFFFFFF;
        q.p[1] = y0;
        q.p[3] = 0;
        last = rows - 1;
        for (i = 0; i < rows; i++) {
            q.p[1] += q.p[3];
            q.p[3] = lh;
            q.uv[1] = 188;
            q.uv[3] = 208;
            if (i == 0) {
                q.p[1] -= 8;
                q.p[3] += 8;
                q.uv[1] -= 8;
            }
            if (!(i < last)) {
                q.p[3] += 8;
                q.uv[3] += 8;
            }
            f = x;
            for (j = 0; j < cols; j++) {
                f2 = f;
                f += cw;
                q.uv[0] = (s16)(u0 & 0xFF);
                q.uv[2] = (s16)u1b;
                if (j == 0) {
                    f2 -= 8.0f;
                    q.uv[0] -= 8;
                } else if (!(j < cols - 1)) {
                    f += 8.0f;
                    q.uv[2] += 8;
                }
                q.p[0] = SX(f2);
                q.p[2] = SX(f) - q.p[0];
                flps0008(&q);
            }
        }
    }
    if (text != 0) {
        ty = fr->y;
        SetTrnslMode(4, 5);
        flfntSetSize(fr->colw, fr->h);
        font_set_palette(fr->pal);
        for (n = rows; n > 0; n--) {
            d = buf;
            for (;;) {
                if (*text == '\n') {
                    *d = 0;
                    text++;
                    break;
                }
                *d = *text;
                if (*text == 0) {
                    n = 0;
                    break;
                }
                text++;
                d++;
            }
            flfntLocate(fr->x, ty);
            font_print_sp(lit_2244, buf);
            ty = (s16)(ty + lh);
        }
    }
}

void PutSpriteDiv3(PFLP8 *q, s16 w, s16 d) {
    s16 ow = q->p[2];
    s16 u0 = q->uv[0];
    s16 u1 = q->uv[2];
    int t;

    q->p[2] = w;
    q->uv[2] = q->uv[0] + d;
    flps0008(q);
    q->p[0] = q->p[0] + q->p[2];
    q->p[2] = ow - (w + w);
    q->uv[0] = u0 + d;
    t = u1 - d;
    q->uv[2] = t;
    flps0008(q);
    q->p[0] = q->p[0] + q->p[2];
    q->p[2] = w;
    q->uv[0] = t;
    q->uv[2] = u1;
    flps0008(q);
}

void PutArrow(s16 x0, s16 y, s16 x1, s16 h, int col, int flag) {
    PFLP8 q;
    int f = flag & 0xFF;

    q.uv[1] = 0x1A;
    q.p[1] = y;
    q.uv[3] = 0x2E;
    q.p[3] = h;
    q.col = col;
    if (f & 1) {
        q.uv[0] = 0xA6;
        q.uv[2] = 0x94;
    } else {
        q.uv[0] = 0x94;
        q.uv[2] = 0xA6;
    }
    if (f & 2) {
        q.p[0] = x0;
        q.p[2] = x1;
        Put_sprite_rotate(&q, 2);
        return;
    }
    q.p[0] = 0.8f * (f32)x0;
    q.p[2] = 0.8f * (f32)x1;
    flps0008(&q);
}

extern u8 minisight_tbl[4][10];

void Put_mini_sight(f32 scale, s16 ofs, int col) {
    PFLP8 q;
    u8 (*e)[10];
    u32 i;
    s16 x0;

    SetFilterMode(1);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    x0 = 1.25f * scale;
    q.uv[1] = 0xD8;
    q.col = col;
    q.uv[2] = 0x100;
    e = minisight_tbl;
    q.uv[3] = 0xEC;
    for (i = 4; i != 0; i--, e++) {
        q.p[0] = x0 + FS16(e, 0);
        q.p[1] = ofs + FS16(e, 2);
        q.p[2] = FS16(e, 4);
        q.p[3] = FS16(e, 6);
        q.uv[0] = q.uv[2] - q.p[2];
        Put_sprite_rotate(&q, FS8(e, 8));
    }
}

void Add_to_Monster_list(int n) {
    F32(&User_data, 0x3F0) |= 1 << (n & 0xFF);
}

int Monster_list_chk(int n) {
    return (F32(&User_data, 0x3F0) & (1 << (n & 0xFF))) != 0;
}

int Monster_list_num(void) {
    u32 v = F32(&User_data, 0x3F0);
    v = (v & 0x15555555) + ((v & 0x2AAAAAAA) >> 1);
    v = (v & 0x33333333) + ((v & 0xCCCCCCCC) >> 2);
    v = (v & 0x0F0F0F0F) + ((v & 0xF0F0F0F0) >> 4);
    v = (v & 0x00FF00FF) + ((v & 0xFF00FF00) >> 8);
    return (v & 0xFFFF) + ((v & 0xFFFF0000) >> 16);
}

s8 Monster_list_search(s8 cur, char dir)
{
  int n;
  u32 f;
  f = *((u32 *) (((u8 *) (&User_data)) + 0x3F0));
  if (f != 0)
  {
    if (cur < 0)
    {
      cur = 0;
    }
    else
    {
      cur += dir;
      if (cur > 29)
      {
        cur = 0;
      }
      if (cur < 0)
      {
        cur = 29;
      }
    }
    for (n = 30; n != 0; n--)
    {
      if (f & (1 << cur))
      {
        return cur;
      }
      if (((s8) dir) >= 0)
      {
        cur++;
        if (cur > 29)
        {
          cur = 0;
        }
      }
      else
        if (cur <= 0)
      {
        cur = 29;
      }
      else
      {
        cur--;
      }
    }

  }
  return -1;
}

void Disp_menu_help(void) {
    Disp_help_mess(PitMenu.x11, PitMenu.x12);
}

extern u8 Item_data[][16];
extern u8 help_mess_00354680[];
extern u32 item_col_tbl[];
extern char lit_2796[];
extern s32 *pit_help_str_tbl[];

void Disp_help_mess(int kind, int id) {
    PFLP8 q;
    u8 *it;

    if ((u8)kind < 7 && (u16)id != 0xFFFF) {
        if ((u8)kind == 1 && (u16)id > 0x18) {
            DispFrameMessage(help_mess_00354680, 0);
            flfntSetSize(0x12, 0x12);
            flfntLocate(0x168, 0x166);
            font_set_palette(FS16(help_mess_00354680, 8));
            font_print_sp((void *)pit_help_str_tbl[1][(u16)id]);
            it = Item_data[(u16)id - 0x18];
            reload_tex(1, 0x118);
            SetTextureStage(0x118);
            q.p[0] = 0xF4;
            q.p[1] = 0x166;
            q.p[2] = 0x28;
            q.p[3] = 0x28;
            q.uv[0] = (((it[5] + 1) & 7) << 5) + 1;
            q.uv[1] = (((it[5] + 1) >> 3) << 5) + 1;
            q.uv[2] = (((it[5] + 1) & 7) << 5) + 0x1F;
            q.uv[3] = (((it[5] + 1) >> 3) << 5) + 0x1F;
            q.col = item_col_tbl[it[6]];
            flps0008(&q);
            if (it[4] & 2) {
                *(u32 *)&q.uv[0] = 0xE00080;
                *(u32 *)&q.uv[2] = 0x010000A0;
                q.col = -1;
                flps0008(&q);
            }
            flfntSetSize(0x10, 0x10);
            flfntLocate(0x132, 0x192);
            font_set_palette(Equip_moji_color_rare_i(it[2]));
            font_print(lit_2796, it[2] + 1);
            return;
        }
        DispFrameMessage(help_mess_00354680, (void *)pit_help_str_tbl[(u8)kind][(u16)id]);
    }
}

void Chat_log_clear(void) {
    PitMenu.logtop = 0;
    PitMenu.lognum = 0;
    PitMenu.x21 = 1;
    PitMenu.logscr = 0;
}

int Get_chat_line_num(void) {
    int n = 0;
    int i = PitMenu.logtop - 1;
    int c = PitMenu.lognum;

    for (; c > 0; c--, i--) {
        PIT_CHAT *l = &PitMenu.log[i & 0x3F];
        n += l->nline;
        if (l->uid[0] != 0) {
            n++;
        }
    }
    return n;
}

int Plaza_get_chat_line_num(void) {
    int n = 0;
    int i = PitMenu.logtop - 1;
    int c = PitMenu.lognum;

    for (; c > 0; c--, i--) {
        n += PitMenu.log[i & 0x3F].nline;
    }
    return n;
}

extern u8 Snd_em_id_conv_tbl[];
typedef struct PSWC { u16 x0; u8 _p2[2]; u16 x4; u8 _p6[2]; u16 x8; u8 _pA[2]; u16 xC; } PSWC;
extern PSWC Psw;
#define GW(o) (*(u8 *)((u8 *)&game_w + (o)))

int NPC_Message(s8 *s, u32 left, int mode, int flag) {
    s16 cnt;
    u32 w;
    u32 pos;
    s8 c;
    u16 v;

    if (PitMenu.open != 0) {
        return -1;
    }
    if (mode == 3) {
        PitMenu.x06 = 0x10;
        return 0;
    }
    if (PitMenu.x08 > left || *(s8 **)&PitMenu.x00 != s) {
        PitMenu.x04 = 0;
        PitMenu.x07 = 0;
    }
    PitMenu.x08 = left;
    *(s8 **)&PitMenu.x00 = s;
    PitMenu.x06 = 1;
    c = *s;
    pos = 0;
    while (c != 0) {
        pos++;
        if (c == 0xA) {
            s += 1;
        } else {
            v = ((c << 8) + s[1]) & 0xFFFF;
            if (v == 0x8142 || v == 0x8148) {
                w = 0xF;
            } else if (v == 0x8141) {
                w = 8;
            } else {
                w = 3;
            }
            if (left < w) {
                if (PitMenu.x04 < pos) {
                    if (GW(0x1DC) == 0) {
                        se_req(6, 0x1D, FS8(Snd_em_id_conv_tbl, 0xA), w);
                    } else {
                        se_req(7, 0x1B, 0, w);
                    }
                }
                PitMenu.x04 = pos;
                return 1;
            }
            left -= w;
            s += 2;
        }
        c = *s;
    }
    if (PitMenu.x04 < pos) {
        if (GW(0x1DC) == 0) {
            se_req(6, 0x1D, FS8(Snd_em_id_conv_tbl, 0xA), c);
        } else {
            se_req(7, 0x1B, 0, c);
        }
    }
    PitMenu.x04 = pos;
    switch (mode) {
    case 1:
        PitMenu.x06 |= 2;
        if (flag == 0) {
            PitMenu.x06 |= 4;
        }
        break;
    case 2:
        PitMenu.x06 |= 8;
        if (PitMenu.x07 == 0) {
            PitMenu.x07++;
            if (GW(0x1DC) == 0) {
                se_req(6, 0x1F, FS8(Snd_em_id_conv_tbl, 0xA));
            } else {
                se_req(7, 0x1D, 0);
            }
        }
        break;
    }
    return 0;
}

static void chat_sw_set(u16 *a, u16 *b) {
    *a = Psw.x0;
    *b = Psw.x4;
    if (Psw.x8 & 0x20) { *a |= 0x2000; }
    if (Psw.x8 & 0x10) { *a |= 0x1000; }
    if (Psw.x8 & 8) { *a |= 0x800; }
    if (Psw.x8 & 4) { *a |= 0x400; }
    if (Psw.xC & 0x20) { *b |= 0x2000; }
    if (Psw.xC & 0x10) { *b |= 0x1000; }
    if (Psw.xC & 8) { *b |= 0x800; }
    if (Psw.xC & 4) { *b |= 0x400; }
}

int NPCZoomInCameraCheck();
void SoftKeyboard_pos_set(f32, int);
void SoftKeyboard_set(int, int, int, int);
void Chat_move(int);

void Chat_init(void) {
    int k = 0;

    if (GW(0x1DC) != 0 && NPCZoomInCameraCheck() == 1) {
        k = 4;
    }
    PitMenu.open++;
    SoftKeyboard_pos_set(80.0f, 0x50);
    SoftKeyboard_set(k, 0xE, 0x2C, 0);
    PitMenu.x0F = PitMenu.x0C = 0;
    se_req(7, 0x11, 0);
    Chat_move(0);
}

int ChatKinsoku_chk(u8 *);
int Menu_chatlog_i(void);
void SoftKeyboard_exit(void);
s8 SoftKeyboard_move(s8 *, s16, s16);
static void chat_log_add(int, s8 *, PIT_CHAT *);
void func_5CB100(u8, s8 *, u8);
void net_send_chat(u8, int, s8 *, u8);
void set01_set(int, int, int);


void Chat_move(int a)
{
  u16 sw0;
  int new_var;
  u16 sw1;
  s8 buf[0x40];
  s8 r;
  u8 w;
  int v;
  struct 
  {
    u8 _p[12];
    u8 id;
  } *pl;
  if (((u8) PitMenu.x18) != 0)
  {
    set01_set(0, 0x14, 0);
    PitMenu.x18 = 0;
  }
  chat_sw_set(&sw0, &sw1);
  buf[0] = 0;
  r = SoftKeyboard_move(buf, sw0, sw1);
  if (r != 0)
  {
    if (((buf[0] != 0) && (r > 0)) && (ChatKinsoku_chk((u8 *) buf) != 0))
    {
      pl = (void *) (&player_work[*((u8 *) (((u8 *) (&game_w)) + 0xD1))]);
      if ((*((u8 *) (((u8 *) (&game_w)) + 0x1DC))) == 0)
      {
        if (PitMenu.x15 != 0)
        {
          v = 0xFF;
        }
        else
        {
          v = PitMenu.x19;
          v = (PitMenu.x16 & v) & 0xFF;
        }
 new_var = 0; do { w = v; } while (new_var);
        chat_log_add(pl->id, buf, 0);
        net_send_chat(pl->id, 1, buf, w);
      }
      else
      {
        func_5CB100(pl->id, buf, PitMenu.x17);
      }
      PitMenu.x0F = 1;
      PitMenu.x0E = 1;
      PitMenu.x0C = 0x12C;
    }
    SoftKeyboard_exit();
    PitMenu.open = 0;
    Menu_chatlog_i();
    return;
  }
  PitMenu.x0F = (PitMenu.x0C = 0);
}

char *strcpy(char *, const char *);
extern u8 chat_font_color[8];
extern u8 chat_cnfg_font_color[8];
extern u8 my_user_id[];

static void chat_log_add(int who, s8 *s, PIT_CHAT *src) {
    PIT_CHAT *l;
    s8 c;
    int i;
    int left;
    int room;
    s8 *d;
    s8 *o;
    int uc;

    if (*s == 0) {
        return;
    }
    l = &PitMenu.log[PitMenu.logtop];
    l->who = who;
    if (src != 0) {
        l->col[0] = F8(src, 0x11C);
        l->col[1] = F8(src, 0x11D);
        l->col[2] = F8(src, 0x11E);
        l->col[3] = F8(src, 0x11F);
        strcpy(l->uid, (char *)src);
        strcpy(l->name, (char *)src + 8);
    } else {
        l->col[0] = 0;
        l->col[1] = l->col[2] = chat_font_color[l->who];
        l->col[3] = chat_cnfg_font_color[(u8)PitMenu.x17];
        strcpy(l->uid, (char *)my_user_id);
        strcpy(l->name, (char *)player_work + l->who * 0xA00 + 0x8D4);
    }
    PitMenu.logtop++;
    PitMenu.lognum++;
    PitMenu.logtop &= 0x3F;
    if (PitMenu.lognum > 0x40) {
        PitMenu.lognum = 0x40;
    }
    if ((who & 0xFF) == 0xFF) {
        room = 0x1E;
    } else {
        room = 0x16;
    }
    l->nline = 0;
    i = 0;
    o = (s8 *)l->text[0];
    for (; i < 2; i++, o += 0x1F) {
        d = o;
        if (*s == 0) {
            *o = 0;
            return;
        }
        left = room;
        while (1) {
            c = *s;
            if (c == 0) {
                *d = 0;
                l->nline++;
                return;
            }
            uc = c & 0xFF;
            if ((uc >= 0x80 && uc <= 0x9F) || (uc >= 0xE0 && uc <= 0xFF)) {
                if (left >= 2) {
                    *d = uc;
                    left -= 2;
                    d[1] = s[1];
                    s += 2;
                    d += 2;
                } else {
                    break;
                }
            } else {
                *d = uc;
                s++;
                d++;
                left--;
            }
            if (left <= 0) {
                break;
            }
        }
        *d = 0;
        l->nline++;
    }
}


void Chat_log_add(int who, u8 *msg) {
    KinshiYogo_chk((char *)(msg + 0x1C));
    chat_log_add(who, (s8 *)(msg + 0x1C), (PIT_CHAT *)msg);
    if ((u32)Get_chat_line_num() > 0xB) {
        PitMenu.logscr++;
        if (PitMenu.logscr > 0x3F) {
            PitMenu.logscr = 0x3F;
        }
    }
    PitMenu.x0E = 0;
    PitMenu.x0F = 1;
    PitMenu.x0C = 0x12C;
    if ((who & 0xFF) != GW(0xD1)) {
        se_req(7, 0x18, 0);
    }
}

void Plaza_chat_log_add(u8 *msg) {
    KinshiYogo_chk((char *)(msg + 0x1C));
    chat_log_add(255, (s8 *)(msg + 0x1C), (PIT_CHAT *)msg);
    if ((u32)Plaza_get_chat_line_num() > 9) {
        PitMenu.logscr++;
        if (PitMenu.logscr > 0x3F) {
            PitMenu.logscr = 0x3F;
        }
    }
}

extern u8 chat_font_color[];
int sprintf(char *, const char *, ...);
void font_print_uf(void *, ...);
void font_print_double2(int, int, int, int, char *);
void Put_megaphone(int, int, int);
static void disp_chat_log_sub(int, s16, int);
void Put_receive_mark(int);
int Get_chat_line_num(void);
int Plaza_get_chat_line_num(void);
void PutArrow(s16, s16, s16, s16, int, int);
int Online_ck();
extern char room_member_id[][8];

void ChatLogAdd_Q(int who, int mask, s8 *msg) {
    struct { char name[8]; char uid[0x114]; u8 col[4]; } e;
    u8 k;
    int p;
    u8 c;

    KinshiYogo_chk((char *)msg);
    if ((mask & 0xFF) == 0xFF) {
        k = 3;
    } else if ((mask & 0xFF) == (1 << GW(0xD1))) {
        k = 1;
    } else {
        k = 2;
    }
    p = who & 0xFF;
    e.col[0] = 0;
    c = chat_font_color[p];
    e.col[2] = c;
    e.col[1] = c;
    e.col[3] = chat_cnfg_font_color[k & 0xFF];
    strcpy(e.uid, (char *)player_work + p * 0xA00 + 0x8D4);
    strcpy(e.name, room_member_id[p]);
    chat_log_add(who, msg, (PIT_CHAT *)&e);
    if ((u32)Get_chat_line_num() > 0xB) {
        PitMenu.logscr++;
        if (PitMenu.logscr > 0x3F) {
            PitMenu.logscr = 0x3F;
        }
    }
    PitMenu.x0E = 0;
    PitMenu.x0F = 1;
    PitMenu.x0C = 0x12C;
    se_req(7, 0x18, 0);
}

int Menu_chatlog_i(void) {
    if (Online_ck() == 0) {
        return -1;
    }
    PitMenu.logscr = 0;
    if ((u32)Get_chat_line_num() < 0xC) {
        PitMenu.x21 = 1;
    } else {
        PitMenu.x21 = 0;
    }
    return 0;
}

static u32 chat_log_disp_line(u8 top);

int Menu_chatlog_mv(int sw) {
    PitMenu.x10 = 0;
    PitMenu.x22 = 0x80;
    if ((u32)Get_chat_line_num() > 0xB) {
        if (PitMenu.logscr != 0) {
            PitMenu.x22 |= 2;
        }
        if (chat_log_disp_line(PitMenu.logscr) > 0xB) {
            PitMenu.x22 |= 1;
            if (((u16)sw & 0x2000) && PitMenu.logscr < PitMenu.lognum - 1) {
                PitMenu.x21 = 0;
                PitMenu.logscr++;
                PitMenu.x22 |= 4;
                se_req(7, 0x16, 0);
                if (chat_log_disp_line(PitMenu.logscr) < 0xC) {
                    PitMenu.x21 = 1;
                }
            }
        } else {
            PitMenu.x21 = 1;
        }
        if (((u16)sw & 0x1000) && PitMenu.logscr > 0) {
            PitMenu.logscr--;
            PitMenu.x22 |= 8;
            se_req(7, 0x16, 0);
            PitMenu.x21 = 0;
        }
    }
    if (PitMenu.logscr == 0) {
        PitMenu.x0F = PitMenu.x0C = 0;
    }
    return sw;
}

static u32 chat_log_disp_line(u8 top) {
    int n = 0;
    int i = (PitMenu.logtop - 1) - top;
    int c = PitMenu.lognum - top;

    for (; c != 0; c--, i--) {
        PIT_CHAT *l = &PitMenu.log[i & 0x3F];
        n += l->nline;
        if (l->uid[0] != 0) {
            n++;
        }
    }
    return n;
}

extern u8 pf_chat_log_base[];
extern u8 lit_3171[];
extern char *str_3166[];

void Pit_disp_chat(void) {
    DispFrameMessage(pf_chat_log_base, lit_3171);
    Put_megaphone(0x97, 0xBB, PitMenu.x17);
    flfntSetSize(0x15, 0x12);
    font_set_palette(0);
    flfntLocate(0xC6, 0xBE);
    font_print_uf(str_3166[(u8)PitMenu.x17]);
    disp_chat_log_sub(0, 0, 0);
}

extern char lit_3181_00383570[];

static void chat_log_name(char *buf, PIT_CHAT *l) {
    if (PitMenu.x14 == 0) {
        sprintf(buf, lit_3181_00383570, l->name);
        return;
    }
    sprintf(buf, lit_3181_00383570, l->uid);
}

static void disp_chat_log_sub(int top, s16 yofs, int a) {
    char buf[0x20];
    int i;
    PIT_CHAT *l;
    int cnt;
    int k;
    s16 y;

    if (PitMenu.lognum != 0) {
        flfntSetSize(0x15, 0x12);
        if ((u32)Get_chat_line_num() > 0xB && !(a & 0xFF)) {
            top &= 0xFF;
            y = 0x18F;
            i = (PitMenu.logtop - 1) - top;
            cnt = PitMenu.lognum - top;
            for (; cnt != 0; cnt--, i--) {
                i &= 0x3F;
                l = &PitMenu.log[i];
                font_set_palette(l->col[3]);
                k = l->nline - 1;
                for (; k >= 0; k--) {
                    flfntLocate(0x1E, (s16)(y + yofs));
                    font_print_uf(l->text[k]);
                    y -= 0x13;
                    if (y < 0xD1) {
                        return;
                    }
                }
                if (l->uid[0] != 0) {
                    chat_log_name(buf, l);
                    font_print_double2(0x1E, (s16)(y + yofs), 1, l->col[2], buf);
                    y -= 0x13;
                    if (y < 0xD1) {
                        return;
                    }
                }
            }
        } else {
            int c = PitMenu.lognum;
            y = 0xD1;
            i = PitMenu.logtop - c;
            for (; c != 0; c--, i++) {
                i &= 0x3F;
                l = &PitMenu.log[i];
                if (l->uid[0] != 0) {
                    chat_log_name(buf, l);
                    font_print_double2(0x1E, (s16)(y + yofs), 1, l->col[2], buf);
                    y += 0x13;
                    if (y >= 0x190) {
                        continue;
                    }
                }
                font_set_palette(l->col[3]);
                for (k = 0; k < l->nline; k++) {
                    flfntLocate(0x1E, (s16)(y + yofs));
                    font_print_uf(l->text[k]);
                    y += 0x13;
                    if (y >= 0x190) {
                        break;
                    }
                }
            }
        }
    }
}

void Pit_disp_chat_log(void) {
    u32 t;
    int col;
    int c;
    s16 y;
    s16 y2;

    SetFilterMode(0);
    DispFrameMessage(pf_chat_log_base, 0);
    t = (u16)((System_timer & 0x1F) << 11);
    col = (((s8)(48.0f * flSin(0.0000958738f * (f32)t)) + 0xCF) << 24) | 0x1ACC8E;
    y = 0xB6;
    if (PitMenu.x22 & 1) {
        c = col;
        if (PitMenu.x22 & 4) {
            y -= 2;
        }
    } else {
        c = 0xA0606060;
    }
    PutArrow(0x80, y, 0x1B, 0xF, c, 2);
    y2 = 0x198;
    if (PitMenu.x22 & 2) {
        if (PitMenu.x22 & 8) {
            y2 += 2;
        }
    } else {
        col = 0xA0606060;
    }
    PutArrow(0x80, y2, 0x1B, 0xF, col, 3);
    disp_chat_log_sub(PitMenu.logscr, -0xA, PitMenu.x21);
    Put_receive_mark(0);
}

void Receive_mess_move(void) {
    if (FS8(&PitMenu, 6) != 0) {
        if (F8(&PitMenu, 0xF) == 0) {
            PitMenu.x0C = 0;
        }
        return;
    }
    if (!(PitMenu.x22 & 0x80) && F8(&PitMenu, 0x1C) == 0) {
        PitMenu.x0F = 0;
        if (F16(&PitMenu, 0xC) > 0) {
            PitMenu.x0C = F16(&PitMenu, 0xC) - 1;
        }
    }
}

extern void *receive_mes_str[2];

void Pit_disp_receive_mes(void) {
    if (!(PitMenu.x22 & 0x80) && F8(&PitMenu, 0x1C) == 0 && F16(&PitMenu, 0xC) != 0) {
        SetFilterMode(0);
        DispFrameMessage(pf_chat_log_base, receive_mes_str[F8(&PitMenu, 0xE)]);
        disp_chat_log_sub(0, 0, 0);
    }
}

extern s16 receive_mark_pos[2][2];
extern u8 pf_receive_mark[];
extern char lit_3351[];

void Put_receive_mark(int n) {
    if ((u8)PitMenu.x0F != 0) {
        if ((u8)n == 1) {
            DispFrameMessage(pf_receive_mark, 0);
        }
        if ((System_timer & 0x1F) > 0xC) {
            Put_megaphone(receive_mark_pos[n & 0xFF][0], receive_mark_pos[n & 0xFF][1], 0);
        }
        flfntSetSize(0x15, 0x12);
        font_set_palette(2);
        flfntLocate((s16)(receive_mark_pos[n & 0xFF][0] + 0x1A), (s16)(receive_mark_pos[n & 0xFF][1] + 3));
        font_print_uf(lit_3351);
    }
}

void SetMessageHaltFlag(void) {
    PitMenu.x0C = 0;
    FS8(&PitMenu, 0x1C) = 1;
}

void ClearMessageHaltFlag(void) {
    FS8(&PitMenu, 0x1C) = 0;
}

int func_5D8370(s8);

void Join_pl_chk(void) {
    u32 i;
    u8 *g;

    PitMenu.x1A = 0;
    PitMenu.x19 = 0;
    if (GW(0x1DC) == 0) {
        for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
            if (game_w.master != i && g[0x208] == 1) {
                PitMenu.x19 |= (1 << i) & 0xFF;
                PitMenu.x1A++;
            }
        }
    } else {
        for (i = 0; i < 8; i++) {
            if (game_w.master != i && func_5D8370(i) == 0) {
                PitMenu.x19 |= (1 << i) & 0xFF;
                PitMenu.x1A++;
            }
        }
    }
    if (PitMenu.x16 != 0) {
        PitMenu.x16 &= PitMenu.x19;
        if (PitMenu.x16 == 0) {
            PitMenu.x16 = 0;
            PitMenu.x15 = 1;
            PitMenu.x17 = 3;
            PitMenu.x18 = 1;
        }
    }
}

extern u8 lit_3439[];
extern char lit_3440[];
extern char lit_3441[];
extern char lit_3442[];

void Disp_NPC_message(void) {
    char buf[0x40];
    int left;
    s16 y;
    s8 *s;
    int room;
    char *d;

    if (!(PitMenu.x06 & 0x10)) {
        DispFrameMessage(pf_chat_log_base, lit_3439);
        flfntSetSize(0x15, 0x12);
        font_set_palette(0);
        left = PitMenu.x04;
        s = *(s8 **)&PitMenu.x00;
        y = 0xD1;
        while (left > 0) {
            d = buf;
            room = 0xB;
            while (left > 0 && room > 0) {
                s8 c = *s;
                left--;
                if (c == 0xA) {
                    s++;
                    break;
                }
                d[0] = c;
                room--;
                d[1] = s[1];
                s += 2;
                d += 2;
            }
            *d = 0;
            flfntLocate(0x1E, y);
            font_print_uf(buf);
            y += 0x13;
        }
        if (PitMenu.x06 & 2) {
            flfntLocate(0x1E, (s16)(y + 0x13));
            if (PitMenu.x06 & 4) {
                font_print_sp(lit_3440);
            } else {
                font_print_sp(lit_3441);
            }
        }
        if (PitMenu.x06 & 8) {
            font_set_palette(2);
            flfntLocate(0xA2, (s16)(y + 0x13));
            font_print_uf(lit_3442);
        }
        Put_receive_mark(0);
    }
}

extern FRL item_list_frame;
extern char *item_list_title[];
extern char lit_3511[];
extern char lit_3512[];
extern char lit_3513[];
extern char lit_3514[];
extern char *item_str[];
extern u8 Item_data[][16];
int Item_preparation_one_ck(s16);

void ItemListWindow(int page, int cursel, int mode) {
    s16 y;
    s16 sel;
    char buf[0x20];
    s16 pal;
    UD_ITEM *it;
    u8 *w;
    s16 base;
    s16 cnt;
    s16 pg;

    w = (u8 *)&player_work[GW(0xD1)] + 0x828;
    pg = page / 10;
    base = pg * 10;
    SetTrnslMode(4, 5);
    if (cursel != 0) {
        item_list_frame.col = cursel;
        sel = page - base;
    } else {
        sel = -1;
    }
    sprintf(buf, lit_3511, item_list_title[mode & 1], pg + 1);
    DispFrameList(&item_list_frame, buf, sel);
    if (mode & 8) {
        DispFrameListOptionArrow(&item_list_frame);
    }
    SetFilterMode(0);
    flfntSetSize(0x12, 0x12);
    it = (UD_ITEM *)w + base;
    y = 0x3E;
    for (cnt = 10; cnt > 0; cnt--, it++) {
        y += 0x16;
        flfntLocate(0x1AF, y);
        font_set_palette(0);
        pal = 2;
        if ((mode & 4) && Item_preparation_one_ck(it->id) == 0) {
            font_set_palette(0xA);
            pal = 0xD;
        }
        if (it->id == 0) {
            font_print_uf(lit_3512);
        } else {
            font_print_uf(item_str[it->id]);
            if (Item_data[it->id][3] > 1) {
                flfntLocate(0x24D, y);
                if (Item_data[it->id][3] == 0xFF) {
                    font_print_uf(lit_3513);
                } else {
                    if (it->num >= Item_data[it->id][3]) {
                        font_set_palette(pal);
                    }
                    font_print(lit_3514, it->num);
                }
            }
        }
    }
}

extern FRL frame_status_main_00354770[];
extern FRM frame_status_sub_003547A0[];
extern char lit_3587[];
extern char *menu_status_str_003546E0[][10];
extern char *status_sub_str_00387C60[];
extern char lit_3588_003837D0[];
extern char lit_3589[];
extern char lit_3590[];
extern char lit_3591[];
extern char lit_3592[];
extern char lit_3593[];
extern char lit_3594[];
extern char *hunter_appellation[];
extern f32 job_atk_adj_tbl[];
extern char *Skill_name[];
extern u8 my_user_id[];
int Event_flag_ck(int);
void Get_hunter_status(void *, u8 *, int *, int *);
int Get_weapon_job2(u8, u16);
void PrintPlayerJob(void *);
void PlayerEquipmentWindow(PLW *);
void Put_comment(int, int, int, void *);

void PlayerStatusWindow(u8 *pl, int tab) {
    char buf[0x40];
    u8 rank;
    int pt;
    int nxt;
    int noRank;
    int t;
    u32 i;
    u8 *q;
    s16 y;

    noRank = Event_flag_ck(4) != 1;
    t = tab & 0xFF;
    sprintf(buf, lit_3587, t + 1);
    frame_status_main_00354770[0].list = (s32 *)menu_status_str_003546E0[noRank];
    DispFrameList(&frame_status_main_00354770[(u8)tab], buf, -1);
    DispFrameListOptionArrow(frame_status_main_00354770);
    DispFrameMessage(&frame_status_sub_003547A0[(u8)tab], status_sub_str_00387C60[(u8)tab]);
    switch (t) {
    case 0:
        font_set_palette(0);
        if (Online_ck() == 1) {
            flfntLocate(0x17A, 0x52);
            if (GW(0x1DC) == 0) {
                font_print_uf((u8 *)&game_w + 0x1E8 + F16(pl, 0xC) * 8);
            } else {
                font_print_uf(my_user_id);
            }
        }
        flfntLocate(0x17A, 0x66);
        font_print_uf(pl + 0x8D4);
        flfntLocate(0x17A, 0x7A);
        PrintPlayerJob(pl);
        if (noRank == 0) {
            Get_hunter_status(&User_data, &rank, &pt, &nxt);
            flfntLocate(0x17A, 0x8E);
            font_print(lit_3588_003837D0, rank, hunter_appellation[rank]);
            flfntLocate(0x17A, 0xA2);
            if (rank < 0x14) {
                font_print(lit_3589, pt, nxt);
            } else {
                font_print(lit_3590, pt);
            }
            flfntLocate(0x17A, 0xB6);
        } else {
            flfntLocate(0x17A, 0x8E);
        }
        font_print(lit_3591, F32(&User_data, 0x20));
        flfntLocate(0x18C, 0xCA);
        font_print(lit_3592, FS16(pl, 0x792));
        flfntLocate(0x18C, 0xDE);
        font_print(lit_3592, FS16(pl, 0x882) / 3);
        flfntLocate(0x18C, 0xF2);
        font_print(lit_3592, (u16)((f32)F16(pl, 0x6AC) * job_atk_adj_tbl[Get_weapon_job2(F8(pl, 0x35F), F16(pl, 0x360)) & 0xFF]));
        flfntLocate(0x18C, 0x106);
        font_print(lit_3592, F16(pl, 0x6AE));
        flfntLocate(0x21C, 0xCA);
        font_print(lit_3593, (s16)*(f32 *)(pl + 0x920));
        flfntLocate(0x21C, 0xDE);
        font_print(lit_3593, (s16)*(f32 *)(pl + 0x924));
        flfntLocate(0x21C, 0xF2);
        font_print(lit_3593, (s16)*(f32 *)(pl + 0x928));
        flfntLocate(0x21C, 0x106);
        font_print(lit_3593, (s16)*(f32 *)(pl + 0x92C));
        Put_comment(0x132, 0x126, 0x14, (u8 *)&User_data + 0x3F4);
        return;
    case 1:
        PlayerEquipmentWindow((PLW *)pl);
        if (F8(pl, 0x910) == 0) {
            flfntLocate(0x132, 0x13A);
            font_print_uf(Skill_name[0]);
            return;
        }
        y = 0x13A;
        i = 0;
        do {
            q = pl + i;
            if (F8(q, 0x910) == 0) {
                break;
            }
            flfntLocate(0x132, y);
            font_print(lit_3594, Skill_name[F8(q, 0x910)]);
            y += 0x14;
            i++;
        } while (i < 5);
        return;
    }
}

extern u8 Armor_Head_Data[][0x14];
extern u8 Armor_Body_Data[][0x14];
extern u8 Armor_Arm_Data[][0x14];
extern u8 Armor_Waist_Data[][0x14];
extern u8 Armor_Leg_Data[][0x14];
extern u8 menu_stat_icon_tbl1[5];
extern u8 menu_stat_icon_tbl2[8];
extern char lit_3652[];
int Get_equip_name(u8, u16);
int Get_equip_rare(u8, u16);

void PlayerEquipmentWindow(PLW *pl) {
    PFLP8 q;
    u8 *eq[5];
    s16 y;
    u32 i;
    int ty;

    eq[0] = Armor_Head_Data[pl->work352[2]];
    eq[1] = Armor_Body_Data[pl->work352[3]];
    eq[2] = Armor_Arm_Data[pl->work352[4]];
    eq[3] = Armor_Waist_Data[pl->work352[5]];
    eq[4] = Armor_Leg_Data[pl->work352[0]];
    SetFilterMode(1);
    reload_tex(1, 0x118);
    SetTextureStage(0x118);
    q.p[0] = 0xF8;
    q.p[2] = 0x20;
    y = 0x55;
    q.p[3] = 0x20;
    q.p[1] = 0x55;
    q.uv[0] = menu_stat_icon_tbl2[Get_weapon_job2(pl->work35F, pl->wpn_kind) & 0xFF];
    q.uv[2] = q.uv[0] + 0x20;
    q.uv[1] = 0xA0;
    q.uv[3] = 0xC0;
    q.col = Equip_icon_color_rare(Get_equip_rare(pl->work35F, pl->wpn_kind), 0xFF, 0);
    flps0008(&q);
    q.uv[1] = 0xC0;
    q.uv[3] = 0xE0;
    for (i = 0; i < 5; i++) {
        y += 0x20;
        q.p[1] = y;
        q.uv[0] = menu_stat_icon_tbl1[i];
        q.uv[2] = q.uv[0] + 0x20;
        q.col = Equip_icon_color_rare(eq[i][3], 0xFF, 0);
        flps0008(&q);
    }
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x168, 0x5C);
    font_print_sp(lit_3652, Get_equip_name(pl->work35F, pl->wpn_kind));
    for (i = 0, ty = 0x7C; i < 5; i++) {
        flfntLocate(0x168, ty);
        font_print_sp(lit_3652, FS32(eq[i], 0x10));
        ty += 0x20;
    }
}

extern char *menu_stat_job_str[];

void PrintPlayerJob(void *pl) {
    font_print_uf(menu_stat_job_str[Get_weapon_job2(F8(pl, 0x35F), F16(pl, 0x360)) & 0xFF]);
}

typedef struct EQD { u8 be; u8 kind; u16 id; } EQD;
u8 EquipmentDescriptionWindowA_s(EQD *, s16, s16, int, u8 *, int);
u8 EquipmentDescriptionWindowA(EQD *, int, int, int, u8 *, int);

void EquipmentDescriptionWindow(u8 *a, s16 b, s16 c, int d, u8 *e) {
    EquipmentDescriptionWindowA_s((EQD *)a, b, c, d, e, 0xB2);
}

extern char lit_3701[];
extern char lit_3702[];
void Put_PageArrow(int, int, int, int);
void Put_PageArrow_s(s16, s16, int, u8);
void flfntLocate_i(int, int);
void flfntLocate_s(int, s16);
static void equip_exp_core(u8 *, s16, s16, int, u8 *);
void Get_equip_icon_uv(u8 *, s16 *, s16 *);

u8 EquipmentDescriptionWindowA(EQD *eq, int x, int y, int page, u8 *cmp, int alpha) {
    struct { s16 x; s16 y; u8 w; u8 h; u8 a; u8 b; s16 sp0; s16 sp1; } fr;
    PFLP8 q;
    u8 pages;
    u8 pg;
    u8 pb;

    fr.h = 0x12;
    fr.w = 0x12;
    fr.x = x;
    fr.y = y;
    fr.a = 0x11;
    fr.b = 6;
    fr.sp0 = 0;
    fr.sp1 = 0;
    DispFrameMessageA((struct FRM *)&fr, 0, alpha);
    if (eq != 0 && eq->be != 0) {
        if (eq->id != 0x3E7) {
            pb = page;
            if (eq->kind != 7) {
                pages = 2;
                page = page & 1;
            } else {
                pages = 4;
                page = page & 3;
            }
            if (!(pb & 0x80)) {
                Put_PageArrow_s((s16)x + 0xE1, (s16)y + 0x64, ((page & 0xFF) + 1) & 0xFF, pages);
            }
            pg = page;
            if (pg < 2) {
                SetFilterMode(1);
                reload_tex(1, 0x118);
                SetTextureStage(0x118);
                q.p[0] = 0.8f * (5.0f + (f32)x);
                q.p[2] = 0x20;
                q.p[3] = 0x20;
                q.col = Equip_icon_color_rare(Get_equip_rare(eq->kind, eq->id), 0xFF, 0);
                Get_equip_icon_uv((u8 *)eq, &q.uv[0], &q.uv[2]);
                q.p[1] = y;
                if (pg == 1) {
                    q.p[1] += 0xE;
                }
                flps0008(&q);
            }
            equip_exp_core((u8 *)eq, x, y, page, cmp);
            return pages;
        }
        font_set_palette(0);
        flfntLocate((s16)x + 0x36, (s16)y + 0xA);
        font_print_uf(lit_3701);
        return 0;
    }
    font_set_palette(0);
    flfntLocate((s16)x + 0x36, (s16)y + 0xA);
    font_print_uf(lit_3702);
    return 0;
}


extern char *equip_exp_str_sword[];
extern char *equip_exp_str_gun[];
extern char *equip_exp_str_armor[];
extern char *weapon_exp_str_common[];
extern char *armor_exp_str_common[];
extern char *reload_level_str[];
extern char *wearable_tbl[];
extern char *lv123str[];
extern char *lv12str[];
extern u8 weapon_exp[][16];
extern u8 armor_exp[][16];
extern char lit_4150[];
extern char lit_4151[];
extern char lit_4152[];
extern char lit_4153[];
extern char lit_4154[];
extern char lit_4155[];
extern char lit_4156[];
extern char lit_4157[];
extern char lit_4158[];
extern char lit_4159[];
extern char lit_4160[];
extern char lit_4161[];
extern char lit_4162[];
extern char lit_4163[];
extern char lit_4164[];
extern char lit_4165[];
extern char lit_4166[];
extern char lit_4167[];
extern char lit_4168[];
extern char lit_4169[];
extern char lit_4170[];
extern char lit_4171[];
extern char lit_4172[];
extern char lit_4173[];
void *Get_equip_data_ptr(void *);
void font_print_strings(int, int, void *, int);
int Get_bowgun_atk(void *);
int Get_weapon_job(void *);
int Get_equip_rare(u8, u16);
static void sword_zokusei(u8 *, int, s16);
static void slash_level_bar(u8 *, s16, f32);

#define ATKCONV(v, job) ((u16)((f32)(v) * job_atk_adj_tbl[job]))

static void equip_exp_core(u8 *eq, s16 x, s16 y, int page, u8 *cmp) {
    u8 rare = Get_equip_rare(eq[1], F16(eq, 2));
    int job = Get_weapon_job(eq) & 0xFF;
    int pg = page & 0xFF;
    u8 kind;
    u8 *d;
    u8 *d2;
    s16 ty;
    s16 tx;
    char **str;
    u8 c1;
    u8 c2;
    u8 c3;
    u8 c4;
    u8 c5;
    u16 atk;
    u16 atk2;
    s8 g[3];
    u16 id;
    u8 *row;

    font_set_palette(5);
    kind = eq[1];
    if (kind != 7 && pg > 1) {
        pg = 1;
    }
    switch (pg & 0xFF) {
    case 0:
        ty = y + 0xA;
        tx = x + 0x2D;
        flfntLocate(tx, ty);
        font_print_sp(lit_4150, (s16)Equip_moji_color_rare(rare), Get_equip_name(eq[1], F16(eq, 2)));
        switch (eq[1]) {
        case 6:
            d = Get_equip_data_ptr(eq);
            atk = ATKCONV(F16(d, 8), job);
            c1 = 0;
            if (cmp != 0) {
                if (eq[1] == cmp[1]) {
                    d2 = Get_equip_data_ptr(cmp);
                    atk2 = ATKCONV(F16(d2, 8), Get_weapon_job(cmp) & 0xFF);
                } else {
                    atk2 = (u16)((f32)Get_bowgun_atk(cmp) * job_atk_adj_tbl[Get_weapon_job(cmp) & 0xFF]);
                }
                if (atk2 < atk) {
                    c1 = 4;
                } else if (atk < atk2) {
                    c1 = 2;
                }
            }
            font_set_palette(c1);
            ty = y + 0x28;
            flfntLocate(x + 0x48, ty);
            font_print(lit_3592, atk);
            slash_level_bar(d, y + 0x3E, 4.0f + (153.0f + (f32)x));
            sword_zokusei(d, x, y + 0x50);
            str = equip_exp_str_sword;
            break;
        case 7:
            d = Get_equip_data_ptr(eq);
            atk = (u16)(job_atk_adj_tbl[job] * (f32)Get_bowgun_atk(eq));
            c1 = 0;
            c2 = 0;
            c3 = 0;
            c4 = 0;
            c5 = 0;
            if (cmp != 0) {
                if (eq[1] == cmp[1]) {
                    d2 = Get_equip_data_ptr(cmp);
                    atk2 = (u16)((f32)Get_bowgun_atk(cmp) * job_atk_adj_tbl[Get_weapon_job(cmp) & 0xFF]);
                    if (F8(d2, 3) < F8(d, 3)) {
                        c2 = 4;
                    } else if (F8(d, 3) < F8(d2, 3)) {
                        c2 = 2;
                    }
                    g[0] = F16(cmp, 4) & 0xF;
                    g[1] = F16(eq, 4) & 0xF;
                    if (g[0] < g[1]) {
                        c3 = 4;
                    } else if (g[1] < g[0]) {
                        c3 = 2;
                    }
                    if (F16(cmp, 4) & 0x40) {
                        if (!(F16(eq, 4) & 0x40)) {
                            c4 = 2;
                        }
                    } else if (F16(eq, 4) & 0x40) {
                        c4 = 4;
                    }
                    if (F16(cmp, 4) & 0x30) {
                        if (!(F16(eq, 4) & 0x30)) {
                            c5 = 2;
                        }
                    } else if (F16(eq, 4) & 0x30) {
                        c5 = 4;
                    }
                } else {
                    d2 = Get_equip_data_ptr(cmp);
                    atk2 = ATKCONV(F16(d2, 8), Get_weapon_job(cmp) & 0xFF);
                }
                if (atk2 < atk) {
                    c1 = 4;
                } else if (atk < atk2) {
                    c1 = 2;
                }
            }
            font_set_palette(c1);
            ty = y + 0x28;
            tx = x + 0x6C;
            flfntLocate(tx, ty);
            font_print(lit_3592, atk);
            font_set_palette(c2);
            flfntLocate(tx, y + 0x3C);
            font_print_uf(reload_level_str[F8(d, 3)]);
            font_set_palette(c3);
            flfntLocate(tx, y + 0x50);
            g[0] = 0x81;
            g[2] = 0;
            g[1] = (F16(eq, 4) & 0xF) + 0x50;
            font_print_uf(g);
            font_set_palette(c4);
            flfntLocate(x, y + 0x64);
            if (F16(eq, 4) & 0x40) {
                font_print_uf(lit_4151);
            } else {
                font_print_uf(lit_4152);
            }
            font_set_palette(c5);
            flfntLocate(x + 0xB4, ty);
            if (F16(eq, 4) & 0x20) {
                font_print_uf(lit_4153);
            } else if (F16(eq, 4) & 0x10) {
                font_print_uf(lit_4154);
            }
            str = equip_exp_str_gun;
            break;
        default:
            c1 = 0;
            c2 = 0;
            c3 = 0;
            c4 = 0;
            c5 = 0;
            d = Get_equip_data_ptr(eq);
            if (cmp != 0 && eq[1] == cmp[1]) {
                d2 = Get_equip_data_ptr(cmp);
                if (F8(d2, 8) < F8(d, 8)) {
                    c1 = 4;
                } else if (F8(d, 8) < F8(d2, 8)) {
                    c1 = 2;
                }
                if ((s8)F8(d2, 9) < (s8)F8(d, 9)) {
                    c2 = 4;
                } else if ((s8)F8(d, 9) < (s8)F8(d2, 9)) {
                    c2 = 2;
                }
                if ((s8)F8(d2, 0xA) < (s8)F8(d, 0xA)) {
                    c3 = 4;
                } else if ((s8)F8(d, 0xA) < (s8)F8(d2, 0xA)) {
                    c3 = 2;
                }
                if ((s8)F8(d2, 0xB) < (s8)F8(d, 0xB)) {
                    c4 = 4;
                } else if ((s8)F8(d, 0xB) < (s8)F8(d2, 0xB)) {
                    c4 = 2;
                }
                if ((s8)F8(d2, 0xC) > (s8)F8(d, 0xC)) {
                    c5 = 4;
                } else if ((s8)F8(d, 0xC) > (s8)F8(d2, 0xC)) {
                    c5 = 2;
                }
            }
            font_set_palette(c1);
            ty = y + 0x28;
            tx = x + 0x48;
            flfntLocate(tx, ty);
            font_print(lit_3593, F8(d, 8));
            font_set_palette(c2);
            flfntLocate(tx, y + 0x3C);
            font_print(lit_3593, (u8)F8(d, 9));
            font_set_palette(c3);
            flfntLocate(x + 0xD8, y + 0x3C);
            font_print(lit_3593, (u8)F8(d, 0xA));
            font_set_palette(c4);
            flfntLocate(tx, y + 0x50);
            font_print(lit_3593, (u8)F8(d, 0xB));
            font_set_palette(c5);
            flfntLocate(x + 0xD8, y + 0x50);
            font_print(lit_3593, (u8)F8(d, 0xC));
            str = equip_exp_str_armor;
            break;
        }
        font_set_palette(5);
        font_print_strings(x, ty, str, 0x14);
        return;
    case 1:
        id = F16(eq, 2);
        switch (kind) {
        case 7:
            id += 0xEA;
        case 6:
            font_print_strings(x, y + 0x3C, weapon_exp_str_common, 0x14);
            row = weapon_exp[id];
            break;
        case 0:
            id += 0x44;
        case 5:
            id += 0x4D;
        case 4:
            id += 0x4E;
        case 3:
            id += 0x4B;
        case 2:
            font_print_strings(x, y + 0x3C, armor_exp_str_common, 0x14);
            row = armor_exp[id];
            break;
        }
        font_set_palette(0);
        font_print_strings(x + 0x36, y, row, 0x14);
        flfntLocate(x + 0x90, y + 0x3C);
        if (eq[1] != 7 && eq[1] != 6) {
            u8 b = F8(Get_equip_data_ptr(eq), 2);
            int w;
            if ((b & 0xC) == 0xC) {
                w = 2;
            } else {
                w = 0;
                if (b & 4) {
                } else {
                    w = 1;
                }
            }
            font_print_uf(wearable_tbl[w]);
        } else {
            font_print_uf(wearable_tbl[(eq[1] - 6) & 0xFF]);
        }
        flfntLocate(x + 0x48, y + 0x50);
        if (F16(eq, 2) != 0) {
            font_set_palette(Equip_moji_color_rare(rare));
            g[0] = 0x81;
            g[1] = rare + 0x50;
            g[2] = 0;
            font_print_uf(g);
            return;
        }
        font_print_uf(lit_4155);
        return;
    case 2:
        font_set_palette(0);
        flfntLocate(x, y);
        font_print_uf(lit_4156);
        d = Get_equip_data_ptr(eq);
        flfntLocate(x, y + 0x14);
        c1 = FS32(d, 0x10) & 7;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4157, lv123str[c1]);
        flfntLocate(x, y + 0x28);
        c1 = (FS32(d, 0x10) & 0x38) >> 3;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4158, lv123str[c1]);
        flfntLocate(x, y + 0x3C);
        c1 = (FS32(d, 0x10) & 0x1C0) >> 6;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4159, lv123str[c1]);
        flfntLocate(x, y + 0x50);
        c1 = (FS32(d, 0x10) & 0xE00) >> 9;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4160, lv123str[c1]);
        flfntLocate(x + 0xA2, y + 0x14);
        c1 = (FS32(d, 0x10) & 0x7000) >> 0xC;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4161, lv123str[c1]);
        flfntLocate(x + 0xA2, y + 0x28);
        font_set_palette((FS32(d, 0x10) & 0x10000) ? 5 : 0xA);
        font_print_uf(lit_4162);
        flfntLocate(x + 0xA2, y + 0x3C);
        c1 = (FS32(d, 0x10) & 0x60000) >> 0x11;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4163, lv12str[c1]);
        flfntLocate(x + 0xA2, y + 0x50);
        c1 = (FS32(d, 0x10) & 0x180000) >> 0x13;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4164, lv12str[c1]);
        return;
    case 3:
        font_set_palette(0);
        flfntLocate(x, y);
        font_print_uf(lit_4156);
        d = Get_equip_data_ptr(eq);
        flfntLocate(x, y + 0x14);
        c1 = (FS32(d, 0x10) & 0x600000) >> 0x15;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4165, lv12str[c1]);
        flfntLocate(x, y + 0x28);
        c1 = (FS32(d, 0x10) & 0x1800000) >> 0x17;
        font_set_palette(c1 != 0 ? 5 : 0xA);
        font_print_sp(lit_4166, lv12str[c1]);
        flfntLocate(x, y + 0x3C);
        font_set_palette((FS32(d, 0x10) & 0x2000000) ? 5 : 0xA);
        font_print_uf(lit_4167);
        flfntLocate(x, y + 0x50);
        font_set_palette((FS32(d, 0x10) & 0x4000000) ? 5 : 0xA);
        font_print_uf(lit_4168);
        flfntLocate(x, y + 0x64);
        font_set_palette((FS32(d, 0x10) & 0x8000000) ? 5 : 0xA);
        font_print_uf(lit_4169);
        flfntLocate(x + 0xA2, y + 0x14);
        font_set_palette((FS32(d, 0x10) & 0x10000000) ? 5 : 0xA);
        font_print_uf(lit_4170);
        flfntLocate(x + 0xA2, y + 0x28);
        font_set_palette((FS32(d, 0x10) & 0x20000000) ? 5 : 0xA);
        font_print_uf(lit_4171);
        flfntLocate(x + 0xA2, y + 0x3C);
        font_set_palette((FS32(d, 0x10) & 0x40000000) ? 5 : 0xA);
        font_print_uf(lit_4172);
        flfntLocate(x + 0xA2, y + 0x50);
        font_set_palette((FS32(d, 0x10) & 0x80000000) ? 5 : 0xA);
        font_print_uf(lit_4173);
        return;
    }
}

extern char *equip_exp_str_sw_attr[];
extern char lit_4221[];
extern char lit_4222[];

static void sword_zokusei(u8 *w, int x, s16 y) {
    int k;

    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    k = -1;
    if (w[0xB] != 0) {
        k = 0;
    } else if (w[0xC] != 0) {
        k = 1;
    } else if (w[0xD] != 0) {
        k = 2;
    } else if (w[0xE] != 0) {
        k = 3;
    } else if (w[0xF] != 0) {
        k = 4;
    } else if (w[0x10] != 0) {
        k = 5;
    } else if (w[0x11] != 0) {
        k = 6;
    }
    if (k >= 0) {
        flfntLocate_s(x, y);
        font_print(lit_4221, equip_exp_str_sw_attr[k]);
        y += 0x14;
    }
    if (w[0xA] != 0) {
        flfntLocate_s(x, y);
        font_print(lit_4222, (int)w[0xA]);
    }
}

void EquipmentCompareWindowA(u8 *cur, u8 *other, s16 x, s16 y, int page, int alpha);

void EquipmentCompareWindow(u8 *cur, u8 *other, s16 x, s16 y, int page) {
    EquipmentCompareWindowA(cur, other, x, y, page, 0xB2);
}

void EquipmentCompareWindowA(u8 *cur, u8 *other, s16 x, s16 y, int page, int alpha) {
    u32 t;
    f32 s;

    EquipmentDescriptionWindowA_s((EQD *)cur, x, y, page, 0, alpha);
    EquipmentDescriptionWindowA_s((EQD *)other, x, y + 0x90, page, cur, alpha);
    SetFilterMode(1);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    t = (u16)((System_timer & 0x1F) << 11);
    s = flSin(0.0000958738f * (f32)t);
    PutArrow(x + 0x89, y + 0x7A, 0x20, 0x10,
             (((((s8)(96.0f * s) + 0x90) & 0xFF) << 16) | 0xFF000000 | ((((s8)(20.0f * s) + 0xE4) & 0xFF) << 8)) | (((s8)(7.0f * s) + 0xF7) & 0xFF), 3);
}

extern u8 Battle_type[];
extern s16 *Pl_slash_tbl[];
extern int slash_bar_color[];
f32 flps0009(void *);

typedef struct PFLP9 { s16 x0, y0, x1, y1, x2, y2; u32 col; } PFLP9;

static void slash_level_bar(u8 *pl, s16 y, f32 x) {
    PFLP9 a;
    PFLP4 b;
    f32 xr;
    f32 xs;
    s16 *seg;
    int *col;
    s16 yy;
    int i;

    yy = y - 3;
    xr = x - 36.0f;
    a.col = 0xFF968A63;
    a.x0 = 0.8f * xr;
    a.y1 = yy;
    a.y0 = yy + 11;
    a.x1 = a.x2 = 0.8f * (5.0f + xr);
    a.y2 = yy + 21;
    flps0009(&a);
    a.x0 = 0.8f * (140.0f + xr);
    a.x1 = a.x2 = 0.8f * (135.0f + xr);
    flps0009(&a);
    b.p[0] = 0.8f * (5.0f + xr);
    b.p[2] = 0.8f * (130.0f + (5.0f + xr));
    b.p[1] = yy;
    b.p[3] = b.p[1] + 0x15;
    b.col = 0xFF968A63;
    flps0004(&b);
    b.p[0] = 0.8f * (6.0f + xr);
    b.p[2] = 0.8f * (128.0f + (6.0f + xr));
    b.p[1] = yy + 2;
    b.p[3] = b.p[1] + 0x11;
    b.col = 0xFF000000;
    flps0004(&b);
    xs = 8.0f + xr;
    col = slash_bar_color;
    seg = Pl_slash_tbl[Battle_type[F8(pl, 0)]] + F8(pl, 2) * 4;
    b.p[1] = yy + 4;
    b.p[3] = yy + 0x11;
    b.p[2] = 0.8f * xs;
    for (i = 0; i < 4; i++, seg += 2, col++) {
        b.p[0] = b.p[2];
        b.p[2] = 0.8f * (xs + 0.41333333f * (f32)*seg);
        b.col = *col;
        flps0004(&b);
    }
    if (F8(pl, 3) < 3) {
        b.p[0] = 0.8f * (xs + 0.41333333f * (150.0f + (f32)(F8(pl, 3) * 0x32)));
        b.p[2] = 0.8f * (124.0f + xs);
        b.col = 0xFF000000;
        flps0004(&b);
    }
}

extern char lit_4368[];

void Put_PageArrow(int x, int y, int a, int b) {
    PFLP8 q;

    q.p[2] = 0xE;
    q.p[3] = 0x12;
    q.p[1] = y;
    q.col = 0xFF20FF30;
    q.p[0] = 0.8f * (f32)((s16)x - 0x18);
    *(u32 *)&q.uv[0] = 0x1A00A6;
    *(u32 *)&q.uv[2] = 0x2E0094;
    flps0008(&q);
    q.p[0] = 0.8f * (f32)((s16)x + 0x36);
    q.uv[0] = 0x94;
    q.uv[2] = 0xA6;
    flps0008(&q);
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    flfntLocate_i(x, y);
    font_print(lit_4368, a & 0xFF, b & 0xFF);
}

extern u8 lit_4374[];
extern u8 setumei_shousai_4372[8];
void font_print_ex(int, int, int, void *);

void Put_shousai(void) {
    flfntSetSize(0x12, 0x12);
    font_print_ex(0x22E, 0x18A, 0, lit_4374);
    PutButtonICON(setumei_shousai_4372, 1);
}

extern u8 equip_icon_u_tbl[8];
extern u8 weapon_icon_u_tbl[8];

void Get_equip_icon_uv(u8 *eq, s16 *a, s16 *b) {
    u8 k = eq[1];

    if (k == 6 || k == 7) {
        a[0] = weapon_icon_u_tbl[Get_weapon_job(eq) & 0xFF] + 1;
        b[0] = a[0] + 0x1E;
        a[1] = 0xA1;
        b[1] = 0xBF;
    } else {
        a[0] = equip_icon_u_tbl[k] + 1;
        b[0] = a[0] + 0x1E;
        a[1] = 0xC1;
        b[1] = 0xDF;
    }
}

extern char *ng_word_tbl_0[];
extern char *ng_word_tbl_2[];
static int ng_word_sub(char *, char *, s8);

void KinshiYogo_chk(char *s) {
    char **p;

    p = ng_word_tbl_0;
    do {
        ng_word_sub(s, *p, 0);
        p++;
    } while (*p != 0);
    p = ng_word_tbl_2;
    do {
        ng_word_sub(s, *p, 2);
        p++;
    } while (*p != 0);
}

u32 strlen(const char *);
char *strstr(const char *, const char *);
static int zen_kigou_suuji_chk(u8 *);

static int ng_word_sub(char *text, char *ng, s8 mode) {
    char *p;
    char *rest;
    int len;
    int hit;

    p = strstr(text, ng);
    rest = text;
    if (p != 0) {
        do {
            switch (mode) {
            case 0:
                len = strlen(ng);
                hit = 1;
                break;
            case 1:
                len = strlen(ng);
                hit = 1;
                break;
            case 2:
                hit = 0;
                len = strlen(ng);
                if (p == text) {
                    hit = 1;
                } else {
                    if (zen_kigou_suuji_chk((u8 *)p - 2) == 1) {
                        goto set;
                    }
                    if (p + len == rest + strlen(rest)) {
                        hit = 1;
                    } else if (zen_kigou_suuji_chk((u8 *)(p + len)) == 1) {
set:
                        hit = 1;
                    }
                }
                break;
            }
            rest = p + 1;
            if (hit != 0) {
                len >>= 1;
                while (len > 0) {
                    p[0] = 0x81;
                    p[1] = 0x96;
                    len--;
                    p += 2;
                }
                rest = p;
            }
            p = strstr(rest, ng);
        } while (p != 0);
    }
    return 0;
}

static int zen_kigou_suuji_chk(u8 *p) {
    u8 c = p[0];

    if (c == 0x81 && p[1] >= 0x40 && p[1] < 0xED) {
        return 1;
    }
    if (c == 0x82 && p[1] >= 0x4F && p[1] < 0x59) {
        return 1;
    }
    return 0;
}

int ChatKinsoku_chk(u8 *s) {
    u8 *p = s;
    u8 c = *p;

    while (c != 0) {
        if (c != 0x81 || p[1] != 0x40) {
            KinshiYogo_chk((char *)s);
            return 1;
        }
        p += 2;
        c = *p;
    }
    return 0;
}

extern u8 default_reibun[];
void *memcpy(void *, const void *, int);
void Init_reibun(void);

void Default_reibun_set(void) {
    memcpy((u8 *)&option_w + 0xDB0, default_reibun, 0x21C);
    Init_reibun();
}

static void init_reibun_sub(s8 *d, s8 *s) {
    u32 n;
    s8 *top = d;

    n = 6;
    while (1) {
        d[0] = s[0];
        if (s[0] == 0) {
            break;
        }
        n--;
        d[1] = s[1];
        s += 2;
        d += 2;
        if (n == 0) {
            top[8] = 0x81;
            top[9] = 0x64;
            top[10] = 0;
            break;
        }
    }
}

typedef struct REIBUN { s8 *s[3]; s8 *edit; } REIBUN;
extern REIBUN str_tbl_reibun0[];

void Init_reibun(void) {
    REIBUN *r = str_tbl_reibun0;
    int n = 12;

    do {
        init_reibun_sub((s8 *)r, r->edit);
        r++;
    } while (--n != 0);
}

static void chcnfg_reibun_set(s8 *src, int no) {
    REIBUN *r = &str_tbl_reibun0[no & 0xFF];
    u32 n = 0x16;
    s8 *d = r->edit;
    s8 *s = src;
    s8 *t;
    u32 m;
    s8 c;

    do {
        c = *s;
        if (c == 0) {
            break;
        }
        *d = c;
        n--;
        d[1] = s[1];
        s += 2;
        d += 2;
    } while (n != 0);
    *d = 0;
    t = (s8 *)r;
    s = src;
    m = 6;
    do {
        t[0] = s[0];
        if (s[0] == 0) {
            break;
        }
        m--;
        t[1] = s[1];
        s += 2;
        t += 2;
        if (m == 0) {
            ((u8 *)r)[8] = 0x81;
            ((s8 *)r)[9] = 0x64;
            ((s8 *)r)[10] = 0;
            break;
        }
    } while (1);
}

int softkey_ck();

int Reibun_Edit_Start(int no) {
    if (softkey_ck() == 0) {
        return 0;
    }
    SoftKeyboard_pos_set(80.0f, 0x50);
    SoftKeyboard_set(2, 0, 0x2C, (int)str_tbl_reibun0[no & 0xFF].edit);
    se_req(7, 0x11, 0);
    return 1;
}

int Reibun_Edit_Core(int no) {
    s8 buf[0x30];
    s8 r;

    buf[0] = 0;
    r = SoftKeyboard_move(buf, *(s16 *)((u8 *)&Psw + 0), *(s16 *)((u8 *)&Psw + 4));
    if (r != 0) {
        if (buf[0] != 0 && r > 0) {
            chcnfg_reibun_set(buf, no);
        }
        SoftKeyboard_exit();
        return 1;
    }
    return 0;
}
