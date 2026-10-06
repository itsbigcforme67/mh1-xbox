#include "lobby_a.h"
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char tl_mail_tbl[];
extern char tl_mail_tbl[];
extern char tl_mail_tbl[];
void plaza_disp_mail(int arg0, int arg1, int arg2) {
    s8 spB4;
    s8 spB3;
    char sp90[0x23];
    s32 temp_s4;
    s32 var_s5;
    s32 var_s7;
    int temp_s1;
    int temp_s2;
    int var_s0;

    font_set_palette(0);
    flfntLocate();
    font_print(&lit_2316, F(int, &tl_mail_tbl, 8));
    temp_s2 =  (arg1 << 0x30) >> 0x30;
    temp_s1 = temp_s2 + 0x3C;
    flfntLocate((temp_s1 << 0x30) >> 0x30);
    flfntSetSize(0x16, 0x12);
    font_print(&lit_2316, (s32)cw + 0x2F88);
    flfntSetSize(0x12, 0x12);
    var_s0 =  ((arg2 + 0x16) << 0x30) >> 0x30;
    flfntLocate();
    font_print(&lit_2316, F(int, &tl_mail_tbl, 0xC));
    flfntLocate((temp_s1 << 0x30) >> 0x30);
    han2zen((s32)cw + 0x2F80, sp90);
    font_print(&lit_2316, sp90);
    flfntLocate(arg1,  ((( (var_s0 << 0x30) >> 0x30) + 0x16) << 0x30) >> 0x30);
    font_print(&lit_2316, F(int, &tl_mail_tbl, 0x10));
    font_set_palette(0);
    var_s5 = (s32)cw + 0x2F99;
    KinshiYogo_chk();
    var_s7 = 0;
loop_1:
    if (var_s5 != 0) {
        temp_s4 = strlen();
        strcpy(sp90);
        if (temp_s4 >= 0x24) {
            if (Ck_hankaku(sp90, 0x23) == 0) {
                spB4 = 0;
                var_s5 += 0x24;
            } else {
                spB3 = 0;
                var_s5 += 0x23;
            }
            var_s0 =  ((var_s0 + 0x16) << 0x30) >> 0x30;
            flfntLocate( ((temp_s2 + 0x3C) << 0x30) >> 0x30, var_s0);
            font_print(&lit_2316, sp90);
            var_s7 += 1;
            if (var_s7 < 4) {
                goto loop_1;
            }
        } else {
            flfntLocate( (temp_s1 << 0x30) >> 0x30,  ((( (var_s0 << 0x30) >> 0x30) + 0x16) << 0x30) >> 0x30);
            font_print(&lit_2316, sp90);
        }
    }
    font_set_palette(0);
}
