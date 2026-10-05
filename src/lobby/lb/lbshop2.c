/* lbshop2, run 1: Lb_shop_init_member .. Lb_shop_init_member (lobby.bin 0x005AE8D0-0x005AEA34): the matching functions of lbshop2_nm.c. */
#pragma readonly_strings on
#include "lbshop2_proto.h"

void Lb_shop_init_member()
{
    lbShop.x1B = 0;
    Lb_shop_tag_init();
    switch (lb_sys.x68) {
    case 0xA:
        lbShop.tbl = material_shop_tbl[*(int *)((u8 *)cw + 0xBF3C) & 3];
        break;
    case 9:
        lbShop.tbl = tool_shop_tbl[*(int *)((u8 *)cw + 0xBF3C) & 3];
        break;
    case 0xC:
        lbShop.tbl = foods_shop_tbl[*(int *)((u8 *)cw + 0xBF3C) & 3];
        break;
    case 3:
        if (*(u8 *)0x3F3404 == 0x57) {
            lbShop.tbl = goods_shop_local;
        } else {
            lbShop.tbl = goods_shop_tbl;
        }
        break;
    case 0x22:
        lbShop.tbl = goods_shop_local2;
        break;
    case 0xD:
        break;
    }
}
