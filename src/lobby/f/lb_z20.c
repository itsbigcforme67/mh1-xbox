/* lb_z20 - auto-drafted 0x005E91C0-0x005E9208: _my_tolower (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void _my_tolower(s8 *arg0) {
    s8 *var_a0;
    s8 var_a1;

    var_a0 = arg0;
    var_a1 = *var_a0;
    if (var_a1 != 0) {
        do {
            if ((var_a1 >= 0x41) && (var_a1 < 0x5B)) {
                *var_a0 += 0x20;
            }
            var_a0 += 1;
            var_a1 = *var_a0;
        } while (var_a1 != 0);
    }
}
