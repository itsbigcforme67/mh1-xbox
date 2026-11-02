/* lb_c09 - lobby small helpers 0x005CFAA0-0x005CFAD8: Lb_chidori_cnt_up. Whole file in lb_c.c. */
#include "lobby_f.h"













#ifdef VERSION_MH2
void Lb_chidori_cnt_up(u8 *pl) {
    if (pl[0x844] == 0) {
        pl[0x867]++;
        if (pl[0x867] >= 0xA && Online_ck() == 1) {
            pl[0x844] = 1;
        }
    }
}
#else
void Lb_chidori_cnt_up(u8 *pl) {
    if (pl[0x8EC] == 0) {
        pl[0x90F]++;
        if (pl[0x90F] >= 0xA) {
            pl[0x8EC] = 1;
        }
    }
}
#endif
