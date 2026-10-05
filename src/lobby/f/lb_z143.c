/* lb_z143 - auto-drafted 0x00600300-0x00600354: tagAct_315 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_315(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(s16, bsw, 0xE04) = get_numeric_parameter2(sp10);
    return 0;
}
