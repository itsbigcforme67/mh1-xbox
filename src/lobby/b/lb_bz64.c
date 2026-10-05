/* lb_bz64 - lobby UI/client 0x005C4E20-0x005C4E60: lobby_visual, lobby_to_quest (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

s32 lobby_visual(void) {
    Visual_main();
    return 0;
}

s32 lobby_to_quest(void) {
    all_reset();
    return 1;
}
