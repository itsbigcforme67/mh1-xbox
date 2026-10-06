#include "lobby_a.h"

s32 net_Check_FriendData(char *arg0, s32 arg1, char *arg2) {
    s32 var_s2;
    char *var_s1;
    s32 temp_s0;

    temp_s0 = arg1 & 0xFF;
    var_s2 = 0;
    var_s1 = arg0;
    while (var_s2 < temp_s0) {
        if (*var_s1 != 0 && memcmp(var_s1, arg2, 8) == 0) return var_s2;
        var_s2++;
        var_s1 += 0x30;
    }
    return -1;
}
