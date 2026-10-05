/* lb_z88 - auto-drafted 0x005FF6E0-0x005FF740: tagAct_200, tagAct_201 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsw;

s32 tagAct_200(void) {
    Disp_Start_PullDown();
    return 0;
}

s32 tagAct_201(int arg0, s8 *arg1) {
    Option_tag_close_check(arg1);
    Disp_End_PullDown();
    F(s32, bsw, 4) = 0;
    *arg1 = 0;
    return 0;
}
