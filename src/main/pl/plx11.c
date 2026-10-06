/* plx11 - SLPM_654.95 0x0014F870-0x0014F9A8: pl_light_ck (sets the "near a big monster" light flag work613 of a hunter: set when a live monster
 * of size class >= 2 (EMW+0x612) is within 300 (class 2) or 500 units horizontally and not far above). */
#include "pl.h"
#include "em.h"
#include "game.h"
#include "plf.h"
#define PU8(p, o) (*(u8 *)((u8 *)(p) + (o)))
extern EMW em_work[];
f32 flSqrt(f32);

void pl_light_ck(PLW *pl) {
    int i;
    EMW *e;
    f32 d;
    f32 dx;
    f32 dz;
    f32 lim;
    f32 hlim;
    if (pl->flag604 != 0 || !(pl->pos[1] - pl->x5AC <= 100.0f)) {
        pl->work613 = 0;
        return;
    }
    e = em_work;
    for (i = 0; i < 20; i++, e++) {
        if (e->be_flag != 0 && e->x01 != 0 && PU8(e, 0x612) >= 2) {
            dx = pl->pos[0] - e->pos[0];
            dz = pl->pos[2] - e->pos[2];
            d = flSqrt(dx * dx + dz * dz);
            if (*(u8 *)((int)e + 0x612) == 2) {
                lim = 300.0f;
                hlim = 300.0f;
            } else {
                lim = 500.0f;
                hlim = 300.0f;
            }
            if (d <= lim && pl->pos[1] - e->pos[2] < hlim) {
                pl->work613 = 1;
                return;
            }
        }
    }
    pl->work613 = 0;
}
