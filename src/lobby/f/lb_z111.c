/* lb_z111 - auto-drafted 0x005FECC0-0x005FECF4: tagAct_133 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s32 bsw;

s32 tagAct_133(int arg0, int arg1) {
    tagoutprintf6(arg1);
    memset(bsw + 0x101C, 0, 0x101);
    return 0;
}
