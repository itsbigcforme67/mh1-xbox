/* lb_h02 - lobby flags/adjust/stick 0x005CE720-0x005CE774: Lb_Em_adj_calc. Whole file in lb_h.c. */
#include "lobby_f.h"








void Lb_Em_adj_calc(PLW *pl, s16 n) {
    F(s16, pl, 0x468) = n;
    F(f32, pl, 0x45C) = (F(f32, pl, 0x934) - pl->pos[0]) / (f32)n;
    F(f32, pl, 0x460) = (F(f32, pl, 0x938) - pl->pos[1]) / (f32)n;
    F(f32, pl, 0x464) = (F(f32, pl, 0x93C) - pl->pos[2]) / (f32)n;
}
