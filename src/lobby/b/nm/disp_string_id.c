#include "lobby_a.h"
extern char lit_273_0065EC20[];
extern char lit_274_0065EC28[];
void disp_string_id(s32 arg0, s32 arg1) {
    char sp30[0x1E];
    s32 temp_v1_2;
    int temp_s0;
    int var_v0;
    int temp_v1;

    memset(sp30, 0, 0x1E);
    temp_v1 = ((arg1 & 0xFF) * 8) + (s32)cw;
    if (F(s8, temp_v1, 0xB) == 0) {
        strcpy(sp30, &lit_273_0065EC20);
    } else {
        han2zen(temp_v1 + 0xB, sp30);
    }
    flfntSetSize(0x14, 0x14);
    temp_v1_2 = arg1 & 0xFF;
    var_v0 = 4;
    if ((arg0 & 0xFF) != temp_v1_2) {
        var_v0 = 0;
    }
    temp_s0 = (temp_v1_2 * 0x58) + 0x7A;
    font_print_double(0x1A4,  (temp_s0 << 0x30) >> 0x30, 1, (s8)var_v0);
    flfntLocate(0x168,  (temp_s0 << 0x30) >> 0x30);
    font_print(&lit_274_0065EC28);
}
