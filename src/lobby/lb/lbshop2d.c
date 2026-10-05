/* lbshop2, run 4: lb_shop_decide .. lb_shop_decide (lobby.bin 0x005AFCC0-0x005AFE58): the matching functions of lbshop2_nm.c. */
#pragma readonly_strings on
#include "lbshop2_proto.h"

void lb_shop_decide(void) {
    int id;

    if (lbShop.mode == 0) id = lbShop.tbl[lbShop.cur];
    else id = User_data[0].item[lbShop.cur].id;
    switch (lbShop.mode) {
    case 0:
        Gold_add(-(lbShop.qty * shopList[lbShop.cur].price), lbShop.cur, lbShop.mode);
        if (lb_monster_list_check(id) == 1) {
            Add_to_Monster_list((id - 0x125) & 0xFF);
            Lb_put_set01(5);
        } else {
            Ud_item_stack(id & 0xFFFF, (s16)lbShop.qty);
        }
        cnWrap_SoundRequest(8);
        break;
    case 1:
        Gold_add(lbShop.qty * shopList[lbShop.cur].price, lbShop.cur, lbShop.mode);
        cnWrap_SoundRequest(8);
        if (Item_data[id].max != 0xFF) {
            Ud_item_stack(id & 0xFFFF, (s16)-lbShop.qty);
        } else {
            Ud_item_erase((s16)lbShop.cur);
        }
        break;
    }
    lbShop.help = 0;
}
