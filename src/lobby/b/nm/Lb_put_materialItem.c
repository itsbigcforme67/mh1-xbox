#include "lobby_a.h"
extern char lit_1287_006555F0[];
extern char item_str[];
void Lb_put_materialItem(s32 arg0, int arg1, int arg2) {
    s32 temp_s0;
    int temp_s0_2;
    int temp_s1;
    int temp_s2;
    int temp_s2_2;
    int var_s3;

    temp_s0 = arg1 & 0xFFFF;
    temp_s2 = (s16)Ud_item_num_ck(temp_s0);
    temp_s0_2 =  (arg1 << 0x30) >> 0x30;
    var_s3 = (s16)Ud_stock_item_num_ck3(temp_s0);
    if (temp_s0_2 != 0) {
        if (( (var_s3 << 0x30) >> 0x30) >= 0x64) {
            var_s3 = 0x63;
        }
        temp_s2_2 =  (temp_s2 << 0x30) >> 0x30;
        temp_s1 =  (arg2 << 0x30) >> 0x30;
        if (temp_s2_2 >= temp_s1) {
            font_set_palette(0);
        } else if ((temp_s2_2 + ( (var_s3 << 0x30) >> 0x30)) >= temp_s1) {
            font_set_palette(6);
        } else {
            font_set_palette(0xA);
        }
        flfntLocate(0x12C, arg0);
        font_print(&lit_1287_006555F0, *(s32 *)((int)&item_str + (temp_s0_2 * 4)), temp_s2_2,  (var_s3 << 0x30) >> 0x30);
    }
}
