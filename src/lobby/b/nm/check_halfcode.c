#include "lobby_a.h"
extern s8 *html_string_ptr;
extern u8 *dp;
void Split_TagCode();
void Analysis_TagCode();
void check_halfcode(void) {
    s32 var_a2;
    s32 var_a1;
    s32 var_v1;
    s8 temp_a0;
    u8 temp_a0_2;

    temp_a0 = *html_string_ptr;
    if ((temp_a0 != 0x5C) && (temp_a0 == 0x3C)) {
loop_3:
        var_a2 = 1;
        temp_a0_2 = (u8)*html_string_ptr;
        var_a1 = 0;
        if ((temp_a0_2 >= 0x80) && (temp_a0_2 < 0xA0)) {
            var_a1 = 1;
        }
        if (var_a1 == 0) {
            var_v1 = temp_a0_2 >= 0xE0;
            if (var_v1 != 0) {
                var_v1 = temp_a0_2 < 0x100;
            }
            if (var_v1 == 0) {
                var_a2 = 0;
            }
        }
        if ((var_a2 == 0) && (temp_a0_2 == 0x3C)) {
            html_string_ptr = html_string_ptr + 1;
            Split_TagCode();
            Analysis_TagCode();
            if (*(s32 *)(dp + 4) & 0x188) {

            } else {
                goto loop_3;
            }
        }
    }
}
