/* lb_s16 - tag handlers with tag buffers 0x005FF230-0x005FF2A8: tagAct_171 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 *bsw;

s32 tagAct_171(s32 arg0) {
    u8 sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (sp10[0] == 0x23) {
        F(s8, bsw, 0x17F) = 1;
        strcpy((bsw + 0x8F6), sp10);
    } else {
        F(s8, bsw, 0x17F) = 2;
        strcpy((bsw + 0x9F6), sp10);
    }
    return 0;
}
