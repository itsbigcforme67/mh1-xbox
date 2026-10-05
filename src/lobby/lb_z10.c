/* lb_z10 - auto-drafted 0x005DBA60-0x005DBA7C: http_test_19, http_test_20 (first drafted by tools/lbauto.py). */
#include "lobby.h"

void http_test_19(u8 *arg0) {
    F(u8, arg0, 0x40) = (u8) (F(u8, arg0, 0x40) + 1);
}

void http_test_20(u8 *arg0) {
    F(s8, arg0, 0x68) = 2;
}
