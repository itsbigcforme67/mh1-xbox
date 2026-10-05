/* lb_s12 - browser tag handlers 2 0x00600120-0x00600194: tagAct_311 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_311(s32 arg0) {
    u8 *pp;
    char sp10[0x108];
    u8 temp_v0_2;
    int temp_v0;

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(u8, bsw, 0xDFC) = get_numeric_parameter2(sp10);
    temp_v0 = bsw;
    pp = &F(u8, temp_v0, 0xDFC);
    temp_v0_2 = F(u8, temp_v0, 0xDFC);
    if (temp_v0_2 != 0) {
        *pp = (u8) (temp_v0_2 + 1);
    }
    return 0;
}
