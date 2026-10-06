#include "lobby_f.h"
extern s32 armorIndex;
extern char D_3C7005[];
extern char D_3C7006[];
extern char D_3C7008[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
s32 shop_armor2_stack(s8 arg0, s16 arg1, int arg2) {
    int var_a2;
    s32 temp_a3;
    s32 temp_t0;
    s32 var_s0;
    u16 temp_v0;

    var_a2 = arg2;
    if ((F(s8, &lbShop, 0x19) == 0) && (var_a2 = 1, (F(s8, &lbShop, 0x1A) == 1))) {
        temp_t0 = armorIndex;
        var_s0 = temp_t0 & 0xFF;
        temp_a3 = var_s0 * 6;
        *((u8 *)&D_3C7005 + temp_a3) = arg0;
        if (arg0 == 6) {
            *((u8 *)&D_3C7006 + temp_a3) = arg1;
            *((u8 *)&D_3C7008 + temp_a3) = 0;
            if (var_s0 == *(u8 *)0x3C7416) {
                Set_equip_idx(&User_data, 1);
                Set_userdata((u8 *)&player_work + (game_w.master * 0xA00));
            }
        } else {
            temp_v0 = F(u16, ((F(s32, &lbShop, 0x74) * 0x28) + F(s32, &lbShop, 0x50)), 0x26);
            switch (temp_v0) {
            case 0:
                Gun_level_up(&User_data, (s16)temp_t0, 1, temp_a3);
                break;
            case 1:
                Gun_Silencer_set(&User_data, (s16)temp_t0, 0);
                break;
            case 2:
                Gun_Silencer_set(&User_data, (s16)temp_t0, 1);
                break;
            case 3:
                Gun_barrel_set(&User_data, (s16)temp_t0, 0);
                break;
            case 4:
                Gun_barrel_set(&User_data, (s16)temp_t0, 1);
                break;
            case 5:
                Gun_Scope_set(&User_data, (s16)temp_t0, 0);
                break;
            case 6:
                Gun_Scope_set(&User_data, (s16)temp_t0, 1);
                break;
            }
            if (Now_equip_ck(&User_data, armorIndex) == 1) {
                Set_equip_idx(&User_data);
                Set_userdata((u8 *)&player_work + (game_w.master * 0xA00));
            }
        }
    } else {
        var_s0 = item_to_stack(var_a2) & 0xFF;
    }
    return var_s0;
}
