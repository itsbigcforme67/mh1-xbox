/* lb_i03 - lobby move dispatch 0x005D00A0-0x005D0120: lb_pl_mv000. Whole file in lb_i.c. */
#include "lobby.h"
extern u8 D_3E4ECC[];






void lb_pl_mv000(PLW *pl) {
    if (PLU8(pl, 0x8EC) != 0) {
        Lb_act_set(pl, 0, 0x55);
        return;
    }
    if (pl->work011 == 1 && pl->char0 == 1 && F(s32, pl, 0x194) < 2) {
        Lb_pl_chr_set(pl, 0x3F, 2, 0);
    }
    lb_basic_com_ck(pl);
}
