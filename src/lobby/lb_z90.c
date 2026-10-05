/* lb_z90 - auto-drafted 0x00602900-0x00602948: get_center_data (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

s32 get_center_data(u16 *arg0) {
    s32 temp_v0_2;
    s32 temp_v1_2;
    s32 var_a1;
    u16 temp_v0;
    void *temp_v1;

    temp_v1 = bsw;
    var_a1 = 0;
    if (F(u8, temp_v1, 0x14) != 0) {
        temp_v0 = *arg0;
        temp_v1_2 = F(u16, temp_v1, 0x10) - F(u16, temp_v1, 0x12);
        if (temp_v0 < temp_v1_2) {
            temp_v0_2 = temp_v1_2 - temp_v0;
            var_a1 = temp_v0_2 >> 1;
            if (temp_v0_2 < 0) {
                var_a1 = (temp_v0_2 + 1) >> 1;
            }
        }
    }
    return var_a1 & 0xFFFF;
}
