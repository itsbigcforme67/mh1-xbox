/* lbshop2, run 3: lb_shop_listIcon .. lb_shop_listIcon (lobby.bin 0x005AF430-0x005AF4D4): the matching functions of lbshop2_nm.c. */
#pragma readonly_strings on
#include "lbshop2_proto.h"

void lb_shop_listIcon(int x, int y, int z, s16 n) {
    int v;

    if (lbShop.mode == 0) {
        v = lbShop.tbl[n + lbShop.x6C * 7];
    } else {
        int k = n + lbShop.x6C * 7;
        if (User_data[0].item[k].num <= 0) return;
        v = User_data[0].item[k].id;
    }
    Lb_put_itemIcon(x, y, z, v);
}
