#include "lobby_s.h"

s32 lb_armor_itemSell(void) {
    Lb_make_mySrcEquip(lbShop.tbl[(lbShop.cur) * 2]);
    cnWrap_SoundRequest(0);
    return 1;
}
