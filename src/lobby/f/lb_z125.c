/* lb_z125 - auto-drafted 0x005FF800-0x005FF830: tagAct_304 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_304(int arg0) {
    int temp_v1;

    temp_v1 = bsw;
    if (F(u8, temp_v1, 0x1120) != 0) {
        get_tag_in_parameter(arg0, temp_v1 + 0x1128, 0x100);
    }
    return 0;
}
