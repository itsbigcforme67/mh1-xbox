/* Lobby browser drawing primitives (clipped rectangle / line helpers), 0x005DBA80-0x005DD3E0. Hand-written from the asm. */
#include "lobby_f.h"
void BsDrawRectangle(f32, f32, f32, f32, u32);
void BsDrawTriangle();
int chopLine(f32 *, f32 *, f32 *, f32 *);
extern BSSYS *bsSys;
extern s16 BsOuterTexWidth[20];
extern s16 BsOuterTexHeight[20];
extern s32 BsOuterTexHdl[20];
extern char lit_615_00665F40[];
void BsDrawSprite(int, int, int, int, int, int, int, int, s16, s16);
void SetFilterMode();
void flfntStackReset();
void flfntSetHalftype();
void flfntSetSize();
void flfntSetPalette();
void flfntLocate();
void flfntPrintf();
void flfntDrawAll();
char *strchr(const char *, int);

/* clamp the corners to the 640x448 screen (the y0 > 640 test is what the code does) and draw a filled rectangle */
void fillRect(f32 x0, f32 y0, f32 x1, f32 y1, u32 color) {
    if (x0 < 0.0f) {
        x0 = 0.0f;
    }
    if (y0 < 0.0f) {
        y0 = 0.0f;
    }
    if (x1 > 640.0f) {
        x1 = 640.0f;
    }
    if (y1 > 448.0f) {
        y1 = 448.0f;
    }
    if (x0 > 640.0f) {
        x0 = 640.0f;
    }
    if (y0 > 640.0f) {
        y0 = 640.0f;
    }
    if (x1 < 0.0f) {
        x1 = 0.0f;
    }
    if (y1 < 0.0f) {
        y1 = 0.0f;
    }
    BsDrawRectangle(x0, y0, x1, y1, color);
}

/* filled triangle: skipped when a vertex is off the screen */
void fillTrgl(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2) {
    if (!(x2 < 0.0f) && !(y2 < 0.0f) && x0 <= 640.0f && y0 <= 448.0f) {
        BsDrawTriangle();
    }
}

/* horizontal / vertical line: clip with chopLine, then a one pixel wide rectangle */
void drawHLine(int color, f32 x0, f32 y0, f32 x1, f32 y1) {
    f32 d, c, b, a;
    a = x0;
    b = y0;
    c = x1;
    d = y1;
    if (chopLine(&a, &b, &c, &d) != 0) {
        BsDrawRectangle(a, b, 1.0f + c, d, color);
    }
}

void drawVLine(int color, f32 x0, f32 y0, f32 x1, f32 y1) {
    f32 d, c, b, a;
    a = x0;
    b = y0;
    c = x1;
    d = y1;
    if (chopLine(&a, &b, &c, &d) != 0) {
        BsDrawRectangle(a, b, c, 1.0f + d, color);
    }
}

/* outline of a rectangle (one pixel wide), clipped against the page margins in bsSys */
void drawRect(int color, f32 x0, f32 y0, f32 x1, f32 y1) {
    if (x0 < (f32)bsSys->x1E) {
        x0 = (f32)bsSys->x1E;
    }
    if (y0 < (f32)bsSys->x1A) {
        y0 = (f32)bsSys->x1A;
    }
    if (x1 > (f32)(640 - bsSys->x20)) {
        x1 = (f32)(s16)(640 - bsSys->x20);
    }
    if (y1 > (f32)(448 - bsSys->x1C)) {
        y1 = (f32)(s16)(448 - bsSys->x1C);
    }
    BsDrawRectangle(x0, y0, x1, 1.0f + y0, color);
    BsDrawRectangle(x1 - 1.0f, y0, x1, y1, color);
    BsDrawRectangle(x0, y1 - 1.0f, x1, y1, color);
    BsDrawRectangle(x0, y0, 1.0f + x0, y1, color);
}

/* draw one string with the font library (the % signs would be format codes: replaced by blanks) */
void drawString(int pal, int a1, int a2, f32 x, f32 y, int size, u8 *s) {
    char *p;
    if (*s == 0 || (char *)s == lit_615_00665F40) {
        return;
    }
    SetFilterMode(1);
    flfntStackReset();
    flfntSetHalftype(1);
    flfntSetSize(size, size);
    flfntSetPalette(pal & 0xFF);
    flfntLocate((int)x, (int)y);
    for (;;) {
        p = strchr((char *)s, 0x25);
        if (p == 0) {
            break;
        }
        *p = 0x20;
    }
    flfntPrintf((char *)s);
    flfntDrawAll();
}

void drawOuterImage(u16 idx, f32 x, f32 y, f32 w, f32 h) {
    BsDrawSprite(-1, BsOuterTexHdl[idx], (int)x, (int)y, (int)w, (int)h, 0, 0, BsOuterTexWidth[idx], BsOuterTexHeight[idx]);
}

/* browser http state (0x5DB9C0): either an error page or load the model file named in the url */
int atoi();
void load_file_mdl();
int afs_file_length();
void http_test_18(u8 *p) {
    int id;
    if (atoi(*(char **)(p + 4) + 6) != 2) {
        *(s8 *)(p + 0x3D) = 1;
        *(s8 *)(p + 0x3C) = 0;
        *(s8 *)(p + 0x40) = 0x14;
        return;
    }
    id = atoi(*(char **)(p + 4) + 9);
    load_file_mdl(*(int *)(p + 0x10), id + 0x886);
    *(int *)(p + 0x38) = afs_file_length((id + 0x886) | 0x20000);
    *(u8 *)(p + 0x40) = *(u8 *)(p + 0x40) + 1;
}

typedef struct BSQUAD { s16 x, y, x2, y2; s32 color; } BSQUAD;
void BsSetRenderState();
void net_flps0004();

/* filled rectangle through the GS packet helper (coordinates are truncated to s16) */
void BsDrawRectangle(f32 x0, f32 y0, f32 x1, f32 y1, u32 color) {
    BSQUAD q;
    BsSetRenderState(1, 0);
    q.x = (s16)x0;
    q.y = (s16)y0;
    q.x2 = (s16)x1;
    q.y2 = (s16)y1;
    q.color = color;
    net_flps0004(&q);
}

typedef struct BSSPR { s16 x, y, w, h; s32 color; s16 u0, v0, u1, v1; } BSSPR;
void nb_flps0009();
void net_flps0008();
void flSetRenderState();

/* textured sprite: uv rectangle (u0, v0)-(u1 - 1, v1 - 1) of the texture `tex` */
void BsDrawSprite(int color, int tex, int x, int y, int w, int h, int u0, int v0, s16 u1, s16 v1) {
    BSSPR q;
    flSetRenderState(0x6C, 0);
    SetFilterMode(0);
    BsSetRenderState(4, tex);
    q.color = color;
    q.x = x;
    q.y = y;
    q.w = w;
    q.h = h;
    q.u1 = u1 - 1;
    q.v1 = v1 - 1;
    q.u0 = u0;
    q.v0 = v0;
    net_flps0008(&q);
}
