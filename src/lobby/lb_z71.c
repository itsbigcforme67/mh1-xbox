/* lb_z71 - auto-drafted 0x005FEBB0-0x005FEBD8: tagAct_125 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

s32 tagAct_125(int arg0, s8 *arg1) {
    F(s32, bsw, 4) = 0;
    *arg1 = 0;
    tagAct_120();
    return 0;
}
