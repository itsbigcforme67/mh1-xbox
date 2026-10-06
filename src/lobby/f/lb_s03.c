/* lb_s03 - browser tag handlers (struct-array indexing) 0x005FE510-0x005FE578: tagAct_061 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_061(int arg0, s32 arg1) {
    int temp_a0;

    temp_a0 = bsw;
    if (F(u8, temp_a0, 0xD892) != 0) {
        BSC(s16, temp_a0, F(u16, temp_a0, 0xD894), 0x251E) = 0;
    } else {
        F(s16, temp_a0, 0x12) = 0;
    }
    tagoutprintf6(arg1);
    return 0;
}
