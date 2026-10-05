#include "lobby_a.h"
extern char lit_2418[];
extern char lit_2419[];
extern char lit_2316[];
extern char lit_2316[];
void Put_page_num(int arg0, int arg1, int arg2, int arg3, int arg4) {
    char sp90[0x2C];
    int sp70;
    s32 temp_s1;
    s32 temp_s2;
    int temp_s0;
    int temp_s0_2;
    int temp_s0_3;
    int temp_s0_4;
    int temp_s0_5;
    int temp_s2_2;
    int temp_s2_3;
    int temp_s2_4;
    int temp_s2_5;

    temp_s2 = arg4;
    temp_s1 = Lb_get_cursor_col();
    font_set_palette(0);
    temp_s0 =  (arg3 << 0x30) >> 0x30;
    if (temp_s0 < 0xA) {
        sprintf(sp90, &lit_2418, ( (arg2 << 0x30) >> 0x30) + 1, F(s32, &lb_num_str, 0x2C));
    } else {
        sprintf(sp90, &lit_2419, ( (arg2 << 0x30) >> 0x30) + 1, F(s32, &lb_num_str, 0x2C));
    }
    han2zen(sp90, &sp70);
    flfntSetSize(0x12, 0x12);
    flfntLocate(arg0, arg1);
    font_set_palette(0);
    if (!(temp_s2 & 0xFF)) {
        font_print(&lit_2316, &sp70);
    } else {
        font_print(&lit_2316, sp90);
    }
    if (temp_s0 >= 2) {
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        if (temp_s2 != 0) {
            if (temp_s0 < 0xA) {
                temp_s0_2 =  (arg0 << 0x30) >> 0x30;
                temp_s2_2 = ( (arg1 << 0x30) >> 0x30) - 1;
                Lb_put_icon_free( ((temp_s0_2 - 0x18) << 0x30) >> 0x30,  (temp_s2_2 << 0x30) >> 0x30, 0x14, temp_s1);
                Lb_put_icon_free( ((temp_s0_2 + 0x28) << 0x30) >> 0x30,  (temp_s2_2 << 0x30) >> 0x30, 0x14, temp_s1);
                return;
            }
            temp_s2_3 = ( (arg1 << 0x30) >> 0x30) - 1;
            temp_s0_3 =  (arg0 << 0x30) >> 0x30;
            Lb_put_icon_free( ((temp_s0_3 - 0x18) << 0x30) >> 0x30,  (temp_s2_3 << 0x30) >> 0x30, 0x14, temp_s1);
            Lb_put_icon_free( ((temp_s0_3 + 0x3A) << 0x30) >> 0x30,  (temp_s2_3 << 0x30) >> 0x30, 0x14, temp_s1);
            return;
        }
        if (temp_s0 < 0xA) {
            temp_s0_4 =  (arg0 << 0x30) >> 0x30;
            temp_s2_4 = ( (arg1 << 0x30) >> 0x30) - 3;
            Lb_put_icon( ((temp_s0_4 - 0x1A) << 0x30) >> 0x30,  (temp_s2_4 << 0x30) >> 0x30, 0, temp_s1);
            Lb_put_icon( ((temp_s0_4 + 0x36) << 0x30) >> 0x30,  (temp_s2_4 << 0x30) >> 0x30, 1, temp_s1);
            return;
        }
        temp_s2_5 = ( (arg1 << 0x30) >> 0x30) - 3;
        temp_s0_5 =  (arg0 << 0x30) >> 0x30;
        Lb_put_icon( ((temp_s0_5 - 0x1A) << 0x30) >> 0x30,  (temp_s2_5 << 0x30) >> 0x30, 0, temp_s1);
        Lb_put_icon( ((temp_s0_5 + 0x5A) << 0x30) >> 0x30,  (temp_s2_5 << 0x30) >> 0x30, 1, temp_s1);
    }
}
