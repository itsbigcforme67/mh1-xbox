/* G online (PC): Lbs_plaza_trans, written from Monster Hunter G's code at lobby 0x573820 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispFrameListA();
s32 Lb_put_new_mail();
s32 Put_2TF();
s32 Put_F();
s32 Put_page_num();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_set_palette();
s32 font_set_stack_no();
s32 han2zen();
s32 put_plaza_menu();
s32 reload_tex();
extern u8 pNet;
s32 font_print();                      /* extern */
s32 sprintf();         /* extern */
extern s32 g_lb_63E1D8;
extern s32 g_main_2EEE70;
extern u8 pfl_menu_2138;
extern u8 textLobbyTbl;

void Lbs_plaza_trans(void *arg0) {
    u8 g_frame[0xB0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp50 (*(s32 *)(g_frame + 0x50))
#define sp80 (*(s32 *)(g_frame + 0x80))
#define spA0 (*(s16 *)(g_frame + 0xA0))
#define spA2 (*(s16 *)(g_frame + 0xA2))
#define spA4 (*(s16 *)(g_frame + 0xA4))
#define spA6 (*(s16 *)(g_frame + 0xA6))
#define spA8 (*(s32 *)(g_frame + 0xA8))
    s32 var_s2;
    u8 *var_s3;
    u8 *temp_v1;

    var_s3 = (*(s32 *)((u8 *)&g_main_2EEE70 + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) * 4)));
    (*(s8 *)((u8 *)&pfl_menu_2138 + 0x7)) = 0xC;
    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    put_plaza_menu();
    reload_tex(1, 0x17B);
    SetTextureStage(0x17B);
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    Put_2TF(&textLobbyTbl);
    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    Lb_put_new_mail(0x186, 0x1C);
    temp_v1 = (*(void **)&pNet);
    if (((s32) (*(u8 *)((u8 *)(temp_v1) + 0x22)) < 2) && ((*(u8 *)((u8 *)(temp_v1) + 0x2D)) == 0)) {
        DispFrameListA(&pfl_menu_2138, NULL, (s32) (*(u8 *)((u8 *)(temp_v1) + 0x2A)), 0xFF);
    } else {
        DispFrameListA(&pfl_menu_2138, NULL, -1, 0xFF);
    }
    if ((s32) (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x22)) >= 2) {
        font_set_palette(0xA);
        flfntLocate(0x82, 0x56);
        sprintf(&sp80, &g_lb_63E1D8, (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) + 1);
        han2zen(&sp80, &sp50);
        font_print(&sp50);
    } else {
        font_set_palette(0);
        Put_page_num(0x82, 0x56, (s32) (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)), 2, 0);
    }
    spA8 = 0x30FFFFFF;
    spA0 = 0x16;
    spA4 = 0xC6;
    var_s2 = 0;
    spA2 = 0x57;
    do {
        spA6 = spA2 + 0x12;
        if (*var_s3 == 0) {
            Put_F(&spA0);
        }
        var_s2 += 1;
        var_s3 += 4;
        spA2 += 0x16;
    } while (var_s2 < 0xB);
}

