/* lb_z75 - auto-drafted 0x005FF380-0x005FF448: tagAct_183, tagAct_062, tagAct_180 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

s32 tagAct_183(int arg0, int arg1) {
    tagoutprintf6(arg1);
    F(s16, bsw, 0x8F2) = 4;
    F(s16, bsw, 0x8F4) = 0;
    DISP_HR_LINE(0);
    return 0;
}

s32 tagAct_062(int arg0, int arg1) {
    tagoutprintf6(arg1);
    F(s16, bsw, 0x8F2) = 4;
    F(s16, bsw, 0x8F4) = 0;
    return 0;
}

s32 tagAct_180(int arg0, int arg1) {
    tagoutprintf6(arg1);
    if (F(u16, bsw, 0x8F4) != 0) {
        DISP_HR_LINE(1);
    } else {
        DISP_HR_LINE(0);
    }
    return 0;
}
