/* lb_z140 - auto-drafted 0x005FF2B0-0x005FF2E8: tagAct_172 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_172(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(s8, bsw, 0x17F) = 3;
    Set_AnchorName(sp10);
    return 0;
}
