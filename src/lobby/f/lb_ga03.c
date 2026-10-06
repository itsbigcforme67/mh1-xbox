/* lb_ga03 - receivers 0x005C5AC0-0x005C5B3C: lb_check_chair. Whole file in lb_a.c. */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];
















void lb_check_chair(int a0, u8 *p) {
    if (memcmp(cw + 0x440, cw + 3, 8) == 0) {
        if (lb_sys.chair_mask & (1 << (*p & 0xFF))) {
            Lb_send_chair_status(a0, 0, *p);
            return;
        }
        Lb_send_chair_status(a0, 1, *p);
    }
}
