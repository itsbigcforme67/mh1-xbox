/* lb_s10 - browser tag handlers 2 0x005FEEB0-0x005FEF00: tagAct_144 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_144(s32 arg0) {
    u16 *pp;
    char sp10[0x108];
    int temp_v0;

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(u16, bsw, 0x4E6) = get_numeric_parameter2(sp10);
    temp_v0 = bsw;
    pp = &F(u16, temp_v0, 0x4E6);
    if (F(u16, temp_v0, 0x4E6) == 0) {
        *pp = 0x10U;
    }
    return 0;
}
