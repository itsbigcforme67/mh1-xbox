/* lb_z77 - auto-drafted 0x005FE6B0-0x005FE718: tagAct_104, tagAct_106 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s32 bsw;

s32 tagAct_104(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0xBF6, 0x100);
    check_special_character(bsw + 0xBF6);
    return 0;
}

s32 tagAct_106(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0xCF6, 0x100);
    return 0;
}
