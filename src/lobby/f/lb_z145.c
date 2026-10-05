/* lb_z145 - auto-drafted 0x00601250-0x006012A4: tagAct_344 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_344(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(s16, bsw, 0xF12) = get_numeric_parameter2(sp10);
    return 0;
}
