/* lb_z24 - auto-drafted 0x005ED9E0-0x005EDA38: _png_read_chunk (first drafted by tools/lbauto.py). */
#include "lobby.h"

s32 _png_read_chunk(u8 *arg0, s32 arg1) {
    s32 temp_a3;
    s32 var_a1;

    var_a1 = arg1;
    temp_a3 = F(s32, arg0, 0);
    if (temp_a3 == 0) {
        F(s8, arg0, 0xC) = -3;
        return 0;
    }
    if (temp_a3 < var_a1) {
        var_a1 = temp_a3;
    }
    F(s32, arg0, 8) = (F(s32, arg0, 0xA8) + (F(s32, arg0, 4) - temp_a3));
    F(s32, arg0, 0) = (F(s32, arg0, 0) - var_a1);
    return var_a1;
}
