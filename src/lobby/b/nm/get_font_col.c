#include "lobby_f.h"
extern char D_35C7B1[];
extern char color_tbl[];
u8 get_font_col(s32 arg0) {
    s32 temp_v1;
    u8 var_a0;

    temp_v1 = arg0 & 0xFF;
    if (temp_v1 != 0) {
        if (!(*((u8 *)&D_35C7B1 + temp_v1) & 4)) {
            goto block_3;
        }
        var_a0 = *((u8 *)&color_tbl + ((temp_v1 - 0x30) & 7));
    } else {
block_3:
        var_a0 = 0;
    }
    return var_a0;
}
