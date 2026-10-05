/* lb_z80 - auto-drafted 0x005FEBE0-0x005FEC98: tagAct_121, tagAct_122, tagAct_123, tagAct_124 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s32 bsw;

s32 tagAct_121(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x193, 0x10);
    return 0;
}

s32 tagAct_122(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x1A3, 0x100);
    return 0;
}

s32 tagAct_123(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x2A3, 0x10);
    return 0;
}

s32 tagAct_124(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x2B3, 0x10);
    return 0;
}
