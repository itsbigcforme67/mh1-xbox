/* lb_z146 - auto-drafted 0x005E60D0-0x005E6134: bs_route_queue_add (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 bs_route_queue_add(s32 arg0, s32 arg1) {
    s32 temp_v0;
    s32 var_s0;
    s32 var_v1;

    var_v1 = 0;
    var_s0 = arg0;
loop_1:
    temp_v0 = F(s32, var_s0, 0);
    if (temp_v0 != 0) {
        var_v1 = var_s0;
        var_s0 = temp_v0;
        goto loop_1;
    }
    F(s32, var_s0, 0) = (*(int *)arg0);
    (*(int *)arg0) = var_s0;
    (*(int *)var_v1) = 0;
    BsUrlCopy_SS(var_s0 + 4, arg1);
    F(s32, var_s0, 0x104) = 1;
    return var_s0;
}
