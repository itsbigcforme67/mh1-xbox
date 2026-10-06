#include "lobby_a.h"
extern s8 armor_shop_r;
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
s32 shop_armor_question(void) {
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_s0;
    s32 temp_v1_2;
    u8 temp_a3;
    int temp_v1;

    temp_v1 = F(s32, &lbShop, 0x64) + (F(s32, &lbShop, 0x74) * 8);
    temp_a2 = F(s32, temp_v1, 4);
    temp_a1 = F(s32, temp_v1, 0);
    if (armor_shop_r == 0) {
        temp_v1_2 = F(u16, &lbShop, 0x8C) & 0xFFFF;
        if (temp_v1_2 & 0x20) {
            temp_s0 = Warehouse_equip_stack(&User_data, temp_a1 & 0xFF, temp_a2 & 0xFFFF, 0) & 0xFF;
            if (F(s8, &lbShop, 0x78) == 0) {
                cnWrap_SoundRequest(0x10);
                cnWrap_SoundRequest(0);
                Warehouse_equip(&User_data);
                armor_shop_r = (s8) (armor_shop_r + 1);
                goto block_24;
            }
            cnWrap_SoundRequest(3);
            lb_armor_tag_decide01();
            Lb_put_set01(0xC);
            return 3;
        }
        if (temp_v1_2 & 0x40) {
            if (F(s8, &lbShop, 0x78) != 1) {
                cnWrap_SoundRequest(3);
                F(s8, &lbShop, 0x78) = 1;
                goto block_24;
            }
            Warehouse_equip_stack(&User_data, temp_a1 & 0xFF, temp_a2 & 0xFFFF, 0);
            cnWrap_SoundRequest(3);
            lb_armor_tag_decide01();
            Lb_put_set01(0xC);
            return 3;
        }
        if (temp_v1_2 & 0x800) {
            if (F(s8, &lbShop, 0x78) != 0) {
                F(s8, &lbShop, 0x78) = 0;
                cnWrap_SoundRequest(1);
            }
        } else if ((temp_v1_2 & 0x400) && (F(s8, &lbShop, 0x78) != 1)) {
            F(s8, &lbShop, 0x78) = 1;
            cnWrap_SoundRequest(1);
        }
block_24:
        return 2;
    }
    if ((temp_a1 != 7) && (temp_a1 != 6)) {
        armor_set_myArmor(F(u16, &lbShop, 0x8C));
    } else if (*(u8 *)0x3C738D != temp_a1) {
        armor_set_myArmor(F(u16, &lbShop, 0x8C));
    } else {
        Set_equip_idx(&User_data);
    }
    F(s8, &lb_sys, 0x78) = 1;
    Set_userdata((int)&player_work + (game_w.master * 0xA00));
    Lb_set_mini_data((s32)cw + (game_w.master * 0x2FC) + 0x1346);
    temp_a3 = game_w.master;
    memcpy((int)&lbCommer + (temp_a3 * 0x5C) + 0x1C, (s32)cw + (temp_a3 * 0x2FC) + 0x1346, 0x40);
    lb_armor_tag_decide01();
    return 0;
}
