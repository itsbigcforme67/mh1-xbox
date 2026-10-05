/* lb_by14 - agent B promoted near-match 0x005B8E80-0x005B8EC8: CallBack_Result_LoginTopInformation (first drafted by tools/lbauto.py). */
#include "lobby_b.h"

void CallBack_Result_LoginTopInformation(CNET_RES res) {
    int temp_a1;

    temp_a1 = (int)cw;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        if (res.val == 0) {
            F(u8, temp_a1, 0x2C34) = (u8) (F(u8, temp_a1, 0x2C34) + 1);
        } else {
            F(u8, temp_a1, 0x2C34) = 7U;
        }
    }
}
