/* G online (PC): lobby_func_005775C0, written from Monster Hunter G's code at lobby 0x5775C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_page_num();
s32 lobby_func_005737C0();
s32 lobby_func_005791D0();
s32 put_main_cursor2();
extern u8 cw;
extern u8 tl_hr_tbl;
extern u8 tl_member_buff;
s32 flfntLocate();                      /* extern */
s32 font_print(); /* extern */
s32 font_print_double(); /* extern */
s32 font_set_palette();       /* extern */
s32 han2zen();                 /* extern */
s32 sprintf(); /* extern */
extern s32 g_lb_636B4C;
extern s32 g_lb_63E2A8;
extern s16 g_lb_6B42B8;
extern s32 g_main_302CB0;
extern u8 lit_2316;

void lobby_func_005775C0(s64 arg0, s64 arg1, s64 arg2, s64 arg3) {
    u8 g_frame[0x140] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(s32 *)(g_frame + 0xA0))
#define spB0 (*(s32 *)(g_frame + 0xB0))
#define spC0 (*(s32 *)(g_frame + 0xC0))
#define spE0 (*(s32 *)(g_frame + 0xE0))
    u8 *temp_a1;   /* (s32 *) */
    s32 var_s1;
    s32 var_s2;
    s64 temp_a0;
    s64 temp_s0;
    s64 temp_s2;
    s64 temp_s6;
    s64 temp_v0;
    s64 var_s4;
    s64 var_s5;
    u8 *var_s3;

    if (g_lb_6B42B8 == 0) {
        lobby_func_005737C0((s64) g_lb_636B4C);
        return;
    }
    put_main_cursor2(arg0, arg1, (s32) (((s8) (arg2))));
    lobby_func_005791D0(3);
    spB0 = (s32) (((s8) (arg2)));
    temp_a0 = ((s16) (arg3));
    temp_v0 = ((s16) (arg0));
    var_s5 = 0;
    spA0 = (s32) temp_v0;
    var_s4 = ((s16) ((arg1 + 0x3E)));
    var_s1 = temp_a0 * 7;
    temp_s0 = temp_v0 + 0xA;
    var_s3 = (u8 *)&g_main_302CB0 + (temp_a0 * 0x150);
    do {
        temp_s2 = ((s16) (var_s5));
        temp_s6 = temp_s2 != spB0;
        if ((var_s1 < 0x32) && ((*(s32 *)((u8 *)&g_main_302CB0 + (var_s1 * 0x30))) != 0)) {
            han2zen(var_s3, &spC0);
            flfntLocate(((s16) (temp_s0)), var_s4);
            temp_a1 = (*(u8 **)&cw);
            if ((*(u8 *)((u8 *)((temp_a1 + (var_s1 * 0x24A))) + 0x1265)) != 2) {
                if ((*(u8 *)((u8 *)((temp_a1 + ((temp_s2 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 7)) * 0x24A))) + 0x1265)) == 0) {
                    var_s2 = (*(s32 *)((u8 *)&tl_hr_tbl + 0x38));
                } else {
                    var_s2 = (*(s32 *)((u8 *)&tl_hr_tbl + 0x3C));
                }
                if ((((s16) (temp_s6))) == 0) {
                    sprintf(&spE0, &g_lb_63E2A8, var_s3 + 8, &spC0, var_s2);
                    font_print_double(((s16) (temp_s0)), var_s4, 1, 4, &spE0);
                } else {
                    font_set_palette(0, &g_lb_63E2A8);
                    font_print(&g_lb_63E2A8, var_s3 + 8, &spC0, var_s2);
                }
            } else {
                font_set_palette(0xA, temp_a1);
                sprintf(&spE0, &g_lb_63E2A8, var_s3 + 8, &spC0, (*(s32 *)((u8 *)&tl_hr_tbl + 0x40)));
                if ((((s16) (temp_s6))) == 0) {
                    font_print_double(((s16) (temp_s0)), var_s4, 1, 0xA, &spE0);
                } else {
                    font_print(&lit_2316, &spE0);
                }
            }
        }
        var_s1 += 1;
        var_s4 = ((s16) ((var_s4 + 0x16)));
        var_s5 = ((s16) ((var_s5 + 1)));
        var_s3 += 0x30;
    } while (var_s5 < 7);
    Put_page_num(((s16) (((((s16) ((spA0 + 0xA)))) + 0x12C))), var_s4, arg3, (s64) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x606)), 0);
}

