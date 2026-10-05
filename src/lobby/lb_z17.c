/* lb_z17 - auto-drafted 0x005E6D00-0x005E6D54: BsRouteReloadCheck (first drafted by tools/lbauto.py). */
#include "lobby.h"

s32 BsRouteReloadCheck(void) {
    if (bs_page_status_flag_get(bs_route_current_page_status(), 1) != 0) {
        return 0;
    }
    bs_page_status_flag_get(bs_route_current_page_status(), 2);
    return 1;
}
