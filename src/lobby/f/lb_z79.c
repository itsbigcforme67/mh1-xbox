/* lb_z79 - auto-drafted 0x005FEA30-0x005FEA88: tagAct_713, tagAct_714 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s32 bsw;

s32 tagAct_713(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x1428, 0x100);
    return 0;
}

s32 tagAct_714(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x1328, 0x100);
    return 0;
}
