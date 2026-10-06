#include "lobby_a.h"
extern f32 html_default_x;
extern f32 html_default_y;
extern f32 html_x;
extern f32 html_y;
extern f32 html_z;
extern s32 html_color;
extern s32 html_size;
extern s8 html_layout;
extern s8 html_start_flag;
extern int html_string_ptr;
extern u8 html_end_flag;
void nwDispStr_Html(f32 arg1, int arg0) {
    s32 var_a2;
    s32 var_a3;
    s32 var_s0;
    s32 var_v1;
    u8 temp_a1;

    var_a3 = 1;
    temp_a1 = (*(u8 *)arg0);
    var_a2 = 0;
    if ((temp_a1 >= 0x80) && (temp_a1 < 0xA0)) {
        var_a2 = 1;
    }
    if (var_a2 == 0) {
        var_v1 = temp_a1 >= 0xE0;
        if (var_v1 != 0) {
            var_v1 = temp_a1 < 0x100;
        }
        if (var_v1 == 0) {
            var_a3 = 0;
        }
    }
    if (var_a3 == 0) {
        html_string_ptr = arg0;
        html_size = 5;
        html_z = arg1;
        html_layout = 6;
        html_x = arg0;
        html_default_x = arg0;
        html_color = 0;
        html_y = arg1;
        html_start_flag = 0;
        html_default_y = arg1;
        html_end_flag = 0U;
        cnWrap_SetFontColor(0, temp_a1, var_a2);
        flfntSetSize(0x14, 0x14);
        var_s0 = 0;
loop_10:
        Analysis_StringData();
        Display_StringData(0);
        if (html_end_flag == 0) {
            var_s0 += 1;
            if (var_s0 < 0x1A) {
                goto loop_10;
            }
        }
        cnWrap_SetFontColor(0);
        flfntSetSize(0x14, 0x14);
    }
}
