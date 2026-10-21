/* G online (PC): lobby_func_00574750, written from Monster Hunter G's code at lobby 0x574750 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntLocate();                  /* extern */
s32 font_print();           /* extern */
s32 font_print_double(); /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();                   /* extern */
s32 lobby_func_005791D0();               /* extern */
s32 put_main_cursor2();                      /* extern */
extern s8 g_main_302290;
extern u8 lit_2316;

void lobby_func_00574750(s32 arg1, s64 arg2) {
    u8 g_frame[0x80] __attribute__((aligned(16)));   /* G's stack frame */
#define sp60 (*(s32 *)(g_frame + 0x60))
    s64 temp_v1;
    s64 var_s1;
    s64 var_s3;
    s8 *var_s2;

    put_main_cursor2((s64) (arg2 << 0x38) >> 0x38);
    lobby_func_005791D0(4);
    var_s1 = 0;
    var_s3 = (s64) ((arg1 + 0x3E) << 0x30) >> 0x30;
    var_s2 = &g_main_302290;
    if ((*(s8 *)&g_main_302290) != 0) {
loop_2:
        temp_v1 = (s64) (var_s1 << 0x30) >> 0x30;
        if (temp_v1 < 8) {
            han2zen(var_s2, &sp60);
            if (((s64) ((temp_v1 != ((s64) (arg2 << 0x38) >> 0x38)) << 0x30) >> 0x30) == 0) {
                font_print_double(0xF4, var_s3, 1, 4);
                font_print_double(0x1F4, var_s3, 1, 4);
            } else {
                font_set_palette(0);
                flfntLocate(0xF4, var_s3);
                font_print(&lit_2316, var_s2 + 8);
                flfntLocate(0x1F4, var_s3);
                font_print(&lit_2316, &sp60);
            }
            var_s2 += 0x18;
            var_s3 = (s64) ((var_s3 + 0x16) << 0x30) >> 0x30;
            var_s1 = (s64) ((var_s1 + 1) << 0x30) >> 0x30;
            if (*var_s2 == 0) {

            } else {
                goto loop_2;
            }
        }
    }
}

