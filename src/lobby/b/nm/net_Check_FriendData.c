#include "lobby_a.h"

s32 net_Check_FriendData(int arg0, s32 arg1, int arg2) {
    s32 temp_s0;
    s32 var_s2;
    int var_s1;

    temp_s0 = arg1 & 0xFF;
    var_s2 = 0;
    if (temp_s0 > 0) {
        var_s1 = arg0;
loop_2:
        if (((*(s8 *)var_s1) != 0) && (memcmp(var_s1, arg2, 8) == 0)) {
            return var_s2;
        }
        var_s2 += 1;
        var_s1 += 0x30;
        if (var_s2 >= temp_s0) {
            goto block_6;
        }
        goto loop_2;
    }
block_6:
    return -1;
}
