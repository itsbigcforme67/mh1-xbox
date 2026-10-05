/* lb_z105 - auto-drafted 0x005E6600-0x005E6644: bs_source_cache_clear (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char BcSource_head[];

void bs_source_cache_clear(int arg0) {
    bs_cache_queue_free_node(&BcSource_head, arg0);
    memset(F(s32, arg0, 0x10C), 0, 0x8000);
    F(s32, arg0, 0x110) = 0;
}
