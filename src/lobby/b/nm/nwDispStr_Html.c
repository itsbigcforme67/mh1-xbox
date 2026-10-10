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
void Analysis_StringData(void);
void Display_StringData(f32 size, f32 line_h);   /* html_text.c; the asm passes 0.0 in f12 and f13 */
/* (x, y, z, string): the asm takes the floats in f12-f14 and the string in a0 (callers lb_by180.c, lb_ui.c, lb_c506.c);
 * the m2c draft had (f32, int), so on the PC the string pointer was the y float's bits: a server's error message
 * (e.g. 6406 "this room is full") crashed the dialog. Rewritten from the asm's stores (agent B, 11 Oct 2026). */
void nwDispStr_Html(f32 x, f32 y, f32 z, int arg0) {
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
        html_z = z;
        html_layout = 6;
        html_x = x;
        html_default_x = x;
        html_color = 0;
        html_y = y;
        html_start_flag = 0;
        html_default_y = y;
        html_end_flag = 0U;
        cnWrap_SetFontColor(0, temp_a1, var_a2);
        flfntSetSize(0x14, 0x14);
        var_s0 = 0;
loop_10:
        Analysis_StringData();
        Display_StringData(0.0f, 0.0f);
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
