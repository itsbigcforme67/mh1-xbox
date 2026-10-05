/* lb_z123 - auto-drafted 0x005FF580-0x005FF5D8: tagAct_191, tagAct_192 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s32 bsw;

s32 tagAct_191(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x2C3, 0x10);
    return 0;
}

s32 tagAct_192(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x2D3, 0x10);
    return 0;
}
