/* lb_z117 - auto-drafted 0x00603780-0x006037C4: Disp_Start_PullDown (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

void Disp_Start_PullDown(void) {
    int temp_a0;

    F(s16, bsw, 0x18) = 0;
    temp_a0 = bsw;
    if ((F(s8, temp_a0, 0x186) == 0) && (F(u8, temp_a0, 0xE96B) == 0)) {
        stockStartPulldown(temp_a0 + 0x6F0);
    }
}
