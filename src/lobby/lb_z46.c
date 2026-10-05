/* lb_z46 - auto-drafted 0x005E6C60-0x005E6C6C: bs_route_current_page_status (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s32 BcRoute_cur;

s32 bs_route_current_page_status(void) {
    return BcRoute_cur + 0x108;
}
