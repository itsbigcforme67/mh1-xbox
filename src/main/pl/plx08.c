/* plx08 - rate_g_calc (SLPM_654.95 0x001513B0-0x00151430): sets the vertical acceleration so the vertical velocity dies away over t/2 frames
   (guess); returns 1 when it is applied at once. Whole file in pl_nm.c. */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
int rate_g_calc(PLW *pl, int t) {
    f32 a;
    f32 v;
    int m = (s16)t;
    t = (s16)(m / 2);
    v = pl->vel[1];
    a = -1.0f * v;
    if (t <= 1 || v < 0.0f) {
        pl->acc[1] = a;
        return 1;
    }
    a /= (f32)t;
    pl->acc[1] = a;
    return 0;
}
