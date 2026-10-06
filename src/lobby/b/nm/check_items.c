#include "lobby_a.h"

s32 check_items(int arg0, int arg1) {
    int temp_s1;
    int var_s0;

    var_s0 = 0;
    temp_s1 =  (Ud_item_num_ck(F(u16, arg0, 0)) << 0x30) >> 0x30;
    if (((s8)arg1) == 0) {
        var_s0 =  (Ud_stock_item_num_ck3(F(u16, arg0, 0)) << 0x30) >> 0x30;
    }
    if ((( (temp_s1 << 0x30) >> 0x30) + ( (var_s0 << 0x30) >> 0x30)) >= F(s16, arg0, 2)) {
        return 1;
    }
    return 0;
}
