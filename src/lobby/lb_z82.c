/* lb_z82 - auto-drafted 0x005FEE40-0x005FEEA4: tagAct_142, tagAct_143 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s32 bsw;

s32 tagAct_142(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x2E4, 0x100);
    return 0;
}

s32 tagAct_143(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x3E4, 0x100);
    check_special_character(bsw + 0x3E4);
    return 0;
}
