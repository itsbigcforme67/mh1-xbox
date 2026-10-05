/* lb_z122 - auto-drafted 0x005FDDC0-0x005FDE10: tagAct_110 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_110(int arg0) {
    char sp10[0x100];
    int temp_v1;

    get_tag_in_parameter(arg0, sp10, 0x100);
    temp_v1 = bsw;
    if ((F(s8, temp_v1, 0x186) == 0) && (F(u8, temp_v1, 0xE96B) == 0)) {
        stockBaseURL(sp10);
    }
    return 0;
}
