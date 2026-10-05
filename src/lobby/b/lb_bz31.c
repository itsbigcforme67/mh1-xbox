/* lb_bz31 - lobby UI/client 0x005B9530-0x005B959C: lbc_browser_05 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 BsLbsErrNum;
extern s8 net_char_change;

void lbc_browser_05(void) {
    s32 temp_a0;
    void *temp_v1;

    temp_a0 = Fade_busy_ck() & 0xFF;
    if (temp_a0 != 1) {
        temp_v1 = (u8 *)cw;
        F(u8, temp_v1, 0x2C43) = (u8) (F(u8, temp_v1, 0x2C43) + 1);
        all_reset(temp_a0);
        Lbs_load();
        net_char_change = 0;
        F(s8, (u8 *)cw, 0x2C08) = 1;
        F(s8, (u8 *)cw, 0x2C44) = 2;
        BsLbsErrNum = 0;
    }
}
