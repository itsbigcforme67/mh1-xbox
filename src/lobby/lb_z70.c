/* lb_z70 - auto-drafted 0x005FE580-0x005FE5F8: tagAct_030, tagAct_105, tagAct_036 (first drafted by tools/lbauto.py). */
#include "lobby.h"

s32 tagAct_030(int arg0, int arg1) {
    tagoutprintf3(arg1);
    return 0;
}

s32 tagAct_105(int arg0, int arg1) {
    tagoutprintf3(arg1);
    Init_Image();
    Disp_Image();
    return 0;
}

s32 tagAct_036(int arg0, int arg1) {
    tagoutprintf3(arg1);
    Init_Image();
    return 0;
}
