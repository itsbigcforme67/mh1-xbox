/* lb_z76 - auto-drafted 0x005FE600-0x005FE628: tagAct_100 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s32 bsw;

s32 tagAct_100(int arg0) {
    get_tag_in_parameter(arg0, bsw + 0xAF6, 0x100);
    return 0;
}
