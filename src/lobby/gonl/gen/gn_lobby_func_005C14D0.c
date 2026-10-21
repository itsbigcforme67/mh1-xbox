/* G online (PC): lobby_func_005C14D0, written from Monster Hunter G's code at lobby 0x5C14D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void lobby_func_005C14D0(s8 *arg0, s32 arg1) {
    s32 temp_a1;
    s32 var_a2;
    s8 *var_a0;

    var_a0 = arg0;
    temp_a1 = arg1 & 0xFF;
    var_a2 = 0;
    if (temp_a1 > 0) {
loop_2:
        if (*var_a0 != 0) {
            var_a2 += 1;
            var_a0 += 0x18;
            if (var_a2 < temp_a1) {
                goto loop_2;
            }
        }
    }
}

