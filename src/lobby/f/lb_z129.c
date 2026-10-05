/* lb_z129 - auto-drafted 0x005FFD20-0x005FFD8C: tagAct_603 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_603(int arg0, int arg1) {
    u8 var_v0;
    int temp_v1;

    font_data_clear();
    tagoutprintf3(arg1);
    temp_v1 = bsw;
    var_v0 = F(u8, (F(s16, temp_v1, 0x124) + temp_v1), 0x168);
    if (var_v0 < 7) {
        var_v0 = (var_v0 + 1) & 0xFF;
    }
    F(s8, temp_v1, 0x2D3) = (s8) ((var_v0 & 0xFF) + 0x30);
    F(s8, bsw, 0x2D4) = 0;
    font_data_set();
    return 0;
}
