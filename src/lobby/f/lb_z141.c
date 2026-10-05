/* lb_z141 - auto-drafted 0x005FF770-0x005FF7C0: tagAct_302 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;
extern char lit_1465_006677C8[];

s32 tagAct_302(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter2(arg0, sp10, 0x100);
    if (strncmp(sp10, &lit_1465_006677C8, 7) == 0) {
        F(s8, bsw, 0x1120) = 1;
    }
    return 0;
}
