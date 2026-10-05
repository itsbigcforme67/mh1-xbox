/* lb_z137 - auto-drafted 0x005FE630-0x005FE6A4: tagAct_101, tagAct_102 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_101(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(s16, bsw, 0xDF6) = get_numeric_parameter2(sp10);
    return 0;
}

s32 tagAct_102(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(s16, bsw, 0xDF8) = get_numeric_parameter2(sp10);
    return 0;
}
