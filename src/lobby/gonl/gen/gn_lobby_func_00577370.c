/* G online (PC): lobby_func_00577370, written from Monster Hunter G's code at lobby 0x577370 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon();
s32 lobby_func_005791D0();
s32 flfntSetSize();             /* extern */
s32 font_print_double(); /* extern */
s32 font_set_palette();                  /* extern */
s32 han2zen();              /* extern */
extern s32 *g_lb_636D30;
extern u16 g_lb_6B4518;
extern s32 g_lb_6B4620;
extern s32 *tl_mail_tbl;

void lobby_func_00577370(void) {
    u8 g_frame[0x60] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
    u8 *var_s2;   /* (s32 *) */
    s32 var_s0;
    s64 var_s1;

    var_s2 = &g_lb_6B4620;
    var_s1 = 0x8E;
    lobby_func_005791D0(7);
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    if ((s32) g_lb_6B4518 >= 5) {
        g_lb_6B4518 = 4;
    }
    var_s0 = 0;
    if ((s32) g_lb_6B4518 > 0) {
        do {
            font_print_double(0xE0, var_s1, 1, 0, var_s2 + 8);
            han2zen(var_s2, &sp40);
            font_print_double(0x15E, var_s1, 1, 0, &sp40);
            if ((*(u8 *)((u8 *)(var_s2) + 0x1A)) != 0) {
                font_print_double(0x220, var_s1, 1, 0, g_lb_636D30);
            } else {
                font_print_double(0x220, var_s1, 1, 0, tl_mail_tbl);
            }
            Lb_put_icon(0x1E0, (s32) (((s16) (((((s16) (var_s1))) - 2)))), (*(u8 *)((u8 *)(var_s2) + 0x19)) + 2, -1);
            var_s2 += 0x20;
            var_s0 = (var_s0 + 1) & 0xFFFF;
            var_s1 = ((s16) ((var_s1 + 0x16)));
        } while (var_s0 < (s32) g_lb_6B4518);
    }
}

