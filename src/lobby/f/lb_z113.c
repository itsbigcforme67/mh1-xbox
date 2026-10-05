/* lb_z113 - auto-drafted 0x005FF630-0x005FF6A4: tagAct_203, tagAct_037 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s32 bsw;

s32 tagAct_203(int arg0, int arg1) {
    tagoutprintf3(arg1);
    memset(bsw + 0x6F0, 0, 0x101);
    Disp_Start_PullDown();
    return 0;
}

s32 tagAct_037(int arg0, int arg1) {
    tagoutprintf3(arg1);
    memset(bsw + 0x6F0, 0, 0x101);
    return 0;
}
