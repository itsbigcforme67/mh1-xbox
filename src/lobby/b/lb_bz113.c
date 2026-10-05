/* lb_bz113 - lobby UI/client 0x005BBF10-0x005BBF94: CallBack_GetDate (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_GetDate(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x17)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            cnLBS_Get_TimingValue((u8 *)cw + 0xBF3C, temp_a1, temp_a1 + 0x2C45);
        } else {
            F(s32, (u8 *)cw, 0xBF3C) = 0;
        }
        temp_a0 = (u8 *)cw;
        F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
    }
}
