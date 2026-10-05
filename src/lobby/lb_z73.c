/* lb_z73 - auto-drafted 0x005FF0E0-0x005FF180: tagAct_163, tagAct_162 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

s32 tagAct_163(int arg0, s8 *arg1) {
    Option_tag_close_check(arg1);
    F(s8, bsw, 0x7F0) = 1;
    memset(bsw + 0x7F1, 0, 0x101);
    F(s32, bsw, 4) = 0;
    *arg1 = 0;
    return 0;
}

s32 tagAct_162(int arg0, int arg1) {
    Option_tag_close_check(arg1);
    F(s8, bsw, 0x7F0) = 1;
    memset(bsw + 0x7F1, 0, 0x101);
    return 0;
}
