/* lb_gdr02 - browser draw 0x005DBA80-0x005DBBD4: fillRect, fillTrgl. Whole file in lb_dr.c. */
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
