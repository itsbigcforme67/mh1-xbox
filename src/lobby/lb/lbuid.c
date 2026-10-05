/* lbui, run 4: DispSceneTitle .. DispSceneSubTitle (lobby.bin 0x00593070-0x005931AC): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

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
