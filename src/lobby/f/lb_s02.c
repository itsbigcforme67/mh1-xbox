/* lb_s02 - browser tag handlers (struct-array indexing) 0x005FE200-0x005FE478: tagAct_060, tagAct_065, tagAct_063, tagAct_064, tagAct_041 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_060(int arg0, s32 arg1) {
    int temp_v1;

    tagoutprintf6(arg1);
    temp_v1 = bsw;
    if (BSC4(u16, temp_v1, F(u16, temp_v1, 0x188), 0x153C) != 0) {
        tagoutprintf4(arg1);
    }
    return 0;
}

s32 tagAct_065(int arg0, s32 arg1) {
    u8 temp_v0;
    u8 temp_v1;
    int temp_a1;
    int temp_v1_2;

    tagoutprintf6(arg1);
    temp_a1 = bsw;
    temp_v0 = F(u8, temp_a1, 0xD892);
    if (temp_v0 != 0) {
        temp_v1 = BSC(u8, temp_a1, BSC2(u16, temp_a1, temp_v0, 0xD89A), 0x24FB);
        if ((temp_v1 == 3) || (temp_v1 == 4)) {
            temp_v1_2 = bsw;
            if (BSC4(u16, temp_v1_2, F(u16, temp_v1_2, 0x188), 0x153C) != 0) {
                tagoutprintf4(arg1, temp_a1);
            }
        }
    } else if (BSC4(u16, temp_a1, F(u16, temp_a1, 0x188), 0x153C) != 0) {
        tagoutprintf4(arg1, temp_a1);
    }
    return 0;
}

s32 tagAct_063(int arg0, s32 arg1) {
    int temp_v1;

    tagoutprintf6(arg1);
    temp_v1 = bsw;
    if (F(u8, temp_v1, 0x18D) == 0) {
        if (BSC4(u16, temp_v1, F(u16, temp_v1, 0x188), 0x153C) != 0) {
            tagoutprintf4(arg1);
        }
        F(u8, bsw, 0x18D) = 1U;
    }
    return 0;
}

s32 tagAct_064(int arg0, s32 arg1) {
    int temp_v1;

    F(s8, bsw, 0x18D) = 0;
    tagoutprintf6(arg1);
    temp_v1 = bsw;
    if (BSC4(u16, temp_v1, F(u16, temp_v1, 0x188), 0x153C) != 0) {
        tagoutprintf4(arg1);
    }
    return 0;
}

s32 tagAct_041(int arg0, s32 arg1) {
    int temp_a0;

    tagoutprintf4(arg1, arg1);
    temp_a0 = bsw;
    if (F(u8, temp_a0, 0xD892) != 0) {
        BSC(s16, temp_a0, F(u16, temp_a0, 0xD894), 0x251E) = 0;
    } else {
        F(s16, temp_a0, 0x12) = 0;
    }
    return 0;
}
