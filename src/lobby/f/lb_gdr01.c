/* lb_gdr01 - browser draw 0x005DB9C0-0x005DBA54: http_test_18. Whole file in lb_dr.c. */
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
