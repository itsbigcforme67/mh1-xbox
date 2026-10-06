#include "lobby_s.h"
extern char User_data[];
extern char User_data[];
extern char lb_armor_sel2Prog[];
extern char Lb_put_shopYesNo[];
extern char shop_armor_help[];
s32 lb_armor_itemBuy(void) {
    s32 temp_s0;
    s32 temp_s1;
    int temp_v0;

    if ((Warehouse_search_space(&User_data) & 0xFF) == 0xFF) {
        return 0;
    }
    lbShop.x6E = 0;
    lbShop.x84 = 2;
    temp_v0 = (int)lbShop.tbl + (lbShop.cur * 8);
    temp_s1 = F(s32, temp_v0, 0);
    temp_s0 = F(s32, temp_v0, 4);
    Lb_make_mySrcEquip((s16)temp_s1);
    F(s16, &lbShop, 0x5C) = (s16) temp_s0;
    F(s8, &lbShop, 0x5A) = 1;
    F(s8, &lbShop, 0x5B) = (s8) temp_s1;
    F(s16, &lbShop, 0x5E) = 0;
    lbShop.f30 = (int (*)())0;
    lbShop.x18 = 0;
    if ((temp_s1 != 7) && (temp_s1 != 6) && (Equip_ok_ck(&User_data, (int)&lbShop + 0x5A) == 0)) {
        lbShop.f30 = (int (*)())lb_armor_sel2Prog;
        lbShop.x18 = 1;
        lbShop.f40 = (void (*)())Lb_put_shopYesNo;
        lbShop.help = F(s32, &shop_armor_help, 0x14);
    }
    cnWrap_SoundRequest(0);
    return 1;
}
