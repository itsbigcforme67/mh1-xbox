/* lb_z85 - auto-drafted 0x005FF180-0x005FF1A8: tagAct_160 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s32 bsw;

s32 tagAct_160(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0x7F1, 0x100);
    return 0;
}
