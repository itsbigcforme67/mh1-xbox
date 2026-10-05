/* lb_s24 - small browser/http fixes 0x005DB270-0x005DB280: http_test_10 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void http_test_10(int arg0, int arg1) {
    F(s8, arg0, 0x68) = 2;
    CpInetInterfaceProblemEnable(1);
}
