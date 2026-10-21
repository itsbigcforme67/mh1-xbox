/* G online (PC): Put_page_num, written from Monster Hunter G's code at lobby 0x574460 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon();
s32 Lb_put_icon_free();
s32 SetTextureStage();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_set_palette();
s32 han2zen();
s32 reload_tex();
extern u8 lb_num_str;
s32 Lb_get_cursor_col();                            /* extern */
s32 font_print();           /* extern */
s32 sprintf(); /* extern */
extern u8 lit_2316;
extern u8 lit_2418;
extern u8 lit_2419;

void Put_page_num(s32 arg0, s32 arg1, s64 arg2, s64 arg3, s32 arg4) {
    u8 g_frame[0xB0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(s32 *)(g_frame + 0x70))
#define sp90 (*(s32 *)(g_frame + 0x90))
    s32 temp_s1;
    s64 temp_s0;
    s64 temp_s0_2;
    s64 temp_s0_3;
    s64 temp_s0_4;
    s64 temp_s0_5;
    s64 temp_s2;
    s64 temp_s2_2;
    s64 temp_s2_3;
    s64 temp_s2_4;

    temp_s1 = Lb_get_cursor_col();
    font_set_palette(0);
    temp_s0 = ((s16) (arg3));
    if (temp_s0 < 0xA) {
        sprintf(&sp90, &lit_2418, (((s16) (arg2))) + 1, (*(s32 *)((u8 *)&lb_num_str + 0x2C)), temp_s0);
    } else {
        sprintf(&sp90, &lit_2419, (((s16) (arg2))) + 1, (*(s32 *)((u8 *)&lb_num_str + 0x2C)), temp_s0);
    }
    han2zen(&sp90, &sp70);
    flfntSetSize(0x12, 0x12);
    flfntLocate(arg0, arg1);
    font_set_palette(0);
    if (!(arg4 & 0xFF)) {
        font_print(&lit_2316, &sp70);
    } else {
        font_print(&lit_2316, &sp90);
    }
    if (temp_s0 >= 2) {
        reload_tex(1, 0x180);
        SetTextureStage(0x180);
        if (arg4 != 0) {
            if (temp_s0 < 0xA) {
                temp_s0_2 = ((s16) ((s64) arg0));
                temp_s2 = (((s16) ((s64) arg1))) - 1;
                Lb_put_icon_free((s32) (((s16) ((temp_s0_2 - 0x18)))), (s32) (((s16) (temp_s2))), 0x14, temp_s1, 0);
                Lb_put_icon_free((s32) (((s16) ((temp_s0_2 + 0x28)))), (s32) (((s16) (temp_s2))), 0x14, temp_s1, 1);
                return;
            }
            temp_s2_2 = (((s16) ((s64) arg1))) - 1;
            temp_s0_3 = ((s16) ((s64) arg0));
            Lb_put_icon_free((s32) (((s16) ((temp_s0_3 - 0x18)))), (s32) (((s16) (temp_s2_2))), 0x14, temp_s1, 0);
            Lb_put_icon_free((s32) (((s16) ((temp_s0_3 + 0x3A)))), (s32) (((s16) (temp_s2_2))), 0x14, temp_s1, 1);
            return;
        }
        if (temp_s0 < 0xA) {
            temp_s0_4 = ((s16) ((s64) arg0));
            temp_s2_3 = (((s16) ((s64) arg1))) - 3;
            Lb_put_icon((s32) (((s16) ((temp_s0_4 - 0x1A)))), (s32) (((s16) (temp_s2_3))), 0, temp_s1);
            Lb_put_icon((s32) (((s16) ((temp_s0_4 + 0x36)))), (s32) (((s16) (temp_s2_3))), 1, temp_s1);
            return;
        }
        temp_s2_4 = (((s16) ((s64) arg1))) - 3;
        temp_s0_5 = ((s16) ((s64) arg0));
        Lb_put_icon((s32) (((s16) ((temp_s0_5 - 0x1A)))), (s32) (((s16) (temp_s2_4))), 0, temp_s1);
        Lb_put_icon((s32) (((s16) ((temp_s0_5 + 0x5A)))), (s32) (((s16) (temp_s2_4))), 1, temp_s1);
    }
}

