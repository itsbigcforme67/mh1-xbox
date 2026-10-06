#include "lobby_a.h"
extern char User_data[];
s32 Lb_shop_item_checkMax(s32 arg0, int arg1) {
    int var_a2;
    s16 temp_v0;
    s32 var_a3;
    int temp_a0;
    int temp_a1;
    int temp_v1;
    int temp_v1_2;

    temp_v1 = (s16)Ud_item_num_ck3();
    if (F(s8, &lbShop, 0x19) != 0) {
        temp_a0 = (s8)arg1;
        var_a3 = 0;
        var_a2 = (int)&User_data;
loop_16:
        if (F(u16, var_a2, 0x37C) == (arg0 & 0xFFFF)) {
            temp_v0 = F(s16, var_a2, 0x37E);
            if (temp_v0 == 0xFF) {
                if (temp_a0 < 2) {
                    return 1;
                }
                goto block_23;
            }
            if (temp_v0 >= temp_a0) {
                return 1;
            }
            goto block_23;
        }
block_23:
        var_a3 += 1;
        var_a2 += 4;
        if (var_a3 >= 0x14) {
            goto block_24;
        }
        goto loop_16;
    }
    temp_v1_2 =  (temp_v1 << 0x30) >> 0x30;
    if (temp_v1_2 == -1) {
        return 0;
    }
    if (temp_v1_2 == 0xFF) {
        if ((((s8)arg1) == 1) && (((s16)Ud_item_num_ck(arg0)) == 0) && (CheckItemPrice_005AFEE0(arg0, (s8)arg1) == 1) && (((s16)Ud_item_search_space()) == 1)) {
            return 1;
        }
        goto block_24;
    }
    temp_a1 = (s8)arg1;
    if ((temp_v1_2 >= temp_a1) && (CheckItemPrice_005AFEE0(arg0) == 1)) {
        return 1;
    }
block_24:
    return 0;
}
