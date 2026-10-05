/* lb_gi01 - near-match fixes 0x005D0030-0x005D0098: lb_pl_horm_sub. Whole file in lb_i.c. */
#include "lobby_f.h"
extern u8 D_3E4ECC[];






void lb_pl_horm_sub(PLW *pl) {
    u16 a;
    int t;
    pl->work81C = 0;
    a = *(u16 *)&pl->work81A;
    t = a + 0x400;
    if (a != 0) {
        if (t < 0x801 && t >= 0) {
            pl->work81A = 0;
            return;
        }
        if ((s16)a >= 0) {
            pl->work81A = *(u16 *)((u8 *)pl + 0x81A) - 0x400;
            return;
        }
        pl->work81A = *(u16 *)((u8 *)pl + 0x81A) + 0x400;
    }
}
