#include "lobby_a.h"
extern u8 chatListFlag;
extern char text_lobby_msg[];
extern char lit_2602[];
extern char lit_2316[];
extern char chatLogBuff[];
extern char lit_2316[];
extern char lit_2316[];
extern char chatIDList[];
extern char chatIDList[];
extern char chatHandleList[];
extern char chatHandleList[];
extern char chatIDList[];
extern char lit_2316[];
extern char lit_2316[];
void plaza_setChatModeTrans(void) {
    char spD0[0x10];
    int spA0;
    int var_s6;
    int var_s7_2;
    s16 temp_s3;
    int var_s7;
    s32 temp_s1_2;
    s32 temp_s4;
    int temp_fp;
    int temp_s1;
    int temp_s4_2;
    int temp_s6;
    int temp_v1;
    int var_s0_2;
    int var_s1;
    int var_s2;
    int var_s5;
    int var_fp;
    int var_s0;
    int var_s5_2;
    int temp_a0;
    int temp_s0;
    int temp_s0_2;
    int temp_v0;

    temp_s0 = F(int, &text_lobby_msg, 8);
    if (F(u8, pNet, 3) != 0) {
        font_set_palette(0);
        flfntSetSize(0x12, 0x12);
        put_main_cursor(F(u8, pNet, 0xA) - 1);
        Lb_put_msg_type2(temp_s0 + 0x150);
        flfntLocate( ((s16)((F(s16, temp_s0, 0x150) + 0x90))), F(s16, temp_s0, 0x152));
        sprintf(spD0, &lit_2602, 7 - F(u8, (u8 *)cw, 0x32BE));
        han2zen(spD0, &spA0);
        font_print(&lit_2316, &spA0);
        temp_s0_2 = temp_s0 + 0x158;
        if (F(u8, pNet, 0xA) == 0) {
            font_print_double(F(s16, temp_s0, 0x158), F(s16, temp_s0_2, 2), 1, 4);
        } else {
            Lb_put_msg_type2();
        }
        if (F(u8, (u8 *)cw, 0x32BE) == 0) {
            Put_megaphone( ((s16)((F(s16, temp_s0, 0x158) - 0x1A))),  ((s16)((F(s16, temp_s0_2, 2) - 4))), 0);
        }
        temp_a0 = (int)pNet;
        temp_s3 = F(s16, temp_s0, 0x158);
        var_s2 =  ((s16)((F(s16, temp_s0_2, 2) + 0x16)));
        if (F(u8, temp_a0, 3) < 3) {
            temp_v1 = (s16)temp_s3;
            temp_s6 = temp_v1 + 0xA4;
            temp_fp = temp_v1 - 0x1A;
            var_s5 = 0;
            var_s7 = (int)&chatLogBuff + (( (F(s16, temp_a0, 0x24) * 0x7000000000000) >> 0x30) * 4);
loop_8:
            temp_s4 = (*(s32 *)var_s7);
            if (temp_s4 != 0) {
                if (F(u8, pNet, 0xA) == (( ((s16)(var_s5))) + 1)) {
                    sprintf(spD0, temp_s4 + 0x4C, 8);
                    font_print_double(temp_s3, (s16) var_s2, 1, 4);
                    strcpy(spD0, temp_s4 + 0x44);
                    han2zen(spD0, &spA0);
                    font_print_double((s16) ( ((s16)(temp_s6))), (s16) var_s2, 1, 4);
                } else {
                    font_set_palette(0);
                    flfntLocate( temp_s3, (s16) var_s2);
                    font_print(&lit_2316, temp_s4 + 0x4C);
                    strcpy(spD0, temp_s4 + 0x44);
                    han2zen(spD0, &spA0);
                    flfntLocate( ((s16)(temp_s6)), (s16) var_s2);
                    font_print(&lit_2316, &spA0);
                }
                var_s1 = 0;
                var_s0 = (int)&chatIDList;
loop_13:
                if (memcmp(temp_s4 + 0x44, var_s0, 8) != 0) {
                    var_s1 =  ((s16)((var_s1 + 1)));
                    var_s0 += 8;
                    if (var_s1 < 7) {
                        goto loop_13;
                    }
                }
                if (( ((s16)(var_s1))) != 7) {
                    Put_megaphone( ((s16)(temp_fp)),  ((s16)((( ((s16)(var_s2))) - 4))), 0);
                }
                var_s5 =  ((s16)((var_s5 + 1)));
                var_s7 += 4;
                var_s2 =  ((s16)((var_s2 + 0x16)));
                if (var_s5 >= 7) {

                } else {
                    goto loop_8;
                }
            }
            temp_v0 = (int)pNet;
            Put_page_num(0x1F4, 0x128, F(s16, temp_v0, 0x24), F(s16, temp_v0, 0x26));
            return;
        }
        var_s5_2 = (int)&chatIDList;
        var_s6 = (int)&chatHandleList;
        var_s0_2 = 0;
        var_s7_2 = (int)&chatHandleList;
        var_fp = (int)&chatIDList;
        temp_s4_2 = ((s16)temp_s3) + 0xA4;
        do {
            if ((*(s8 *)var_s5_2) != 0) {
                temp_s1 =  ((s16)(var_s0_2));
                if (F(u8, pNet, 0xA) == (temp_s1 + 1)) {
                    sprintf(spD0, var_s6, 0x10);
                    temp_s1_2 = 1 << temp_s1;
                    if (chatListFlag & temp_s1_2) {
                        font_print_double(temp_s3, (s16) var_s2, 1, 4);
                    } else {
                        font_print_double(temp_s3, (s16) var_s2, 1, 0xA);
                    }
                    memcpy(spD0, var_s5_2, 8);
                    han2zen(spD0, &spA0);
                    if (chatListFlag & temp_s1_2) {
                        font_print_double((s16) ( ((s16)(temp_s4_2))), (s16) var_s2, 1, 4);
                    } else {
                        font_print_double((s16) ( ((s16)(temp_s4_2))), (s16) var_s2, 1, 0xA);
                    }
                } else {
                    if (chatListFlag & (1 << temp_s1)) {
                        font_set_palette(0);
                    } else {
                        font_set_palette(0xA);
                    }
                    flfntLocate( temp_s3, (s16) var_s2);
                    font_print(&lit_2316);
                    strcpy(spD0);
                    han2zen(spD0, &spA0);
                    flfntLocate( ((s16)(temp_s4_2)), (s16) var_s2);
                    font_print(&lit_2316, &spA0);
                }
            }
            var_s5_2 += 8;
            var_s2 =  ((s16)((var_s2 + 0x16)));
            var_s6 += 0x10;
            var_s0_2 =  ((s16)((var_s0_2 + 1)));
            var_s7_2 += 0x10;
            var_fp += 8;
        } while (var_s0_2 < 7);
    }
}
