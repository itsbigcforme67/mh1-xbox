#include "lobby_s.h"
extern s8 armor_shop_r;
extern char shopList2[];
extern char shop_armor_put_shopHelp[];
extern char shop_process_after[];
extern char User_data[];
void lb_process_decide(void) {
    s16 sp3A;
    s8 sp39;
    int sp38;
    s32 temp_s0;
    s32 temp_s1;
    int temp_v1;

    temp_v1 = (int)lbShop.tbl + (lbShop.cur * 8);
    temp_s0 = F(s32, temp_v1, 4);
    temp_s1 = F(s32, temp_v1, 0);
    if ((lbShop.mode == 0) && (lbShop.x1A == 1)) {
        Gold_add(-*(s32 *)((int)&shopList2 + (lbShop.cur * 0x28)), lbShop.cur);
    } else {
        Gold_add(-(lbShop.qty * shopList[lbShop.cur].price), lbShop.cur);
    }
    lb_process_use_item(lbShop.cur);
    lbShop.f40 = (void (*)())shop_armor_put_shopHelp;
    cnWrap_SoundRequest(8);
    lbShop.x8F = 1;
    lbShop.f38 = (int (*)())shop_process_after;
    armor_shop_r = 0;
    if ((temp_s1 != 7) && (temp_s1 != 6)) {
        sp39 = (s8) temp_s1;
        sp3A = (s16) temp_s0;
        if (Equip_ok_ck(&User_data, &sp38) == 0) {
            lbShop.f40 = (void (*)())0;
        }
    }
}
