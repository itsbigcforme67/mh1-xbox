/* G online (PC): transSelectHandleName, written from Monster Hunter G's code at lobby 0x5BAA70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 net_etc_mes_tbl;
extern u8 textLobbyTbl;
s32 DispFrameMessage();          /* extern */
s32 DispHelpLine();                             /* extern */
s32 DispSceneTitle();                           /* extern */
s32 Get_sw2();                           /* extern */
s32 Lb_put_button();   /* extern */
s32 Put_2TF();                         /* extern */
s32 SetFilterMode();                     /* extern */
s32 SetTextureStage();                   /* extern */
s32 disp_string_handle();           /* extern */
s32 disp_string_id();               /* extern */
s32 flSetRenderState();         /* extern */
s32 flfntLocate();              /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print();            /* extern */
s32 font_print_double(); /* extern */
s32 font_set_palette();                  /* extern */
s32 lobby_func_005926C0(); /* extern */
s32 reload_tex(); /* extern */
extern u8 g_lb_636EC0;
extern u8 g_lb_636EC4;
extern u8 g_lb_645018;
extern s128 lit_324_00617F70;
extern s128 lit_326_00617F80;

void transSelectHandleName(void *arg0) {
    s16 sp42;
    union { s128 v; u8 b[0x10]; } u_sp40;
#define sp40 (u_sp40.v)
    union { s128 v; u8 b[0x20]; } u_sp30;
#define sp30 (u_sp30.v)
    s32 temp_a1;
    s32 temp_a1_2;
    s32 var_s1;

    Get_sw2(0);
    sp40 = lit_324_00617F70;
    sp30 = lit_326_00617F80;
    reload_tex(1, 0x174, lit_324_00617F70, &sp40);
    SetTextureStage(0x174);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(&(*(u8 *)((u8 *)&textLobbyTbl + 0x28)));
    DispFrameMessage(&sp40, 0);
    sp42 += 0x58;
    DispFrameMessage(&sp40, 0);
    sp42 += 0x58;
    DispFrameMessage(&sp40, 0);
    flfntSetSize(0x14, 0x14);
    font_print_double(0x32, 0x6A, 1, 5);
    DispFrameMessage(&sp30, 0);
    font_print_double(0x34, 0x90, 1, 0);
    flfntSetSize(0x18, 0x14);
    font_print_double(0x70, 0x90, 1, 0);
    var_s1 = 0;
    do {
        temp_a1 = var_s1 & 0xFF;
        disp_string_id((*(u8 *)((u8 *)(arg0) + 0x29)), temp_a1, temp_a1);
        temp_a1_2 = var_s1 & 0xFF;
        disp_string_handle((*(u8 *)((u8 *)(arg0) + 0x29)), temp_a1_2, temp_a1_2);
        var_s1 += 1;
    } while (var_s1 < 3);
    DispHelpLine();
    DispSceneTitle();
    lobby_func_005926C0(0x17C, 0x1C, 0x84, 0x30);
    Lb_put_button(0x216, 0x1C, 0);
    Lb_put_button(0x216, 0x34, 1);
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    flfntLocate(0x230, 0x1F);
    font_print(&g_lb_645018, (*(u8 *)((u8 *)&net_etc_mes_tbl + 0x18)), g_lb_636EC0);
    flfntLocate(0x230, 0x37);
    font_print(&g_lb_645018, (*(u8 *)((u8 *)&net_etc_mes_tbl + 0x18)), g_lb_636EC4);
}

