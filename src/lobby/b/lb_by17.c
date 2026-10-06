/* lb_by17 - agent B promoted near-match 0x005BB370-0x005BB3C8: CallBack_Result_ConditionSearchUser (first drafted by tools/lbauto.py). */
#include "lobby_b.h"

void CallBack_Result_ConditionSearchUser(CNET_RES res) {
    int temp_a1;

    temp_a1 = (int)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xA)) {
        if (res.val == 0) {
            F(u8, temp_a1, 0x2C35) = (u8) (F(u8, temp_a1, 0x2C35) + 1);
        } else {
            F(u8, temp_a1, 0x2C35) = 3U;
        }
    }
}
