/* lb_z18 - auto-drafted 0x005E7360-0x005E73B0: bs_cache_queue_image_free_all (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void bs_cache_queue_image_free_all(s32 arg0) {
    s32 var_a0;

    var_a0 = bs_cache_queue_get_last();
    if (var_a0 != 0) {
        do {
            bs_image_cache_clear(var_a0);
            var_a0 = bs_cache_queue_get_last(arg0);
        } while (var_a0 != 0);
    }
}
