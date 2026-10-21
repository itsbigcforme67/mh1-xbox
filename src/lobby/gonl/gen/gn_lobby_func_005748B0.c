/* G online (PC): lobby_func_005748B0, written from Monster Hunter G's code at lobby 0x5748B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 BsProxyUrlstr;
extern u8 tl_member_buff;
s32 Lb_put_icon();    /* extern */
s32 Put_page_num();  /* extern */
s32 flfntLocate();                      /* extern */
s32 font_print();                /* extern */
s32 font_print_double(); /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                        /* extern */
s32 lobby_func_005791D0();               /* extern */
s64 lobby_func_005C1560();  /* extern */
s32 put_main_cursor2();                      /* extern */
extern u8 lit_2316;

void lobby_func_005748B0(s64 arg0, s32 arg1, s64 arg2, s64 arg3) {
    u8 g_frame[0xD0] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(u8 * *)(g_frame + 0xA0))
#define spB0 (*(s8 *)(g_frame + 0xB0))
    s64 temp_fp;
    s64 temp_s0;
    s64 temp_s1;
    s64 var_s3;
    s64 var_s5;
    s8 *var_s4;

    spA0 = &(*(u8 *)((u8 *)&tl_member_buff + 0x1398));
    put_main_cursor2(((s8) (arg2)));
    lobby_func_005791D0(4);
    temp_fp = ((s16) (arg0));
    temp_s1 = temp_fp + 0xA;
    temp_s0 = (((s16) (temp_s1))) + 0x14;
    var_s3 = 0;
    var_s5 = ((s16) ((arg1 + 0x3E)));
    var_s4 = (u8 *)&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)) + ((((s16) (arg3))) * 0xA8);
    do {
        if (*var_s4 != 0) {
            han2zen(var_s4, &spB0);
            if ((((s16) (((((s16) (var_s3))) != (((s8) (arg2))))))) == 0) {
                font_print_double(((s16) (temp_s0)), var_s5, 1, 4, var_s4 + 8);
                font_print_double(0x1F4, var_s5, 1, 4, &spB0);
            } else {
                font_set_palette(0);
                flfntLocate(((s16) (temp_s0)), var_s5);
                font_print(&lit_2316, var_s4 + 8);
                flfntLocate(0x1F4, var_s5);
                font_print(&lit_2316, &spB0);
            }
            if ((((s16) (lobby_func_005C1560(&(*(u8 *)((u8 *)&BsProxyUrlstr + 0x110)), 8, var_s4)))) != -1) {
                Lb_put_icon(((s16) (temp_s1)), var_s5, 0x2C, -1);
            }
        }
        var_s4 += 0x18;
        var_s3 = ((s16) ((var_s3 + 1)));
        var_s5 = ((s16) ((var_s5 + 0x16)));
    } while (var_s3 < 7);
    Put_page_num(((s16) (((((s16) ((temp_fp + 0xA)))) + 0x12C))), var_s5, arg3, (*(s16 *)((u8 *)(spA0) + 0x606)), 0);
}

