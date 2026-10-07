/* lb_pz03 - lobby.bin 0x00599B10-0x00599CB0: Lbs_plaza_trans, draws the plaza main menu (menu tables, new-mail mark, frame list, page number and the 11 rows' backdrops). The box struct is the Put_F argument (x,y,w,h, colour); statement order of its initialisers found by search. */
#pragma readonly_strings on
#include "lbui_proto.h"
int DispFrameListA();
int Put_page_num();
int SetFilterMode();
int SetTextureStage();
void Lb_put_new_mail();

void Lbs_plaza_trans(a)
u8 *a;
{
    u8 *p = plazaMenuTbl[pNet->menu];
    struct { s16 x; s16 y; s16 w; s16 h; s32 col; } box;
    int i;

    font_set_stack_no(*(s32 *)(a + 0x18));
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    put_plaza_menu(pNet);
    reload_tex(1, 0x154);
    SetTextureStage(0x154);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(textLobbyTbl);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    Lb_put_new_mail(0x1AA, 0x1C);
    if (pNet->depth < 2) {
        DispFrameListA(pfl_menu_2138, 0, pNet->cur, 0xFF);
    } else {
        DispFrameListA(pfl_menu_2138, 0, -1, 0xFF);
    }
    Put_page_num(0x82, 0x56, pNet->menu, 2, 0);
    box.col = 0x30FFFFFF;
    box.x = 0x16;
    i = 0;
    box.w = 0xC6;
    box.y = 0x57;
    do {
        box.h = box.y + 0x12;
        if (*p == 0) {
            Put_F(&box);
        }
        i++;
        p += 0x24;
        box.y += 0x16;
    } while (i < 0xB);
}
