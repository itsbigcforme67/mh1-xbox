#include "lobby_a.h"

void net_Check_FriendSuu(int arg0, s32 arg1) {
    s32 temp_a1;
    s32 var_a2;
    int var_a0;

    var_a0 = arg0;
    temp_a1 = arg1 & 0xFF;
    var_a2 = 0;
    if (temp_a1 > 0) {
loop_2:
        if ((*(s8 *)var_a0) != 0) {
            var_a2 += 1;
            var_a0 += 0x30;
            if (var_a2 < temp_a1) {
                goto loop_2;
            }
        }
    }
}
