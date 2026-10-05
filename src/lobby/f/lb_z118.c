/* lb_z118 - auto-drafted 0x00605B60-0x00605BA8: get_top_rowspan_no (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s32 bsw;

u8 get_top_rowspan_no(int arg0) {
    u16 var_v0;
    int temp_v0;
    int var_a0;

    var_a0 = arg0;
    var_v0 = F(u16, var_a0, 0x36);
    if (var_v0 != 0) {
        do {
            temp_v0 = bsw + ((var_v0 & 0xFFFF) * 0x5C);
            var_a0 = temp_v0 + 0x24E0;
            var_v0 = F(u16, temp_v0, 0x2516);
        } while (var_v0 != 0);
    }
    return F(u8, var_a0, 0x18);
}
