/* lb_z08 - auto-drafted 0x005D9C70-0x005D9CF8: http_test_02 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void http_test_02(u8 *arg0) {
    s32 temp_v0;

    if (F(s8, arg0, 0x35) != 0) {
        F(s8, arg0, 0x3D) = 0x11;
        F(s8, arg0, 0x3C) = 1;
        F(u8, arg0, 0x40) = 0xBU;
        return;
    }
    temp_v0 = sceHTTPCreate();
    F(s32, arg0, 0x6C) = temp_v0;
    if (temp_v0 == 0) {
        F(s8, arg0, 0x3D) = 2;
        F(s8, arg0, 0x3C) = 1;
        F(u8, arg0, 0x40) = 0xBU;
        return;
    }
    F(u8, arg0, 0x40) = (u8) (F(u8, arg0, 0x40) + 1);
    F(s8, arg0, 0x41) = 0;
}
