/* lb_s05 - browser tag handlers (struct-array indexing) 0x00600260-0x006002F8: tagAct_314 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_314(s32 arg0) {
    char sp20[0x108];
    int var_s0;
    int temp_a0;
    int temp_v0;

    var_s0 = 0;
    get_tag_in_parameter(arg0, sp20, 0x100);
    temp_a0 = bsw;
    if (F(s8, temp_a0, 0x186) != -0xA) {
        return 0;
    }
    temp_v0 = BSC(int, temp_a0, F(u16, temp_a0, 0xD894), 0x24E0);
    if (temp_v0 != 0) {
        var_s0 = F(u16, temp_v0, 0x1C);
    }
    F(s16, bsw, 0xE02) = get_numeric_parameter4(sp20, var_s0);
    return 0;
}
