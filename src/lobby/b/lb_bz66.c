/* lb_bz66 - lobby UI/client 0x005B1E00-0x005B1E74: nwSetEff_FreeDialog (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char tk_dialog_mv02[];

void nwSetEff_FreeDialog(s32 arg0, s32 arg1) {
    int temp_a1;
    int temp_v0;

    F(u8, (u8 *)cw, 0x2F79) = 0xFF;
    F(s8, (u8 *)cw, 0x2F78) = 1;
    F(s8, (u8 *)cw, 0x2F7B) = 0;
    temp_v0 = pull_set_work(1);
    if (temp_v0 != 0) {
        temp_a1 = F(int, temp_v0, 0x18);
        F(int, temp_v0, 0x20) = (int)&tk_dialog_mv02;
        F(s16, temp_a1, 0x14) = (s16) (arg0 & 0xFF);
        F(s32, temp_a1, 0x18) = arg1;
    }
}
