/* lb_z116 - auto-drafted 0x006018C0-0x006018F8: set_MAX_Y_SIZE (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;

void set_MAX_Y_SIZE(int arg0) {
    s32 temp_a0;
    int temp_a2;

    temp_a2 = bsw;
    if (F(s8, temp_a2, 0x186) == 0) {
        temp_a0 = F(u16, arg0, 0xA) + F(u16, arg0, 6);
        if (F(u16, temp_a2, 0x184) < temp_a0) {
            F(u16, temp_a2, 0x184) = (u16) temp_a0;
        }
    }
}
