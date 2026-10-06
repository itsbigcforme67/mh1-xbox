/* lb_s22 - tag handler 347 0x006013B0-0x0060141C: tagAct_347 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 *bsw;

s32 tagAct_347(s32 arg0) {
    u8 sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    if (sp10[0] == 0) {
        return 0;
    }
    F(s32, bsw, 0xF18) = get_numeric_parameter5(sp10);
    return 0;
}
