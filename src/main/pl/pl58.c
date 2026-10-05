/* Player code (SLPM_654.95 0x00152050-0x0015213C): World_calc / World_calc2 */
#include "pl.h"
#include "game.h"
#include "plf.h"
f32 *Stage_data_get(int stg);
void flmatGetTrans(f32 *, u8 *);

void World_calc(PLW *pl) {
    f32 *sd = Stage_data_get(pl->stg);
    pl->work754 = pl->pos[0] + sd[0];
    pl->work758 = pl->pos[1] + (sd[6] + sd[7]) / 2.0f;
    pl->work75C = pl->pos[2] + sd[1];
}

void World_calc2(int stg, f32 *pos, f32 *out) {
    f32 *sd = Stage_data_get((u16)stg);
    out[0] = pos[0] + sd[0];
    out[1] = pos[1] + (sd[6] + sd[7]) / 2.0f;
    out[2] = pos[2] + sd[1];
}
