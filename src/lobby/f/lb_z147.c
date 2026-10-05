/* lb_z147 - auto-drafted 0x005E64B0-0x005E64F0: bs_cache_queue_get_last (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 bs_cache_queue_get_last(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_a0;
    s32 var_v0;

    var_a0 = arg0;
    var_v0 = 0;
    if (F(s32, (*(int *)var_a0), 0x104) == 0) {
        return 0;
    }
loop_2:
    var_a0 = (*(int *)var_a0);
    if (var_a0 != 0) {
        if (F(s32, var_a0, 0x104) != 0) {
            var_v0 = var_a0;
        }
        goto loop_2;
    }
    return var_v0;
}
