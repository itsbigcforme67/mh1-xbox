/* em_cmd_r91 - near-match fixes: em_cmd_pl_ride_ck,area_route_rnd32. Whole file in em_cmd_nm.c. 0x00565EA0-0x00565F20: area_route_rnd32. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *area_route_rnd32(EMW *em, u8 *a) {
    u8 n;
    u16 i;
    int cum;
    u16 rnd;
    int w;

    n = a[2];
    cum = 0;
    i = 0;
    rnd = em->x39A & 0x1F;
    a += 3;
    for (; i < n; i++) {
        w = a[2];
        a += 3;
        if (w == 0) {
            a += 1;
        } else {
            if ((u8)w == 0xFF) {
                break;
            }
            cum = (cum + w) & 0xFF;
            if (rnd < cum) {
                break;
            }
            a += 1;
        }
    }
    return a;
}
