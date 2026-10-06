/* lb_by55 - agent B promoted near-match 0x00539460-0x00539504: check_items (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 check_items(u8 *p, s8 stocked) {
    s16 have;
    s16 stock;

    stock = 0;
    have = Ud_item_num_ck(*(u16 *)p);
    if (stocked == 0) {
        stock = Ud_stock_item_num_ck3(*(u16 *)p);
    }
    if (have + stock >= *(s16 *)(p + 2)) {
        return 1;
    }
    return 0;
}
