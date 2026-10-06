#include "lobby_a.h"
extern char plazaMenuTbl[];
extern char textLobbyTbl[];
extern char pfl_menu_2138[];
extern char pfl_menu_2138[];
void Lbs_plaza_trans(int arg0) {
    s32 sp58;
    s16 sp56;
    s16 sp54;
    s16 sp52;
    s16 sp50;
    s32 var_s2;
    int var_s3;
    int temp_v1;

    var_s3 = ((int *)&plazaMenuTbl)[F(u8, pNet, 8)];
    font_set_stack_no(F(s32, arg0, 0x18));
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    put_plaza_menu(pNet);
    reload_tex(1, 0x154);
    SetTextureStage(0x154);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(&textLobbyTbl);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    Lb_put_new_mail(0x1AA, 0x1C);
    temp_v1 = (int)pNet;
    if (F(u8, temp_v1, 2) < 2) {
        DispFrameListA(&pfl_menu_2138, 0, F(u8, temp_v1, 9), 0xFF);
    } else {
        DispFrameListA(&pfl_menu_2138, 0, -1U, 0xFF);
    }
    Put_page_num(0x82, 0x56, F(u8, pNet, 8), 2);
    sp50 = 0x16;
    var_s2 = 0;
    sp58 = 0x30FFFFFF;
    sp54 = 0xC6;
    sp52 = 0x57;
    do {
        sp56 = sp52 + 0x12;
        if ((*(u8 *)var_s3) == 0) {
            Put_F(&sp50);
        }
        var_s2 += 1;
        var_s3 += 0x24;
        sp52 += 0x16;
    } while (var_s2 < 0xB);
}
