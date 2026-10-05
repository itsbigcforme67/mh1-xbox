/* lb_z84 - auto-drafted 0x005FF060-0x005FF094: tagAct_1404 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s32 bsw;

s32 tagAct_1404(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x5EB, 0x100);
    check_special_character(bsw + 0x5EB);
    return 0;
}
