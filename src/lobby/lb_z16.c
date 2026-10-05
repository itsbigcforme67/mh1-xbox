/* lb_z16 - auto-drafted 0x005E6A50-0x005E6A5C: BsCacheCleanup (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern char BcImage_head[];

void BsCacheCleanup(void) {
    bs_cache_queue_image_free_all(&BcImage_head);
}
