/* lbui, run 4: DispSceneTitle .. DispSceneSubTitle (lobby.bin 0x00593070-0x005931AC): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"


/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void DispSceneTitle(void) {
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(helpLineTbl);
    if (pSceneTitle != 0) {
        flfntSetSize(0x1E, 0x1E);
        font_print_double(pSceneTitle->x, pSceneTitle->y, 1, 0, pSceneTitle->s);
    }
}

void DispSceneSubTitle(void) {
    flfntSetSize(0x12, 0x12);
    if (CW->x35D5 != 0 && *(s8 *)(game_w.master + (int)cw + 0x2BFE) != 0) {
        Draw_menu_square(0xD4, 0x30, 0xC0, 0x20, 0, 0);
        font_print_double(pSceneSubTitle->x, 0x38, 1, 0, pSceneSubTitle->s);
        return;
    }
    Draw_menu_square(0xD4, 0x44, 0xC0, 0x20, 1, subTitleCol);
    font_print_double(pSceneSubTitle->x, pSceneSubTitle->y, 1, 0, pSceneSubTitle->s);
}
