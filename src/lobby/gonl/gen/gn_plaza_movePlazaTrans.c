/* G online (PC): plaza_movePlazaTrans, written from Monster Hunter G's code at lobby 0x574240 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_page_num();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_print_double();
s32 font_set_palette();
s32 lobby_func_005791D0();
s32 put_main_cursor();
extern u8 ClassInfo;
extern u8 pNet;
s32 font_print();           /* extern */
s32 sprintf();    /* extern */
extern s32 g_lb_63E248;
extern s32 g_lb_63E250;
extern u8 PlazaInfo;
extern u8 lit_2316;

void plaza_movePlazaTrans(void) {
    u8 g_frame[0xB0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp50 (*(s32 *)(g_frame + 0x50))
#define spA0 (*(s32 *)(g_frame + 0xA0))
    s32 var_s1;
    s32 var_s2;
    s32 var_s3;
    u8 *temp_v0;
    u8 *var_s0;

    put_main_cursor((s32) (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B)));
    flfntSetSize(0x12, 0x12);
    lobby_func_005791D0(1);
    var_s3 = 0x8E;
    var_s2 = 0;
    var_s1 = (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) * 7;
    var_s0 = (u8 *)&PlazaInfo + (var_s1 * 0x15C);
    do {
        sprintf(&spA0, &g_lb_63E248, (*(u16 *)((u8 *)(var_s0) + 6)), (*(u16 *)((u8 *)(var_s0) + 8)));
        if (var_s1 < (s32) (*(u16 *)((u8 *)&ClassInfo + 0x2))) {
            sprintf(&sp50, &g_lb_63E250, (u16) (var_s0 + 0x1A));
            if (var_s2 == (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B))) {
                font_print_double(0xE2, var_s3, 1, 4, &sp50);
                font_print_double(0x21E, var_s3, 1, 4, &spA0);
            } else {
                font_set_palette(0);
                flfntLocate(0xE2, var_s3);
                font_print(&lit_2316, &sp50);
                flfntLocate(0x21E, var_s3);
                font_print(&lit_2316, &spA0);
            }
        }
        var_s2 += 1;
        var_s0 += 0x15C;
        var_s1 += 1;
        var_s3 = (s32) (((s16) ((var_s3 + 0x16))));
    } while (var_s2 < 7);
    temp_v0 = (*(void **)&pNet);
    Put_page_num(0x20E, var_s3, (s32) (*(s16 *)((u8 *)(temp_v0) + 0x1C)), (s32) (*(s16 *)((u8 *)(temp_v0) + 0x1E)), 0);
}

