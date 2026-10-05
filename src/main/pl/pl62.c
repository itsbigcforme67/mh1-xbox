/* Player code (SLPM_654.95 0x00152140-0x00152254): St_pick_ck */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "plst.h"
ST_ITEM *Stage_item_data_get(u8);
ST_UNIQ *Stage_unique_data_get(u8);
u16 *Stage_item_probability_get(int);
u16 Item_get_ck(int);
f32 flSqrt(f32);

int St_pick_ck(PLW *pl, u16 *id, f32 *pos) {
    ST_ITEM *d = Stage_item_data_get(pl->stg);
    f32 dx;
    f32 dz;
    int r;
    if (d == 0) {
        return 0xFFFF;
    }
    while (d->pos[0] != -1.0f) {
        if (!(pl->pos[1] < d->pos[1] - 200.0f) && pl->pos[1] < 100.0f + d->pos[1]) {
            dx = pl->pos[0] - d->pos[0];
            dz = pl->pos[2] - d->pos[2];
            if (flSqrt(dx * dx + dz * dz) <= d->r) {
                r = d->id;
                *id = d->x14;
                pos[0] = d->pos[0];
                pos[1] = d->pos[1];
                pos[2] = d->pos[2];
                return r;
            }
        }
        d++;
    }
    return 0xFFFF;
}
