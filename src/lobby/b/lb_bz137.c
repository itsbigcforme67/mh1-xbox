/* lb_bz137 - lobby UI/client 0x005B73D0-0x005B7430: Lbc_connect (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void Lbc_connect(void) {
    s32 var_s0;

    if (Online_ck() == 1) {
        var_s0 = 0;
loop_2:
        if (cnLBS_RecvData(*(s32 *)0x4E36F4) == 1) {
            var_s0 += 1;
            F(s32, (u8 *)cw, 0x35F4) = 0xE10;
            if (var_s0 < 0x64) {
                goto loop_2;
            }
        }
    }
}
