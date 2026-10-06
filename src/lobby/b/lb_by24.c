/* lb_by24 - agent B promoted near-match 0x005C4DD0-0x005C4E14: lobby_80 (first drafted by tools/lbauto.py). */
#include "lobby_b.h"

s32 lobby_80(void) {
    all_reset();
    lb_sys.x70 = 0;
    lb_sys.x02 = 0;
    lb_sys.x01++;
    return 0;
}
