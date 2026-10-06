#include "lobby_f.h"
extern char D_3F3728[];
extern char D_3F3714[];
s32 Lb_shop_sw(int arg0) {
    s32 temp_a0;
    s32 var_s0;

    var_s0 = 0;
    if (F(u8, &lb_sys, 0x8E) < 3) {
        return 0;
    }
    if (SoftKeyboard_alive_check() == 0) {
        temp_a0 = ((s8)arg0) * 0x22;
        var_s0 = ((*((u8 *)&D_3F3728 + temp_a0) & 0x3C00) | *((u8 *)&D_3F3714 + temp_a0)) & 0xFFFF;
    }
    if (var_s0 & 0xFFFF) {
        F(u8, &lb_sys, 0x8E) = 0U;
    }
    return var_s0;
}
