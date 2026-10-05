/* lb_bz70 - lobby UI/client 0x005AEA40-0x005AEA50: lb_shop_init (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char shop_default_tag_00389E90[4];
typedef struct { u8 pad[0x48]; int tag; } LBSHOP48;

void lb_shop_init(void) {
    ((LBSHOP48 *)lbShop)->tag = (int)shop_default_tag_00389E90;
}
