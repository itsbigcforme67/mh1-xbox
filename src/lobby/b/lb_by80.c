/* lb_by80 - agent B promoted near-match 0x00539220-0x00539364: lb_process_tag_decide01 (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern s32 armorIndex;
extern char shop_process2_help[];

void lb_process_tag_decide01(void) {
    int a0;

    memset(&shopList, 0, 0x5000);
    if (lbShop.mode == 0) {
        lbShop.help = ((s32 *)&shop_process2_help)[lbShop.x1A];
        lb_process_set_weaponList();
        a0 = (s16)*(u8 *)0x3C738D;
    } else {
        lbShop.help = ((s32 *)&shop_process2_help)[2];
        a0 = (s16)lb_process_set_armorList();
    }
    if ((lbShop.mode == 0) && (lbShop.x1A == 1)) {
        lbShop.x70 = armorIndex;
    } else {
        if (armorIndex >= lbShop.count) {
            armorIndex = lbShop.count - 1;
        }
        lbShop.x70 = armorIndex % 7;
    }
    lbShop.f40 = 0;
    lbShop.x1C = 0;
    if (lbShop.x6C >= lbShop.x6D) {
        lbShop.x6C = lbShop.x6D - 1;
    }
    Lb_make_mySrcEquip(a0);
}
