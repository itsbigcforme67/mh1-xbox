/* lb_h03 - lobby flags/adjust/stick 0x005CE7D0-0x005CE820: Lb_Em_pos_adj. Whole file in lb_h.c. */
#include "lobby_f.h"








void Lb_Em_pos_adj(PLW *pl) {
    u16 t = F(u16, pl, 0x468);
    u8 *p = (u8 *)pl + 0x444;
    if (t != 0) {
        F(s16, p, 0x24) = t - 1;
        if (t > 0) {
            pl->pos[0] = pl->pos[0] + F(f32, p, 0x18);
            pl->pos[1] = pl->pos[1] + F(f32, p, 0x1C);
            pl->pos[2] = pl->pos[2] + F(f32, p, 0x20);
        }
    }
}
