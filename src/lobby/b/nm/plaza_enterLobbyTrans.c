#include "lobby_a.h"
extern char tl_member_buff[];
extern char tl_msg_tbl[];
extern char LobbyInfo[];
extern char ClassInfo[];
extern char lit_193_0065DBE8[];
extern char lit_2315[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2317[];
extern char lit_2316[];
extern char tl_msg_tbl[];
void plaza_enterLobbyTrans(int arg0, int arg1) {
    int sp100;
    int spC0;
    s32 spB0;
    s32 spA0;
    int var_s2;
    s16 temp_s5;
    s32 var_s3;
    s32 var_s7;
    int temp_s0;
    int temp_s0_2;
    int temp_s0_3;
    int temp_s6;
    int temp_v0;
    int temp_v1_2;
    int var_s2_2;
    int var_s4;
    int var_s4_2;
    u8 temp_v1;
    int temp_a0;
    int temp_a3;
    int temp_v1_3;
    int var_s1;

    var_s3 = 0;
    var_s2 = (int)&tl_member_buff;
    put_mainWindow();
    flfntSetSize(0x12, 0x12);
    temp_a0 = (int)pNet;
    temp_v1 = F(u8, temp_a0, 3);
    if ((temp_v1 != 2) && (temp_v1 != 3)) {
        put_main_cursor2(arg0, arg1, F(u8, temp_a0, 0xA) % 7);
        temp_v0 =  (arg0 << 0x30) >> 0x30;
        spA0 = temp_v0;
        put_titles( ((temp_v0 + 0xA) << 0x30) >> 0x30,  ((( (arg1 << 0x30) >> 0x30) + 0x28) << 0x30) >> 0x30, F(s32, &tl_msg_tbl, 0));
        temp_a3 = (int)pNet;
        var_s4 =  ((arg1 + 0x3E) << 0x30) >> 0x30;
        temp_s0 = temp_v0 + 0xA;
        temp_v1_2 =  (temp_s0 << 0x30) >> 0x30;
        spB0 = temp_v1_2 + 0x135;
        temp_s6 = temp_v1_2 + 0xE8;
        var_s7 = ((F(u8, temp_a3, 0xA) / 7) + ((u8) F(u8, temp_a3, 0xA) >> 0x1F)) * 7;
        var_s1 = (int)&LobbyInfo + (var_s7 * 0x15C);
        do {
            if (var_s7 < F(u16, &ClassInfo, 6)) {
                temp_s5 = F(s16, var_s1, 0xE);
                var_s2_2 =  ((F(s16, var_s1, 2) - temp_s5) << 0x30) >> 0x30;
                if (var_s2_2 < 0) {
                    var_s2_2 = 0;
                }
                sprintf(&spC0, &lit_193_0065DBE8, Get_ServerName(), var_s1 + 0x14);
                sprintf(&sp100, &lit_2315, ((int *)&lb_num_str)[( (var_s2_2 << 0x30) >> 0x30)], ((int *)&lb_num_str)[((s16)temp_s5)]);
                Lb_put_icon( ((temp_v1_2 + 0xCA) << 0x30) >> 0x30, var_s4, 7, -1);
                Lb_put_icon( (s16) spB0, var_s4, 8, -1);
                if (var_s3 == (F(u8, pNet, 0xA) % 7)) {
                    font_print_double( (temp_s0 << 0x30) >> 0x30, var_s4, 1, 4);
                    font_print_double( (temp_s6 << 0x30) >> 0x30, var_s4, 1, 4);
                } else {
                    font_set_palette(0);
                    flfntLocate( (temp_s0 << 0x30) >> 0x30, var_s4);
                    font_print(&lit_2316, &spC0);
                    flfntLocate( (temp_s6 << 0x30) >> 0x30, var_s4);
                    font_print(&lit_2316, &sp100);
                }
            } else {
                sprintf(&sp100, &lit_2317);
                if (var_s3 == (F(u8, pNet, 0xA) % 7)) {
                    font_print_double( (temp_s0 << 0x30) >> 0x30, var_s4, 1, 4);
                } else {
                    font_set_palette(0);
                    flfntLocate( (temp_s0 << 0x30) >> 0x30, var_s4);
                    font_print(&lit_2316, &sp100);
                }
            }
            var_s3 += 1;
            var_s4 =  ((var_s4 + 0x16) << 0x30) >> 0x30;
            var_s1 += 0x15C;
            var_s7 += 1;
        } while (var_s3 < 7);
        temp_s0_2 = spA0 + 0x13C;
        flfntLocate( (temp_s0_2 << 0x30) >> 0x30, var_s4);
        temp_v1_3 = (int)pNet;
        Put_page_num( (temp_s0_2 << 0x30) >> 0x30, var_s4,  (((F(u8, temp_v1_3, 0xA) / 7) + ((u8) F(u8, temp_v1_3, 0xA) >> 0x1F)) << 0x30) >> 0x30, 2);
    } else {
        temp_s0_3 = ( (arg0 << 0x30) >> 0x30) + 0xA;
        put_titles( (temp_s0_3 << 0x30) >> 0x30,  ((( (arg1 << 0x30) >> 0x30) + 0x28) << 0x30) >> 0x30, F(s32, &tl_msg_tbl, 4));
        var_s4_2 =  ((arg1 + 0x3E) << 0x30) >> 0x30;
        if (F(u8, pNet, 6) == 0) {
            flfntSetSize(0x16, 0x12);
            font_set_palette(4);
            font_print_double( ((( (temp_s0_3 << 0x30) >> 0x30) + 0x46) << 0x30) >> 0x30,  ((( (var_s4_2 << 0x30) >> 0x30) + 0x3C) << 0x30) >> 0x30, 1, 4);
        } else {
loop_20:
            if (var_s3 < 8) {
                put_member_info( (temp_s0_3 << 0x30) >> 0x30, var_s4_2, var_s2 + 0x280, var_s2 + 0x288);
                var_s3 += 1;
                var_s2 += 0x2FC;
                var_s4_2 =  ((var_s4_2 + 0x16) << 0x30) >> 0x30;
                goto loop_20;
            }
        }
    }
}
