/* lb_s01 - browser tag handlers (struct-array indexing) 0x005FDE10-0x005FDF1C: tagAct_bon, tagAct_bof (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_bon(int arg0, s32 arg1) {
    int temp_a1;

    tagoutprintf3(arg1);
    temp_a1 = bsw;
    if (F(u8, temp_a1, 0xD892) != 0) {
        BSC(s8, temp_a1, F(u16, temp_a1, 0xD894), 0x252F) = 1;
    } else {
        F(s8, temp_a1, 0x17C) = 1;
    }
    DispFontSize(1, temp_a1);
    return 0;
}

s32 tagAct_bof(int arg0, s32 arg1) {
    int temp_a0;
    int temp_a1;

    tagoutprintf3(arg1);
    temp_a1 = bsw;
    if (F(u8, temp_a1, 0xD892) != 0) {
        if (BSC(u8, temp_a1, F(u16, temp_a1, 0xD894), 0x24FB) == 3) {
            return 0;
        }
        BSC(s8, temp_a1, F(u16, temp_a1, 0xD894), 0x252F) = 0;
        goto block_5;
    }
    F(s8, temp_a1, 0x17C) = 0;
block_5:
    DispFontSize(0, temp_a1);
    return 0;
}
