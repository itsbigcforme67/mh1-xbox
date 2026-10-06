/* lb_by83 - agent B promoted near-match 0x0053CAB0-0x0053CBEC: lb_armor_itemBuy (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern char User_data[];
extern int lb_armor_sel2Prog();
extern void Lb_put_shopYesNo();
extern char shop_armor_help[];

s32 lb_armor_itemBuy(void) {
    s32 id;
    s32 qty;
    s32 *e;

    if ((Warehouse_search_space(&User_data) & 0xFF) == 0xFF) {
        return 0;
    }
    lbShop.x6E = 0;
    lbShop.x84 = 2;
    id = lbShop.tbl[lbShop.cur * 2];
    qty = lbShop.tbl[lbShop.cur * 2 + 1];
    Lb_make_mySrcEquip((s16)id);
    *(s16 *)&lbShop.x5A[2] = qty;
    lbShop.x5A[0] = 1;
    lbShop.x5A[1] = id;
    *(s16 *)&lbShop.x5A[4] = 0;
    lbShop.f30 = 0;
    lbShop.x18 = 0;
    if ((id != 7) && (id != 6) && (Equip_ok_ck(&User_data, lbShop.x5A) == 0)) {
        lbShop.f30 = lb_armor_sel2Prog;
        lbShop.x18 = 1;
        lbShop.f40 = Lb_put_shopYesNo;
        lbShop.help = ((s32 *)&shop_armor_help)[5];
    }
    cnWrap_SoundRequest(0);
    return 1;
}
