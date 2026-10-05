#include "lobby_a.h"

void CallBack_NoticeUserMiniData(void) {
    char sp5C[0x40];
    int sp40;
    int var_s0;
    int var_s1;

    cnLBS_Get_NoticeUserMiniData(&sp40);
    if (softdip_ck(0xF1) == 1) {
        lb_check_mini_data( (Lb_get_plID(&sp40) << 0x38) >> 0x38, &sp40, sp5C);
        return;
    }
    var_s1 = 0;
    var_s0 = (int)&lbCommer;
loop_3:
    if (memcmp(var_s0, &sp40, 8) == 0) {
        memcpy((int)&lbCommer + (((s8)var_s1) * 0x5C) + 0x1C, sp5C, 0x40);
        return;
    }
    var_s1 =  ((var_s1 + 1) << 0x38) >> 0x38;
    var_s0 += 0x5C;
    if (var_s1 >= 8) {
        return;
    }
    goto loop_3;
}
