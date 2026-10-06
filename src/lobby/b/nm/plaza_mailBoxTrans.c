#include "lobby_a.h"
extern char RecvMailInfo[];
extern char RecvMailInfo[];
extern char tl_msg_tbl[];
extern char RecvMailInfo[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2316[];
extern char tl_msg_tbl[];
extern char tl_mail_tbl[];
extern char tl_mail_tbl[];
extern char tl_mail_tbl[];
extern char lit_2316[];
void plaza_mailBoxTrans(int arg0, int arg1, int arg2) {
    int spA0;
    int var_a3;
    int var_s1;
    int var_s2;
    int var_s3;
    int temp_fp;
    int temp_s0;
    int temp_s0_2;
    int temp_s0_3;
    int temp_s1;
    int temp_s1_2;
    int temp_s1_3;
    int temp_s1_4;
    int temp_s2;
    int temp_s2_2;
    int temp_s5;
    int temp_s6;
    int temp_s7;
    int temp_v1;
    int temp_v1_3;
    int var_s0;
    int var_s4;
    int var_t0;
    u8 temp_v1_2;
    int temp_a0;
    int temp_a1;

    var_t0 = 0;
    var_s3 = (int)&RecvMailInfo;
    var_a3 = (int)&RecvMailInfo;
    F(s16, pNet, 0x26) = 0;
loop_1:
    if (F(s8, var_a3, 1) != 0) {
        temp_a1 = (int)pNet;
        var_a3 += 0x9A;
        var_t0 =  ((var_t0 + 1) << 0x30) >> 0x30;
        F(s16, temp_a1, 0x26) = (s16) (F(s16, temp_a1, 0x26) + 1);
        if (var_t0 >= 8) {

        } else {
            goto loop_1;
        }
    }
    if (F(u8, pNet, 3) != 0) {
        temp_v1 = (s8)arg2;
        switch (temp_v1) {                          /* irregular */
        case 0:
            put_mainWindow(arg0, arg1);
            break;
        case 1:
            put_mainWindowTex(arg0, arg1);
            break;
        }
        font_set_palette(0);
        flfntSetSize(0x12, 0x12);
        temp_a0 = (int)pNet;
        temp_v1_2 = F(u8, temp_a0, 3);
        if (temp_v1_2 < 2) {
            if (F(s16, temp_a0, 0x26) != 0) {
                put_main_cursor2(arg0, arg1, F(u8, temp_a0, 0xA));
                temp_s1 =  (arg0 << 0x30) >> 0x30;
                put_titles( ((temp_s1 + 0xA) << 0x30) >> 0x30,  ((( (arg1 << 0x30) >> 0x30) + 0x28) << 0x30) >> 0x30, F(s32, &tl_msg_tbl, 0xC));
                var_s0 = 0;
                var_s4 =  ((arg1 + 0x3E) << 0x30) >> 0x30;
                if (F(s16, pNet, 0x26) > 0) {
                    temp_v1_3 =  ((temp_s1 + 0xA) << 0x30) >> 0x30;
                    var_s2 = (int)&lb_num_str;
                    var_s1 = (int)&RecvMailInfo;
                    temp_s5 = temp_v1_3 + 9;
                    temp_s6 = temp_v1_3 + 0x78;
                    temp_s7 = temp_v1_3 + 0x104;
                    temp_fp = temp_v1_3 + 0x40;
loop_15:
                    if (F(s8, var_s3, 1) != 0) {
                        if (F(u8, pNet, 0xA) == ( (var_s0 << 0x30) >> 0x30)) {
                            font_print_double( (temp_s5 << 0x30) >> 0x30, var_s4, 1, 4);
                            flfntSetSize(0x16, 0x12);
                            font_print_double( (temp_s6 << 0x30) >> 0x30, var_s4, 1, 4);
                            flfntSetSize(0x12, 0x12);
                            han2zen(var_s1 + 1, &spA0);
                            font_print_double( (temp_s7 << 0x30) >> 0x30, var_s4, 1, 4);
                        } else {
                            font_set_palette(0);
                            flfntLocate((temp_s5 << 0x30) >> 0x30);
                            font_print(&lit_2316, F(int, var_s2, 4));
                            flfntSetSize(0x16, 0x12);
                            flfntLocate( (temp_s6 << 0x30) >> 0x30, var_s4);
                            font_print(&lit_2316, var_s1 + 9);
                            flfntSetSize(0x12, 0x12);
                            flfntLocate( (temp_s7 << 0x30) >> 0x30, var_s4);
                            han2zen(var_s1 + 1, &spA0);
                            font_print(&lit_2316, &spA0);
                        }
                        if (F(u8, var_s3, 0) == 0) {
                            Lb_put_icon( (temp_fp << 0x30) >> 0x30, var_s4, 0xA, -1);
                        } else {
                            Lb_put_icon( (temp_fp << 0x30) >> 0x30, var_s4, 9, -1);
                        }
                        var_s3 += 0x9A;
                        var_s2 += 4;
                        var_s4 =  ((var_s4 + 0x16) << 0x30) >> 0x30;
                        var_s0 =  ((var_s0 + 1) << 0x30) >> 0x30;
                        var_s1 += 0x9A;
                        if (var_s0 >= F(s16, pNet, 0x26)) {
                            return;
                        }
                        goto loop_15;
                    }
                }
            } else {
                temp_s1_2 = ( (arg1 << 0x30) >> 0x30) + 0x28;
                temp_s0 = ( (arg0 << 0x30) >> 0x30) + 0xA;
                put_titles( (temp_s0 << 0x30) >> 0x30,  (temp_s1_2 << 0x30) >> 0x30, F(s32, &tl_msg_tbl, 0xC));
                flfntSetSize(0x14, 0x14);
                font_print_double( ((( (temp_s0 << 0x30) >> 0x30) + 0x46) << 0x30) >> 0x30,  ((( (temp_s1_2 << 0x30) >> 0x30) + 0x3C) << 0x30) >> 0x30, 1, 4);
            }
        } else {
            if (temp_v1_2 == 2) {
                temp_s1_3 = ( (arg1 << 0x30) >> 0x30) + 0x28;
                temp_s0_2 = ( (arg0 << 0x30) >> 0x30) + 0xA;
                put_titles( (temp_s0_2 << 0x30) >> 0x30,  (temp_s1_3 << 0x30) >> 0x30, F(s32, &tl_mail_tbl, 0));
                temp_s2 =  (temp_s0_2 << 0x30) >> 0x30;
                put_titles( ((temp_s2 + 0x12C) << 0x30) >> 0x30,  (temp_s1_3 << 0x30) >> 0x30, F(s32, &tl_mail_tbl, 4));
                put_titles( ((temp_s2 + 0x168) << 0x30) >> 0x30,  (temp_s1_3 << 0x30) >> 0x30, (*(int *)((u8 *)&lb_num_str + 4 + (F(u8, pNet, 0xE) * 4))));
                plaza_disp_mail(pNet, temp_s2,  ((( (temp_s1_3 << 0x30) >> 0x30) + 0x16) << 0x30) >> 0x30);
                return;
            }
            temp_s2_2 = ( (arg1 << 0x30) >> 0x30) + 0x28;
            temp_s1_4 = ( (arg0 << 0x30) >> 0x30) + 0xA;
            put_titles( (temp_s1_4 << 0x30) >> 0x30,  (temp_s2_2 << 0x30) >> 0x30, F(s32, &tl_mail_tbl, 0x14));
            temp_s0_3 =  (temp_s2_2 << 0x30) >> 0x30;
            plaza_disp_mail(pNet,  (temp_s1_4 << 0x30) >> 0x30,  ((temp_s0_3 + 0x16) << 0x30) >> 0x30);
            put_mail_input_square(pNet,  (temp_s1_4 << 0x30) >> 0x30);
            if (F(s8, (u8 *)cw, 0x2F99) != 0) {
                font_set_palette(0);
            } else {
                font_set_palette(0xA);
            }
            flfntLocate( (temp_s1_4 << 0x30) >> 0x30,  ((temp_s0_3 + 0xB0) << 0x30) >> 0x30);
            font_print(&lit_2316, F(int, &tl_mail_tbl, 0x18));
        }
    }
}
