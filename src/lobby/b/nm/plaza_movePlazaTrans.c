#include "lobby_a.h"
extern char text_lobby_msg[];
extern char PlazaInfo[];
extern char lit_2354_0065DDD0[];
extern char ClassInfo[];
extern char lit_2355[];
extern char lit_2316[];
extern char lit_2316[];
extern char lit_2317[];
typedef struct { u8 pad0000[0xA]; u8 x000A; u8 pad000B[0x19]; s16 x0024; } ARG_plaza_movePlazaTrans_arg0;
void plaza_movePlazaTrans(ARG_plaza_movePlazaTrans_arg0 *arg0) {
    int spE0;
    int spD0;
    int sp80;
    s16 temp_s5;
    s32 var_s2;
    s32 var_s3;
    int temp_s0_2;
    int var_s4;
    int temp_s0;
    int var_s1;

    temp_s0 = F(int, &text_lobby_msg, 8);
    put_main_cursor(arg0->x000A % 7);
    flfntSetSize(0x12, 0x12);
    put_titles2(temp_s0 + 0x100);
    temp_s5 = F(s16, temp_s0, 0x100);
    var_s3 = 0;
    temp_s0_2 = temp_s5 + 0xFC;
    var_s4 =  ((F(s16, temp_s0, 0x102) + 0x16) << 0x30) >> 0x30;
    var_s2 = ((arg0->x000A / 7) + ((u8) arg0->x000A >> 0x1F)) * 7;
    var_s1 = (int)&PlazaInfo + (var_s2 * 0x15C);
    do {
        sprintf(&spE0, &lit_2354_0065DDD0, F(u16, var_s1, 2));
        han2zen(&spE0, &spD0);
        if (var_s2 < F(u16, &ClassInfo, 2)) {
            sprintf(&sp80, &lit_2355, Get_ServerName(), var_s1 + 0x14);
            if (var_s3 == (arg0->x000A % 7)) {
                font_print_double(temp_s5, var_s4, 1, 4);
                font_print_double((s16) ( (temp_s0_2 << 0x30) >> 0x30), var_s4, 1, 4);
            } else {
                font_set_palette(0);
                flfntLocate(temp_s5, var_s4);
                font_print(&lit_2316, &sp80);
                flfntLocate((s16) ( (temp_s0_2 << 0x30) >> 0x30), var_s4);
                font_print(&lit_2316, &spD0);
            }
        } else if (var_s2 < 0xA) {
            font_set_palette(0);
            flfntLocate(temp_s5, var_s4);
            font_print(&lit_2317);
        }
        var_s3 += 1;
        var_s1 += 0x15C;
        var_s2 += 1;
        var_s4 =  ((var_s4 + 0x16) << 0x30) >> 0x30;
    } while (var_s3 < 7);
    Put_page_num( ((temp_s5 + 0x12C) << 0x30) >> 0x30, var_s4, arg0->x0024, 2);
}
