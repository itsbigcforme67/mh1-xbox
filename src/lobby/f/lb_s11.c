/* lb_s11 - browser tag handlers 2 0x005FF450-0x005FF508: tagAct_181, tagAct_182 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_181(s32 arg0) {
    u16 *pp;
    char sp10[0x108];
    int temp_v0;

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(u16, bsw, 0x8F2) = get_numeric_parameter2(sp10);
    temp_v0 = bsw;
    pp = &F(u16, temp_v0, 0x8F2);
    if (F(u16, temp_v0, 0x8F2) < 4) {
        *pp = 4U;
    }
    return 0;
}

s32 tagAct_182(s32 arg0) {
    u16 *pp;
    char sp10[0x108];
    int temp_v0;

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(u16, bsw, 0x8F4) = get_numeric_parameter4(sp10, 0);
    temp_v0 = bsw;
    pp = &F(u16, temp_v0, 0x8F4);
    if (F(u16, temp_v0, 0x8F4) < 2) {
        *pp = 2U;
    }
    return 0;
}
