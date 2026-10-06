/* lb_s04 - browser tag handlers (struct-array indexing) 0x005FF8A0-0x005FF9C0: tagAct_400, tagAct_401 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_400(int arg0, s32 arg1) {
    int temp_a0;
    int temp_v1;

    tagoutprintf4(arg1, arg1);
    temp_a0 = bsw;
    if (F(u8, temp_a0, 0xD892) != 0) {
        BSC(u8, temp_a0, F(u16, temp_a0, 0xD894), 0x2530) = (u8) (BSC(u8, temp_a0, F(u16, temp_a0, 0xD894), 0x2530) | 1);
    } else {
        F(u8, temp_a0, 0x18B) = (u8) (F(u8, temp_a0, 0x18B) | 1);
    }
    F(s8, bsw, 0x18C) = 1;
    tagoutprintf4(arg1);
    return 0;
}

s32 tagAct_401(int arg0, s32 arg1) {
    int temp_a0;
    int temp_v1;

    tagoutprintf4(arg1, arg1);
    temp_a0 = bsw;
    if (F(u8, temp_a0, 0xD892) != 0) {
        BSC(u8, temp_a0, F(u16, temp_a0, 0xD894), 0x2530) = (u8) (BSC(u8, temp_a0, F(u16, temp_a0, 0xD894), 0x2530) & 0xFE);
    } else {
        F(u8, temp_a0, 0x18B) = (u8) (F(u8, temp_a0, 0x18B) & 0xFE);
    }
    F(s8, bsw, 0x18C) = 0;
    return 0;
}
