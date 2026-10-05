/* lb_bz39 - lobby UI/client 0x005BD520-0x005BD530: Lbs_InRoomCheck (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

s32 Lbs_InRoomCheck(void) {
    return F(u8, (u8 *)cw, 0x35D3) != 0;
}
