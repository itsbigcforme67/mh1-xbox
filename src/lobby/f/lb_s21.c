/* lb_s21 - tag handlers with tag buffers 0x00600AE0-0x00600B2C: tagAct_323 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 *bsw;

s32 tagAct_323(s32 arg0) {
    u8 sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (sp10[0] == 0) {
        return 0;
    }
    F(s32, bsw, 0xF18) = get_numeric_parameter5(sp10);
    return 0;
}
