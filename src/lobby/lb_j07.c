/* lb_j07 - lobby move handlers 0x005D1C40-0x005D1D98: lb_pl_mv082. Whole file in lb_j.c. */
#include "lobby.h"






void pl_sleeping();





















void lb_pl_mv082(PLW *pl, s8 m) {
    int a;
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        PLU8(pl, 0x4D4) = 0;
        pl->x05 = pl->x05 + 1;
        pl->work08 = 0;
        if (m == 0) {
            Lb_pl_chr_set(pl, 0x287, 0, 0);
            Lb_put_hint(0, 0x63);
            return;
        }
        Lb_pl_chr_set(pl, 0x288, 0, 0);
        return;
    case 1:
        if (m == 0) {
            pl->work08 = pl->work08 + 1;
            if (pl->work08 == 0xA0) {
                adx_se_set(pl, 7);
            }
        }
        if (F(s32, pl, 0x194) <= 0) {
            PLU8(pl, 0x4D4) = 1;
            pl_flag_clr(pl, 0x20000);
            a = pl->ang[1] + 0x7FFF + 1;
            pl->ang_y = a;
            pl->ang[1] = a & 0xFFFF;
            Lb_pl_to_normal(pl, 0, 0, 0);
            lb_sys.x68 = 0;
        }
        break;
    }
}
