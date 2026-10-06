#include "lobby_s.h"
extern s8 armor_shop_r;
extern char buki_sei_tbl[];
extern char bou_sei_tbl[];
extern char User_data[];
extern char User_data[];
s32 shop_armor2_question(void) {
    s32 temp_s0;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 var_a2;
    u16 var_a0;
    u16 var_a1;
    u8 temp_a3;
    int temp_a1;
    int temp_a1_2;
    int temp_v1;

    var_a2 = lbShop.cur;
    temp_a1 = (int)lbShop.tbl + (var_a2 * 8);
    if (lbShop.mode == 0) {
        if (lbShop.x1A == 1) {
            var_a0 = F(u16, temp_a1, 0);
            var_a1 = F(u16, temp_a1, 4);
        } else {
            var_a2 = *(s32 *)((int)&shopList + 0x26 + (var_a2 * 0x28));
            temp_a1_2 = (int)&buki_sei_tbl + (var_a2 * 0x18);
            var_a0 = (u16) F(u8, temp_a1_2, 0);
            var_a1 = F(u16, temp_a1_2, 2);
            if (var_a1 == 0x3E7) {
                var_a0 = (u16) F(u8, &lbShop, 0x5B);
                var_a1 = F(u16, &lbShop, 0x5C);
            }
        }
    } else {
        temp_v1 = (int)&bou_sei_tbl + (*(s32 *)((int)&shopList + 0x26 + (var_a2 * 0x28)) * 0x18);
        var_a0 = (u16) F(u8, temp_v1, 0);
        var_a1 = F(u16, temp_v1, 2);
    }
    temp_v1_2 = lbShop.key & 0xFFFF;
    if (armor_shop_r == 0) {
        if (temp_v1_2 & 0x20) {
            temp_s0 = shop_armor2_stack(var_a0 & 0xFFFF, var_a1 & 0xFFFF, (u16) var_a2) & 0xFF;
            if (lbShop.x78 == 0) {
                cnWrap_SoundRequest(0x10);
                cnWrap_SoundRequest(0);
                Warehouse_equip(&User_data, temp_s0);
                armor_shop_r = (s8) (armor_shop_r + 1);
                goto block_30;
            }
            cnWrap_SoundRequest(3);
            Lb_put_set01(0xC);
            return 3;
        }
        if (temp_v1_2 & 0x40) {
            if (lbShop.x78 != 1) {
                cnWrap_SoundRequest(3, var_a1, (u16) var_a2);
                lbShop.x78 = 1;
                goto block_30;
            }
            shop_armor2_stack(var_a0 & 0xFFFF, var_a1 & 0xFFFF, (u16) var_a2);
            cnWrap_SoundRequest(3);
            Lb_put_set01(0xC);
            return 3;
        }
        if (temp_v1_2 & 0x800) {
            if (lbShop.x78 != 0) {
                lbShop.x78 = 0;
                cnWrap_SoundRequest(1, var_a1, (u16) var_a2);
            }
        } else if ((temp_v1_2 & 0x400) && (lbShop.x78 != 1)) {
            lbShop.x78 = 1;
            cnWrap_SoundRequest(1, var_a1, (u16) var_a2);
        }
block_30:
        return 2;
    }
    temp_v1_3 = var_a0 & 0xFFFF;
    if ((temp_v1_3 != 7) && (temp_v1_3 != 6)) {
        armor_set_myArmor((u8) var_a0, var_a1, (u16) var_a2);
    } else if (*(u8 *)0x3C738D != temp_v1_3) {
        armor_set_myArmor((u8) var_a0, var_a1, (u16) var_a2);
    } else {
        Set_equip_idx(&User_data, var_a1, (u16) var_a2);
    }
    F(s8, &lb_sys, 0x78) = 1;
    Set_userdata((int)&player_work + (game_w.master * 0xA00));
    Lb_set_mini_data((s32)cw + (game_w.master * 0x2FC) + 0x1346);
    temp_a3 = game_w.master;
    memcpy((int)&lbCommer + (temp_a3 * 0x5C) + 0x1C, (s32)cw + (temp_a3 * 0x2FC) + 0x1346, 0x40, temp_a3);
    return 0;
}
