/* lb_z74 - auto-drafted 0x005FF1D0-0x005FF228: tagAct_202, tagAct_031 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsw;

s32 tagAct_202(int arg0, int arg1) {
    Disp_Send_PullDown(arg1);
    F(s8, bsw, 0x7F0) = 0;
    return 0;
}

s32 tagAct_031(int arg0, int arg1) {
    F(s8, bsw, 0x17F) = 0;
    tagoutprintf3(arg1);
    return 0;
}
