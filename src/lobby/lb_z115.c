/* lb_z115 - auto-drafted 0x005FF830-0x005FF8A0: tagAct_301 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

s32 tagAct_301(void) {
    s32 temp_a0;
    int temp_v1;

    temp_v1 = bsw;
    if ((F(u8, temp_v1, 0x1120) != 0) && (F(u8, temp_v1, 0xE96B) == 0)) {
        if (F(s8, temp_v1, 0x186) == 0) {
            stockMetaRefresh(F(s32, temp_v1, 0x1124), temp_v1 + 0x1128);
        } else {
            temp_a0 = F(s32, temp_v1, 0x1124);
            if (temp_a0 == 0) {
                stockMetaRefresh(temp_a0, temp_v1 + 0x1128);
            }
        }
    }
    return 0;
}
