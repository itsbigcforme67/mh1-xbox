/* lb_bz19 - lobby UI/client 0x005B4D30-0x005B4D54: Lb_get_player_id (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void *Lb_get_player_id(s32 arg0) {
    return (u8 *)&lb_player + ((arg0 & 0xFF) * 0x38) + 0x24;
}
