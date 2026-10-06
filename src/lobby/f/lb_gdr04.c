/* lb_gdr04 - browser draw 0x005DCE80-0x005DCF60: drawHLine, drawVLine. Whole file in lb_dr.c. */
#include "lobby_f.h"
void BsDrawRectangle(f32, f32, f32, f32, u32);
void BsDrawTriangle();
int chopLine(f32 *, f32 *, f32 *, f32 *);
extern BSSYS *bsSys;
extern s16 BsOuterTexWidth[20];
extern s16 BsOuterTexHeight[20];
extern s32 BsOuterTexHdl[20];
extern char lit_615_00665F40[];
void BsDrawSprite(int, int, int, int, int, int, int, int, int, int);
void SetFilterMode();
void flfntStackReset();
void flfntSetHalftype();
void flfntSetSize();
void flfntSetPalette();
void flfntLocate();
void flfntPrintf();
void flfntDrawAll();
char *strchr(const char *, int);








/* browser http state (0x5DB9C0): either an error page or load the model file named in the url */
int atoi();
void load_file_mdl();
int afs_file_length();

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
