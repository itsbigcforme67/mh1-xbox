/* lb_z13 - auto-drafted 0x005E6080-0x005E60D0: bs_request_cache_clear (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern char BcRequest_head[];

void bs_request_cache_clear(u8 *arg0) {
    bs_request_queue_free_node(&BcRequest_head, arg0);
    F(s8, arg0, 0x10C) = 0;
    F(s8, arg0, 0x10D) = 0;
    F(s32, arg0, 0x110) = 0;
    F(s32, arg0, 0x114) = 0;
    F(s32, arg0, 0x11C) = 0;
    F(s32, arg0, 0x120) = 0;
    F(s8, arg0, 0x125) = 0;
    F(s8, arg0, 0x124) = 0;
}
