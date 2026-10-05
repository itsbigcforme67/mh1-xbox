#include "lobby_a.h"
extern char lit_4327[];
void CallBack_Event_LobbyCommer(void) {
    s8 sp168;
    char sp160[0x5C];
    s8 sp15F;
    s8 sp15E;
    s8 sp15D;
    int sp5C;
    char sp40[0x120];
    s32 var_a0;
    s32 var_s1;
    int var_a1;
    int var_s0;
    int temp_v0;

    cnLBS_Get_RoomLeaveUser(sp160);
    temp_v0 = (int)cw;
    if ((F(s8, temp_v0, 3) == 0) || (memcmp(temp_v0 + 3, sp160, 8) < 0)) {
        memcpy((u8 *)cw + 3, sp160, 8);
    }
    var_s1 = 0;
    var_s0 = (int)&lbCommer;
loop_4:
    if (memcmp(var_s0, sp160, 8) != 0) {
        var_s1 += 1;
        var_s0 += 0x5C;
        if (var_s1 >= 8) {
            var_a0 = 0;
            var_a1 = (int)&lbCommer;
loop_7:
            if ((*(s8 *)var_a1) == 0) {
                memcpy((int)&lbCommer + (var_a0 * 0x5C), sp160, 0x5C);
            } else {
                var_a0 += 1;
                var_a1 += 0x5C;
                if (var_a0 < 8) {
                    goto loop_7;
                }
            }
            if ((F(u8, (u8 *)cw, 0x35D5) != 0) && (sp168 != 0)) {
                memset(sp40, 0, 0x120);
                sp15F = 6;
                sp15E = 6;
                sp15D = 6;
                sprintf(&sp5C, &lit_4327, &sp168);
                Chat_log_add(0, sp40);
            }
        } else {
            goto loop_4;
        }
    }
}
