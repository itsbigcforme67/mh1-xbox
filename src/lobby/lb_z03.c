/* lb_z03 - auto-drafted 0x005D7AC0-0x005D7B04: Lb_get_angle (first drafted by tools/lbauto.py). */
#include "lobby.h"

s32 Lb_get_angle(u8 *arg0, int arg1) {
    return ((((calc_vec_ang2(arg1, arg0 + 0xAC) & 0xFFFF) + 0x4000) & 0xFFFF) - F(s32, arg0, 0xA4)) & 0xFFFF;
}
