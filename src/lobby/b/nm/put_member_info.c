#include "lobby_a.h"
extern char lit_2632[];
extern char lit_2633[];
extern char lit_2316[];
void put_member_info(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5) {
    int spC0;
    int sp60;
    int var_a3;
    int temp_s1;

    temp_s1 = arg4;
    if ((*(s8 *)arg2) != 0) {
        han2zen(arg2, &spC0);
        if (temp_s1 != 0) {
            sprintf(&sp60, &lit_2632, &spC0, ((int *)&lb_num_str)[((F(u8, temp_s1, 1) / 10) + ((u8) F(u8, temp_s1, 1) >> 0x1F))]);
        } else {
            sprintf(&sp60, &lit_2633, &spC0);
        }
        flfntSetSize(0x12, 0x12);
        if ((((s8)arg5) == 0) && (F(u8, pNet, 0xC) == 0)) {
            font_print_double(arg0, arg1, 1, 4);
            font_print_double( ((( (arg0 << 0x30) >> 0x30) + 0xA2) << 0x30) >> 0x30, arg1, 1, 4);
            var_a3 = 0xFF8080FF;
        } else {
            font_set_palette(0);
            flfntLocate(arg0, arg1);
            font_print(&lit_2316, arg3);
            flfntLocate( ((( (arg0 << 0x30) >> 0x30) + 0xA2) << 0x30) >> 0x30, arg1);
            font_print_uf(&sp60);
            var_a3 = -1;
        }
        if (temp_s1 != 0) {
            Lb_put_icon(0x1FE, arg1, F(u8, temp_s1, 0) + 2, var_a3);
        }
    }
}
