/* lb_z109 - auto-drafted 0x005FE480-0x005FE504: tagAct_045 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_045(int arg0, int arg1) {
    int temp_a0;
    int temp_s0;

    tagoutprintf4(arg1);
    temp_a0 = bsw;
    if (F(u8, temp_a0, 0xD892) != 0) {
        temp_s0 = temp_a0 + (F(u16, temp_a0, 0xD894) * 0x5C) + 0x24E0;
        set_align_data(temp_s0);
        F(s16, temp_s0, 0x3E) = 0x3C;
    } else {
        F(s16, temp_a0, 0x12) = 0x3C;
    }
    return 0;
}
