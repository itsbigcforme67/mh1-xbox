/* lb_by49 - agent B promoted near-match 0x0053C220-0x0053C23C: lb_armor_init (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern s32 shop_armor00_tag[1];

void lb_armor_init(void) {
    lbShop.tag = shop_armor00_tag;
    lbShop.x17 = 2;
}
