/* lb_s09 - browser tag handlers 2 0x005FE740-0x005FE7A0: tagAct_700, tagAct_701 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 *bsw;

s32 tagAct_700(s32 arg0) {
    get_tag_in_parameter(arg0, bsw + 0xFAC1, 0x100);
    return 0;
}

s32 tagAct_701(void) {
    memset(bsw + 0xFAC1, 0, 0x100);
    return 0;
}
