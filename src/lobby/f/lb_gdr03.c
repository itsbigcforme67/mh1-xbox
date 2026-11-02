/* lb_gdr03 - browser draw 0x005DCAB0-0x005DCB38: drawOuterImage. Whole file in lb_dr.c. */
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

void drawOuterImage(u16 idx, f32 x, f32 y, f32 w, f32 h) {
    BsDrawSprite(-1, BsOuterTexHdl[idx], (int)x, (int)y, (int)w, (int)h, 0, 0, BsOuterTexWidth[idx], BsOuterTexHeight[idx]);
}
