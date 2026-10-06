/* lb_by22 - agent B promoted near-match 0x005C30C0-0x005C314C: Get_ServerColor (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char BsLbsInfo[];
extern char ConnectLbsId[];
extern char netr_sub01_col[];

s32 Get_ServerColor(void) {
    s32 var_s1;
    int var_s0;

    var_s1 = 0;
    var_s0 = (int)&BsLbsInfo;
loop_1:
    if (memcmp(&ConnectLbsId, var_s0, 0xC) != 0) {
        var_s1 += 1;
        var_s0 += 0x102;
        if (var_s1 >= 0xA) {

        } else {
            goto loop_1;
        }
    }
    if (var_s1 < 0xA) {
        return ((int *)&netr_sub01_col)[var_s1];
    }
    return -1;
}
