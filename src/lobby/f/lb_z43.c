/* lb_z43 - auto-drafted 0x005E1070-0x005E10A8: stockBgColor (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsSys;

void stockBgColor(s32 arg0) {
    void *temp_a1;

    temp_a1 = bsSys;
    if (F(u8, temp_a1, 0x34) != 0) {
        F(s32, temp_a1, 0x14) = 0xFF000001;
        return;
    }
    F(s32, temp_a1, 0x14) = (arg0 | 0xFF000000);
}
