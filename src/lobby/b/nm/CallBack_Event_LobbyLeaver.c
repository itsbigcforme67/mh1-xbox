#include "lobby_a.h"

void CallBack_Event_LobbyLeaver(void) {
    int sp40;
    int var_s1;
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s1_2;
    int temp_a1;
    int temp_a2;
    int temp_v1;

    cnLBS_Get_RoomLeaveUser(&sp40);
    var_s0 = 0;
    var_s1 = (int)&lbCommer;
loop_1:
    if (memcmp(var_s1, &sp40, 8) == 0) {
        memset((int)&lbCommer + (var_s0 * 0x5C), 0, 0x5C);
    } else {
        var_s0 += 1;
        var_s1 += 0x5C;
        if (var_s0 < 8) {
            goto loop_1;
        }
    }
    if (memcmp((u8 *)cw + 3, &sp40, 8) == 0) {
        var_s0_2 = 0;
        var_s1_2 = 0;
        F(s8, (u8 *)cw, 3) = 0;
        do {
            temp_a1 = (int)cw;
            temp_a2 = temp_a1 + var_s1_2;
            if ((F(s8, temp_a2, 0x132C) != 0) && ((F(s8, temp_a1, 3) == 0) || (memcmp(temp_a1 + 3, temp_a2 + 0x132C, 8) < 0))) {
                temp_v1 = (int)cw;
                memcpy(temp_v1 + 3, temp_v1 + var_s1_2 + 0x132C, 8);
            }
            var_s0_2 += 1;
            var_s1_2 += 0x2FC;
        } while (var_s0_2 < 8);
    }
}
