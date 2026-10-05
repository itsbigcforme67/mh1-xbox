#include "lobby_a.h"
extern char BsLbsInfo[];
extern char ConnectLbsId[];
extern char netr_sub01_col[];
s32 Get_ServerColor(void) {
    int var_s0;
    s32 var_s1;

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
