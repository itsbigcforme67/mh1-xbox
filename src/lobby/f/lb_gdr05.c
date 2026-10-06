/* lb_gdr05 - browser draw 0x005DD3E0-0x005DD4C8: drawString. Whole file in lb_dr.c. */
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
