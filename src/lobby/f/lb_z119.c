/* lb_z119 - auto-drafted 0x0060DF20-0x0060DF98: plaza_log_id_chk_sub (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 plaza_log_id_chk_sub(s32 arg0, int arg1, s32 arg2) {
    int var_s1;
    s32 var_s0;

    var_s1 = arg1;
    var_s0 = arg2;
    if (arg2 != 0) {
loop_2:
        if (strcmp((*(s32 *)var_s1) + 0x44, arg0) == 0) {
            return 0;
        }
        var_s0 -= 1;
        var_s1 += 4;
        if (var_s0 == 0) {
            goto block_6;
        }
        goto loop_2;
    }
block_6:
    return 1;
}
