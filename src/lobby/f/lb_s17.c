/* lb_s17 - tag handlers with tag buffers 0x005FF7C0-0x005FF800: tagAct_303 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 *bsw;

s32 tagAct_303(s32 arg0) {
    u8 sp10[0x108];

    if (F(u8, bsw, 0x1120) != 0) {
        get_tag_in_parameter3(arg0, sp10, 0x100);
        F(s32, bsw, 0x1124) = get_numeric_parameter2(sp10);
    }
    return 0;
}
