/* G online (PC): lobby_func_00576A70, written from Monster Hunter G's code at lobby 0x576A70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_print_double();
s32 font_set_palette();
s32 han2zen();
s32 lobby_func_00559E20();
void * lobby_func_00559EB0();
s32 lobby_func_00559FF0();
s32 lobby_func_005737C0();
s32 lobby_func_005791D0();
extern u8 g_main_2EFBC0;
extern u8 g_main_2EFBC0;
extern u8 tl_member_buff;
s32 Lb_back_money();                             /* extern */
s32 font_print();                      /* extern */
s32 func_0012BDD0();                      /* extern */
s32 sprintf();    /* extern */
extern s32 lb_ibox_str;
extern void *g_lb_636B50;
extern void *g_lb_636DF8;
extern s32 g_lb_63E268;
extern s32 g_lb_63E2B8;
extern u8 lit_2316;

void lobby_func_00576A70(void) {
    u8 g_frame[0x1B0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(s32 *)(g_frame + 0x70))
#define sp130 (*(s32 *)(g_frame + 0x130))
    s16 temp_s0_2;
    s16 temp_s1;
    s32 temp_s0;
    s32 var_s2;
    s32 var_s3;
    s32 var_s5;
    s32 temp_a2;
    s32 var_s4;
    u8 *temp_a1;
    u8 temp_a0;
    u8 *temp_v0;

    if ((*(u8 **)&g_main_2EFBC0) != NULL) {
        flfntSetSize(0x12, 0x12);
        lobby_func_005791D0(6);
        var_s4 = 0x78;
        if (*(*(u8 **)&g_main_2EFBC0) == 0) {
            lobby_func_005737C0(g_lb_636B50);
        }
        var_s3 = 0;
        var_s5 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 7;
        var_s2 = (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)) * 0x70;
loop_4:
        temp_a1 = (*(u8 **)&g_main_2EFBC0);
        temp_a0 = *temp_a1;
        var_s4 = ((s16) ((var_s4 + 0x16)));
        if (var_s5 >= (s32) temp_a0) {
            if (temp_a0 == 0x64) {
                font_print_double(0x118, (s32) var_s4, 1, 4, g_lb_636DF8);
            }
        } else {
            temp_s0 = lobby_func_00559FF0(temp_a1 + var_s2 + 4) & 0xFFFF;
            flfntLocate(0xF4, (s32) var_s4);
            font_set_palette(lobby_func_00559E20(temp_s0));
            func_0012BDD0(lobby_func_00559EB0(temp_s0));
            font_set_palette(0);
            temp_a2 = ((s16) ((Lb_back_money(temp_s0) & 0xFF)));
            if (temp_a2 != 0) {
                sprintf(&sp130, &g_lb_63E268, temp_a2);
            } else {
                sprintf(&sp130, &lit_2316, (s32) lb_ibox_str);
            }
            han2zen(&sp130, &sp70);
            flfntLocate(0x202, (s32) var_s4);
            font_print(&sp70);
            temp_v0 = (*(u8 **)&g_main_2EFBC0) + var_s2;
            temp_s1 = (*(s16 *)((u8 *)(temp_v0) + 0xE));
            temp_s0_2 = (*(s16 *)((u8 *)(temp_v0) + 0xC));
            sprintf(&sp130, &g_lb_63E2B8, (s32) temp_s0_2, temp_s1);
            han2zen(&sp130, &sp70);
            flfntLocate(0x226, (s32) var_s4);
            if (temp_s0_2 == temp_s1) {
                font_set_palette(0xA);
            } else {
                font_set_palette(0);
            }
            font_print(&sp70);
            if ((*(u8 *)((u8 *)(((*(u8 **)&g_main_2EFBC0) + var_s2)) + 0x10)) != 0) {
                Lb_put_icon(0xDC, (s32) var_s4, 0x2D, -1);
            }
            var_s3 += 1;
            var_s2 += 0x10;
            var_s5 += 1;
            if (var_s3 >= 7) {

            } else {
                goto loop_4;
            }
        }
    }
}

