/* lb_lmp01 - agent C 0x005B2CD0-0x005B2E3C: lm_place_trans (lobby member window: place panel; 5-float struct copy from netr_sub01_tbl, TF record declared before the text buffer). */
#define flfntLocate flfntLocate_hdr
#define font_print_double font_print_double_hdr
#include "lobby_a.h"
#undef flfntLocate
#undef font_print_double
void font_print_double(int, int, int, int, char *);
extern char lit_219_0065E680[];
extern char netr_sub01_tbl[];
extern char *lm_menu2_tbl;
typedef struct { f32 f[5]; } F5;
typedef struct { s16 x, y, w, h; s32 col; u8 pad[0x10]; } TF;
void lm_place_trans(void) {
    TF t;
    char buf[0x40];

    *(F5 *)&t = *(F5 *)(netr_sub01_tbl + 0x14);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Draw_menu_square(0x190, 0x38, 0xDA, 0x104, 0, 0);
    Paint_square(0x196, 0x40, 0xCE, 0x1A, 0x30FFFFFF);
    flfntSetSize(0x14, 0x14);
    font_print_double(0x19A, 0x42, 1, 5, lm_menu2_tbl);
    sprintf(buf, lit_219_0065E680, Get_ServerName());
    font_print_double(0x19A, 0x62, 1, 0, buf);
    Get_PlazaName(buf);
    font_print_double(0x19A, 0x100, 1, 0, buf);
    Get_LobbyName(buf);
    font_print_double(0x19A, 0x118, 1, 0, buf);
    t.x = 0x1C2;
    t.y = 0x7E;
    t.w = 0x78;
    t.h = 0x78;
    t.col = Get_ServerColor();
    Put_2TF(&t);
}
