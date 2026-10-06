#include "lobby_s.h"
extern s16 armorIndex;
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
extern char User_data[];
s32 item_to_stack(s32 arg0, s32 arg1) {
    s32 var_s0;
    u16 temp_v0;
    if (arg1 != 0x3E7) {
        if ((lbShop.x1A == 1) && (arg0 == 7)) {
            temp_v0 = F(u16, ((lbShop.cur * 0x28) + (int)lbShop.list), 0x26);
            switch (temp_v0) {
            case 0:
                Gun_level_up(&User_data, armorIndex, 1);
                break;
            case 1:
                Gun_Silencer_set(&User_data, armorIndex, 0);
                break;
            case 2:
                Gun_Silencer_set(&User_data, armorIndex, 1);
                break;
            case 3:
                Gun_barrel_set(&User_data, armorIndex, 0);
                break;
            case 4:
                Gun_barrel_set(&User_data, armorIndex, 1);
                break;
            case 5:
                Gun_Scope_set(&User_data, armorIndex, 0);
                break;
            case 6:
                Gun_Scope_set(&User_data, armorIndex, 1);
                break;
            }
        } else {
            var_s0 = Warehouse_equip_stack(&User_data, arg0 & 0xFF, arg1 & 0xFFFF, 0) & 0xFF;
        }
    } else {
        var_s0 = Warehouse_equip_stack(&User_data, F(u8, &lbShop, 0x5B), F(u16, &lbShop, 0x5C), 0) & 0xFF;
    }
    return var_s0;
}
