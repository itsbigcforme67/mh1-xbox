/* lb_z139 - auto-drafted 0x005FEFE0-0x005FF054: tagAct_1402, tagAct_1403 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_1402(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(s16, bsw, 0x6EC) = get_numeric_parameter2(sp10);
    return 0;
}

s32 tagAct_1403(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(s16, bsw, 0x6EE) = get_numeric_parameter2(sp10);
    return 0;
}
