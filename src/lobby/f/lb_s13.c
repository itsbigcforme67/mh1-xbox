/* lb_s13 - browser tag handlers 2 0x006012B0-0x006013A4: tagAct_345, tagAct_346 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_345(s32 arg0) {
    u8 *pp;
    char sp10[0x108];
    int temp_v0;

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(u8, bsw, 0xF14) = get_numeric_parameter2(sp10);
    temp_v0 = bsw;
    pp = &F(u8, temp_v0, 0xF14);
    if (F(u8, temp_v0, 0xF14) == 1) {
        *pp = 0U;
    }
    return 0;
}

s32 tagAct_346(s32 arg0) {
    u8 *pp;
    char sp10[0x108];
    int temp_v0;

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(u8, bsw, 0xF15) = get_numeric_parameter2(sp10);
    temp_v0 = bsw;
    pp = &F(u8, temp_v0, 0xF15);
    if (F(u8, temp_v0, 0xF15) == 1) {
        *pp = 0U;
    }
    return 0;
}
