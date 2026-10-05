/* lb_bz79 - lobby UI/client 0x005B37B0-0x005B37FC: lm_introduction_mv (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

int lm_introduction_mv() {
    switch (plaza_setMyComment()) {
    case 3:
        ClearMessageHaltFlag();
        return 0x40;
    default:
        return 0;
    }
}
