/* lb_by47 - agent B promoted near-match 0x00538620-0x0053863C: lb_process_init (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern s32 shop_process00_tag[1];

void lb_process_init(void) {
    lbShop.tag = shop_process00_tag;
    lbShop.x17 = 2;
}
