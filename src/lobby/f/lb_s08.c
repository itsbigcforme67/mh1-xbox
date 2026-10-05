/* lb_s08 - browser tag handlers (struct-array indexing) 0x00604BD0-0x00604CB8: body_data_set (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

void body_data_set(void) {
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    int temp_s0;
    int temp_a0;
    int temp_a0_2;
    int temp_a0_3;
    int temp_v0;

    temp_v0 = bsw;
    temp_s0 = F(u8, temp_v0, 0x17C);
    F(s16, temp_v0, 0x124) = 0;
    temp_a0 = bsw;
    F(s8, (F(s16, temp_a0, 0x124) + temp_a0), 0x168) = 3;
    temp_v0_2 = get_numeric_parameter5((bsw + 0x2A3));
    if (temp_v0_2 >= 0) {
        temp_a0_2 = bsw;
        BSC4(s32, temp_a0_2, F(s16, temp_a0_2, 0x124), 0x128) = temp_v0_2;
    }
    temp_v0_3 = get_numeric_parameter5((bsw + 0x193));
    if (temp_v0_3 >= 0) {
        F(s32, bsw, 0x120) = temp_v0_3;
    }
    temp_v0_4 = get_numeric_parameter5((bsw + 0x2B3));
    if (temp_v0_4 >= 0) {
        F(s32, bsw, 0x178) = temp_v0_4;
    }
    DispFontSize(temp_s0);
    temp_a0_3 = bsw;
    if ((F(s8, temp_a0_3, 0x186) == 0) && (F(u8, temp_a0_3, 0xE96B) == 0)) {
        stockBgColor(F(s32, temp_a0_3, 0x120));
        stockBgImage((bsw + 0x1A3));
    }
}
