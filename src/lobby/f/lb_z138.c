/* lb_z138 - auto-drafted 0x005FED30-0x005FED80: tagAct_131 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;
extern char lit_1140_006677B8[];

s32 tagAct_131(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter2(arg0, sp10, 0x100);
    if (strncmp(sp10, &lit_1140_006677B8, 4) == 0) {
        F(s8, bsw, 0x111C) = 1;
    }
    return 0;
}
