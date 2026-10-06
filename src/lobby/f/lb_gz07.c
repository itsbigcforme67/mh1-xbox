/* lb_gz07 - browser table/tag handlers 0x006009B0-0x00600A5C: tagAct_320 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_320(int arg0) {
    int temp_a1;
    int temp_s1;
    s32 temp_s0;
    int temp_v0;

    temp_a1 = bsw;
    temp_v0 = temp_a1 + (F(u16, temp_a1, 0xD894) * 0x5C);
    temp_s0 = F(s32, temp_v0, 0x24E0);
    temp_s1 = temp_v0 + 0x24E0;
    if (temp_s0 == 0) {
        return -1;
    }
    F(s8, temp_a1, 0x18D) = 0;
    F(s8, temp_s1, 0x4C) = 0;
    if (F(s8, bsw, 0x186) == -0xA) {
        F(s32, temp_s1, 0x14) = (*(s32 *)arg0);
    }
    set_TR_data_1st(temp_s1, temp_s0);
    set_TR_position(temp_s1, temp_s0);
    return 0;
}
