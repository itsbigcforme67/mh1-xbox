/* G online (PC): lm_place_trans, written from Monster Hunter G's code at lobby 0x58BD90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Paint_square();
s32 Put_2TF();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_print_double();
s32 font_set_palette();
s32 reload_tex();
extern u8 lm_menu2_tbl;
extern u8 net_etc_mes_tbl;
extern u8 netr_sub01_tbl;
s32 Draw_menu_square(); /* extern */
s32 Get_LobbyName();                   /* extern */
s32 Get_PlazaName();                   /* extern */
s32 Get_ServerColor();                              /* extern */
s32 Get_ServerName();                               /* extern */
s32 func_0012BDD0();              /* extern */
s32 lobby_func_005C2120();                       /* extern */
s32 sprintf();    /* extern */
extern s32 g_lb_63E878;
extern s32 lit_519_0065E690;

void lm_place_trans(s32 unused_a0, s32 unused_a1, s32 arg2) {
    u8 g_frame[0x70] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s32 *)(g_frame + 0x10))
#define sp50 (*(s16 *)(g_frame + 0x50))
#define sp52 (*(s16 *)(g_frame + 0x52))
#define sp54 (*(s16 *)(g_frame + 0x54))
#define sp56 (*(s16 *)(g_frame + 0x56))
#define sp58 (*(s32 *)(g_frame + 0x58))

    (*(f32 *)((u8 *)(&sp50) + 0)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&netr_sub01_tbl + 0x14))) + 0));
    (*(f32 *)((u8 *)(&sp50) + 4)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&netr_sub01_tbl + 0x14))) + 4));
    (*(f32 *)((u8 *)(&sp50) + 8)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&netr_sub01_tbl + 0x14))) + 8));
    (*(f32 *)((u8 *)(&sp50) + 0xC)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&netr_sub01_tbl + 0x14))) + 0xC));
    (*(f32 *)((u8 *)(&sp50) + 0x10)) = (f32) (*(f32 *)((u8 *)(&(*(s32 *)((u8 *)&netr_sub01_tbl + 0x14))) + 0x10));
    flSetRenderState(0x60, 0, arg2);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    Draw_menu_square(0x190, 0x38, 0xDA, 0x104, 0, 0xA0000000);
    Paint_square(0x196, 0x40, 0xCE, 0x16, 0x30FFFFFF);
    flfntSetSize(0x12, 0x12);
    font_set_palette(5);
    flfntLocate(0x196, 0x40);
    func_0012BDD0(&lit_519_0065E690, (*(s32 *)&lm_menu2_tbl));
    flfntSetSize(0x14, 0x14);
    sprintf(&sp10, &g_lb_63E878, Get_ServerName(), (*(s32 *)((u8 *)&net_etc_mes_tbl + 0xC)));
    font_print_double(0x19A, 0x62, 1, 0, &sp10);
    Get_PlazaName(&sp10);
    font_print_double(0x19A, 0x100, 1, 0, &sp10);
    Get_LobbyName(&sp10);
    font_print_double(0x19A, 0x118, 1, 0, &sp10);
    sp50 = 0x1C2;
    sp52 = 0x7E;
    sp54 = 0x78;
    sp56 = 0x78;
    sp58 = lobby_func_005C2120(Get_ServerColor());
    Put_2TF(&sp50);
}

