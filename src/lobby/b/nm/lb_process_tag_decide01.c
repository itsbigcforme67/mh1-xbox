#include "lobby_s.h"
extern s32 armorIndex;
extern char shop_process2_help[];
extern char shop_process2_help[];
void lb_process_tag_decide01(void) {
    int var_at;
    s32 var_v0;
    int var_a0;

    memset(&shopList, 0, 0x5000);
    if (lbShop.mode == 0) {
        lbShop.help = *(s32 *)((int)&shop_process2_help + (lbShop.x1A * 4));
        lb_process_set_weaponList();
        var_a0 =  ( *(u8 *)0x3C738D << 0x30) >> 0x30;
    } else {
        lbShop.help = F(s32, &shop_process2_help, 8);
        var_a0 = (s16)lb_process_set_armorList();
    }
    if ((lbShop.mode == 0) && (lbShop.x1A == 1)) {
        var_v0 = armorIndex;
        var_at = (int)&lbShop + 0x70;
    } else {
        if (armorIndex >= lbShop.count) {
            armorIndex = (lbShop.count - 1);
        }
        var_at = (int)&lbShop + 0x70;
        var_v0 = armorIndex % 7;
    }
    (*(s32 *)var_at) = var_v0;
    lbShop.f40 = (void (*)())0;
    lbShop.x1C = 0;
    if (lbShop.x6C >= lbShop.x6D) {
        lbShop.x6C = (s8) (lbShop.x6D - 1);
    }
    Lb_make_mySrcEquip(var_a0);
}
