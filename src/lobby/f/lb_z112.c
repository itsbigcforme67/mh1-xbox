/* lb_z112 - auto-drafted 0x005FED80-0x005FEDC8: tagAct_132 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_132(void) {
    int temp_v1;

    temp_v1 = bsw;
    if ((F(s8, temp_v1, 0x186) == 0) && (F(u8, temp_v1, 0xE96B) == 0)) {
        stockActionURL(temp_v1 + 0x101C, F(u8, temp_v1, 0x111C));
    }
    return 0;
}
