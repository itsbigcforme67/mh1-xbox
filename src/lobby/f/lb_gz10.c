/* lb_gz10 - browser table/tag handlers 0x00601ED0-0x00601F4C: get_numeric_parameter2 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 get_numeric_parameter2(int arg0) {
    s32 temp_v0;
    s32 var_a2;
    s32 var_a1;
    int var_s0;
    u8 temp_a0;

    var_s0 = arg0;
    temp_v0 = strlen();
    var_a1 = 0;
    var_a2 = 0;
    if (0 < temp_v0) {
        do {
            temp_a0 = (*(u8 *)var_s0);
            if ((temp_a0 < 0x3A) && (temp_a0 >= 0x30)) {
                var_a1 = var_a1 * 0xA;
                var_a1 = var_a1 + (temp_a0 - 0x30);
            }
            var_a2 += 1;
            var_s0 += 1;
        } while (var_a2 < temp_v0);
    }
    return var_a1;
}
