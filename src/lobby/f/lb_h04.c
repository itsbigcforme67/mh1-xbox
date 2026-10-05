/* lb_h04 - lobby flags/adjust/stick 0x005CE9F0-0x005CEA3C: lb_ck_unique_act. Whole file in lb_h.c. */
#include "lobby_f.h"








int lb_ck_unique_act(int a0, u8 *p) {
    switch (F(u16, p, 2)) {
    case 7:
        return Lb_ck_target(a0, p + 4, 0x5A);
    }
    return 1;
}
