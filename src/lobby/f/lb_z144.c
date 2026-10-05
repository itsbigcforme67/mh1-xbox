/* lb_z144 - auto-drafted 0x006003D0-0x00600424: tagAct_317 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_317(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(s32, bsw, 0xE08) = get_numeric_parameter5(sp10);
    return 0;
}
