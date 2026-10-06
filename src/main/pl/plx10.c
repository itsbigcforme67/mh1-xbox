/* plx10 - SLPM_654.95 0x00152260-0x001523D8: St_pick_ck2 (the player stands on a stage item spot: finds the spot within its radius and
 * height band, returns the item id, 0xFFFE when used up, 0 when none, and sometimes uses one up unless the skill 0x2F applies). */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "plst.h"
ST_ITEM *Stage_item_data_get(u8);
u16 Item_get_ck(int);
f32 flSqrt(f32);

int St_pick_ck2(PLW *pl) {
    ST_ITEM *d = Stage_item_data_get(pl->stg);
    f32 dx;
    f32 dz;
    int r;
    u16 rn;
    if (d == 0) {
        return 0xFFFF;
    }
    while (d->pos[0] != -1.0f) {
        if (!(pl->pos[1] < d->pos[1] - 200.0f) && pl->pos[1] < 100.0f + d->pos[1]) {
            dx = pl->pos[0] - d->pos[0];
            dz = pl->pos[2] - d->pos[2];
            if (flSqrt(dx * dx + dz * dz) <= d->r) {
                if (d->num > 0) {
                    r = Item_get_ck(d->id & 0x7FFF) & 0xFFFF;
                    if (r != 0 && r != 0xFFFF && d->num != 0xFF) {
                        rn = ran_suu(1);
                        if ((rn & 7) != 0 || Pl_Skill_ck(pl, 0x2F) == 1) {
                            d->num--;
                        } else {
                            d->num = 0;
                        }
                    }
                } else {
                    return 0xFFFE;
                }
                return r;
            }
        }
        d++;
    }
    return 0;
}
