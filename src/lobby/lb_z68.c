/* lb_z68 - auto-drafted 0x005FDF20-0x005FDFD8: tagAct_028, tagAct_029, tagAct_038, tagAct_039 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

s32 tagAct_028(int arg0, int arg1) {
    tagoutprintf3(arg1);
    F(s8, bsw, 0x17E) = 1;
    return 0;
}

s32 tagAct_029(int arg0, int arg1) {
    tagoutprintf3(arg1);
    F(s8, bsw, 0x17E) = 0;
    return 0;
}

s32 tagAct_038(int arg0, int arg1) {
    tagoutprintf3(arg1);
    F(s8, bsw, 0x17D) = 1;
    return 0;
}

s32 tagAct_039(int arg0, int arg1) {
    tagoutprintf3(arg1);
    F(s8, bsw, 0x17D) = 0;
    return 0;
}
