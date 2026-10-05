/* lb_bz51 - lobby UI/client 0x005C0A90-0x005C0AC0: CallBack_Event_ShutDownOpponent (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void CallBack_Event_ShutDownOpponent(void) {
    u8 temp_a0;
    void *temp_a1;

    temp_a1 = (u8 *)cw;
    temp_a0 = F(u8, temp_a1, 0x2C31);
    if ((temp_a0 != 5) && (temp_a0 == 4)) {
        F(s8, temp_a1, 0x2C0D) = 1;
    }
}
