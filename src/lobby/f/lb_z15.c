/* lb_z15 - auto-drafted 0x005E63C0-0x005E63D4: bs_page_status_flag_get (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

s32 bs_page_status_flag_get(u8 *arg0, s32 arg1) {
    return F(u8, arg0, 1) & (arg1 & 0xFFFF) & 0xFF;
}
