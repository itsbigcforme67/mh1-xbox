/* G online (PC): lobby_func_00578C40, written from Monster Hunter G's code at lobby 0x578C40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_2TF();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_set_palette();
s32 han2zen();
s32 reload_tex();
extern u8 lb_shop_msg;
extern u8 textLobbyTbl;
extern u8 tl_mail_tbl;
s32 font_print();           /* extern */
s32 sprintf(); /* extern */
extern u8 ClassInfo;
extern s32 g_main_306468;
extern u8 lit_2316;
extern u8 lit_3241;

void lobby_func_00578C40(s16 arg0) {
    u8 g_frame[0xB0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s32 *)(g_frame + 0x30))
#define sp70 (*(s32 *)(g_frame + 0x70))
#define sp90 (*(s16 *)(g_frame + 0x90))
#define sp92 (*(s16 *)(g_frame + 0x92))

    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    (*(f32 *)((u8 *)(&sp90) + 0)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&textLobbyTbl + 0x14))) + 0));
    (*(f32 *)((u8 *)(&sp90) + 4)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&textLobbyTbl + 0x14))) + 4));
    (*(f32 *)((u8 *)(&sp90) + 8)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&textLobbyTbl + 0x14))) + 8));
    (*(f32 *)((u8 *)(&sp90) + 0xC)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&textLobbyTbl + 0x14))) + 0xC));
    (*(f32 *)((u8 *)(&sp90) + 0x10)) = (f32) (*(f32 *)((u8 *)(&(*(u8 *)((u8 *)&textLobbyTbl + 0x14))) + 0x10));
    sp92 = arg0;
    Put_2TF(&sp90);
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    sprintf(&sp70, &lit_3241, (*(s32 *)((u8 *)&tl_mail_tbl + 0x34)), (*(s32 *)((u8 *)&g_main_306468 + (ClassInfo * 0x15C))), (*(s32 *)((u8 *)&lb_shop_msg + 0x8)));
    han2zen(&sp70, &sp30);
    flfntLocate((s32) (((s16) ((sp90 + 0xA)))), (s32) (((s16) ((sp92 + 3)))));
    font_print(&lit_2316, &sp30);
}

