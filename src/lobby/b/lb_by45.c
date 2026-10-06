/* lb_by45 - agent B promoted near-match 0x0053CA20-0x0053CAA4: lb_armor_select (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char shop_default_help[];

s32 lb_armor_select(void) {
    switch (F(s8, &lbShop, 0x19)) {       /* irregular */
    case 0:
        F(s32, &lbShop, 0x4C) = F(s32, &shop_default_help, 8);
        return lb_armor_itemBuy();
    case 1:
        F(s32, &lbShop, 0x4C) = F(s32, &shop_default_help, 0xC);
        return lb_armor_itemSell();
    default:
        return 0;
    }
}
