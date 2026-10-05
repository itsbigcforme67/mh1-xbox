/* lb_c09 - lobby small helpers 0x005CFAA0-0x005CFAD8: Lb_chidori_cnt_up. Whole file in lb_c.c. */
#include "lobby.h"













void Lb_chidori_cnt_up(u8 *pl) {
    if (pl[0x8EC] == 0) {
        pl[0x90F]++;
        if (pl[0x90F] >= 0xA) {
            pl[0x8EC] = 1;
        }
    }
}
