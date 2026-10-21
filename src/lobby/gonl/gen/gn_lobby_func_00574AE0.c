/* G online (PC): lobby_func_00574AE0, written from Monster Hunter G's code at lobby 0x574AE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon();
s32 flfntLocate();
s32 font_print_double();
s32 font_set_palette();
s32 han2zen();
s32 lobby_func_005791D0();
s32 lobby_func_005799F0();
s32 lobby_func_005C1560();
extern u8 BsProxyUrlstr;
s32 font_print();                /* extern */
extern u8 lit_2316;

void lobby_func_00574AE0(s32 arg0, s32 arg1, s32 arg2) {
    u8 g_frame[0xA0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp80 (*(s8 *)(g_frame + 0x80))
    s32 temp_s0;
    s32 temp_s1;
    s32 var_s3;
    s32 var_s5;
    s8 *var_s4;

    lobby_func_005799F0(arg0, arg1, (s32) (((s8) (arg2))), -0xC5D7E8, -0x418C);
    lobby_func_005791D0(4);
    temp_s1 = (((s16) (arg0))) + 0xA;
    var_s3 = 0;
    var_s4 = &(*(s8 *)((u8 *)&BsProxyUrlstr + 0x110));
    var_s5 = ((s16) ((arg1 + 0x3E)));
    temp_s0 = (((s16) (temp_s1))) + 0x14;
    do {
        if (*var_s4 != 0) {
            han2zen(var_s4, &sp80);
            if ((((s16) (((((s16) (var_s3))) != (((s8) (arg2))))))) == 0) {
                font_print_double((s32) (((s16) (temp_s0))), (s32) var_s5, 1, 4, var_s4 + 8);
                font_print_double(0x1F4, (s32) var_s5, 1, 4, &sp80);
            } else {
                font_set_palette(0);
                flfntLocate((s32) (((s16) (temp_s0))), (s32) var_s5);
                font_print(&lit_2316, var_s4 + 8);
                flfntLocate(0x1F4, (s32) var_s5);
                font_print(&lit_2316, &sp80);
            }
            if ((((s16) (lobby_func_005C1560(&(*(s8 *)((u8 *)&BsProxyUrlstr + 0x110)), 8, var_s4)))) != -1) {
                Lb_put_icon((s32) (((s16) (temp_s1))), (s32) var_s5, 0x2C, -1);
            }
        }
        var_s4 += 0x18;
        var_s3 = ((s16) ((var_s3 + 1)));
        var_s5 = ((s16) ((var_s5 + 0x16)));
    } while (var_s3 < 8);
}

