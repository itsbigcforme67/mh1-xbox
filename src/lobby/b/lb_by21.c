/* lb_by21 - agent B promoted near-match 0x005C0B10-0x005C0B60: get_font_col (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 D_35C7B1[];
extern u8 color_tbl[8];

s32 get_font_col(s32 arg0) {
    s32 temp_v1;

    temp_v1 = arg0 & 0xFF;
    if (temp_v1 == 0 || !(D_35C7B1[temp_v1] & 4)) {
        arg0 = 0;
    } else {
        arg0 = color_tbl[(temp_v1 - 0x30) & 7];
    }
    return arg0;
}
