/* lb_by57 - agent B promoted near-match 0x0053CBF0-0x0053CC30: lb_armor_itemSell (first drafted by tools/lbauto.py). */
#include "lobby_s.h"

s32 lb_armor_itemSell(void) {
    Lb_make_mySrcEquip(*(s16 *)((char *)lbShop.tbl + lbShop.cur * 8));
    cnWrap_SoundRequest(0);
    return 1;
}
