/* lb_z14 - auto-drafted 0x005E6260-0x005E62A4: bs_route_queue_free_after (first drafted by tools/lbauto.py). */
#include "lobby.h"

s32 bs_route_queue_free_after(s32 *arg0, s32 arg1) {
    s32 var_a1;

    var_a1 = arg1;
loop_1:
    if (*arg0 != var_a1) {
        var_a1 = bs_route_queue_free_reverse(arg0, var_a1);
        goto loop_1;
    }
    return var_a1;
}
