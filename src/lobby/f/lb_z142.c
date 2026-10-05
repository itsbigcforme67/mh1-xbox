/* lb_z142 - auto-drafted 0x006001A0-0x00600254: tagAct_312, tagAct_313 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_312(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(s16, bsw, 0xDFE) = get_numeric_parameter2(sp10);
    return 0;
}

s32 tagAct_313(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(s16, bsw, 0xE00) = get_numeric_parameter2(sp10);
    return 0;
}
