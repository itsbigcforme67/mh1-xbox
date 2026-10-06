/* lb_bz76 - lobby UI/client 0x005B2CB0-0x005B2CCC: lm_place_mv (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

int lm_place_mv(a)
int a;
{
    if (!((u16)a & 0x40)) return a;
    return 0x40;
}
