/* lb_z09 - auto-drafted 0x005DB980-0x005DB9BC: http_test_15, http_test_16, http_test_17 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void http_test_15(u8 *arg0) {
    F(s8, arg0, 0x68) = 2;
}

void http_test_16(u8 *arg0) {
    F(s8, arg0, 0x3C) = 1;
    F(s8, arg0, 0x40) = 0x11;
}

void http_test_17(u8 *arg0) {
    F(s8, arg0, 0x68) = 2;
}
