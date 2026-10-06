#include "lobby_s.h"
extern char shop_armor_help[];
extern char armor_shop_tbl[];
extern char User_data[];
extern char User_data[];
void lb_armor_tag_decide01(void) {
    s16 sp9A;
    s8 sp99;
    int sp98;
    int var_s5;
    s32 temp_a1;
    s32 temp_s0;
    s32 var_s2;
    s8 var_a0;
    u32 var_s1;
    int temp_s4;
    int var_s3;

    var_s5 = (int)&shopList;
    var_s1 = 0;
    temp_s4 = (int)&player_work + (game_w.master * 0xA00);
    lbShop.help = *(s32 *)((int)&shop_armor_help + (lbShop.x1A * 4));
    memset(&shopList, 0, 0x5000, &player_work);
    lbShop.tbl = (s32 *) *(s32 *)((int)&armor_shop_tbl + (lbShop.x1A * 4));
    var_s3 = (int)lbShop.tbl;
    var_s2 = 0;
    temp_s0 = (Warehouse_search_space(&User_data) & 0xFF) == 0xFF;
loop_1:
    temp_a1 = F(s32, var_s3, 0);
    if ((temp_a1 != 0xFFFF) && (F(s32, var_s3, 4) != 0xFFFF)) {
        sp99 = (s8) temp_a1;
        sp9A = (s16) F(s32, var_s3, 4);
        if ((1 << F(u8, temp_s4, 0x11)) & (Get_equip_bit(&User_data, &sp98) & 0xFF)) {
            strcpy(var_s5 + 4, Get_equip_name((u8) F(s32, var_s3, 0), (u16) F(s32, var_s3, 4)));
            F(u32, var_s5, 0) = Get_equip_price((u8) F(s32, var_s3, 0), (u16) F(s32, var_s3, 4));
            if ((u32) *(u32 *)0x3C6FE0 >= (u32) F(u32, var_s5, 0)) {
                if (temp_s0 == 1) {
                    goto block_7;
                }
                F(s16, var_s5, 0x24) = 0;
            } else {
block_7:
                F(s16, var_s5, 0x24) = 1;
            }
            var_s1 += 1;
            var_s5 += 0x28;
        }
        var_s2 += 1;
        var_s3 += 8;
        if (var_s2 < 0x64) {
            goto loop_1;
        }
    }
    var_a0 = (var_s1 / 7) + (var_s1 >> 0x1F);
    lbShop.count = var_s1;
    if ((var_s1 % 7) != 0) {
        var_a0 += 1;
    }
    lbShop.x6D = var_a0;
    lbShop.x84 = 0;
}
