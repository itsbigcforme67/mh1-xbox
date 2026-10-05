/* Player code (SLPM_654.95 0x001523E0-0x00152760): stage unique/item pick-up checks, item index */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "plst.h"
ST_ITEM *Stage_item_data_get(u8);
ST_UNIQ *Stage_unique_data_get(u8);
u16 *Stage_item_probability_get(int);
u16 Item_get_ck(int);
f32 flSqrt(f32);

int St_unique_ck(PLW *pl, f32 *pos, u16 *x14, u8 *x00) {
    ST_UNIQ *d = Stage_unique_data_get(pl->stg);
    f32 dx;
    f32 dz;
    if (d == 0) {
        return 0;
    }
    while (d->pos[0] != -1.0f) {
        if (!(pl->pos[1] < d->pos[1] - 50.0f) && pl->pos[1] < 50.0f + d->pos[1]) {
            dx = pl->pos[0] - d->pos[0];
            dz = pl->pos[2] - d->pos[2];
            if (flSqrt(dx * dx + dz * dz) <= d->r) {
                pos[0] = d->pos[0];
                pos[1] = d->pos[1];
                pos[2] = d->pos[2];
                *x14 = d->x14;
                *x00 = d->x00;
                return d->kind;
            }
        }
        d++;
    }
    return 0;
}

void St_unique_adr_set(PLW *pl) {
    ST_UNIQ *d = Stage_unique_data_get(pl->stg);
    f32 dx;
    f32 dz;
    f32 dist;
    if (d == 0) {
        pl->fish878 = 0;
        return;
    }
    while (d->pos[0] != -1.0f) {
        if (!(pl->pos[1] < d->pos[1] - 50.0f) && pl->pos[1] < 50.0f + d->pos[1]) {
            dx = pl->pos[0] - d->pos[0];
            dz = pl->pos[2] - d->pos[2];
            dist = flSqrt(dx * dx + dz * dz);
            if (d->kind == 2 && pl_flag_ck(pl, 0x80000) != 0) {
                dist -= 30.0f;
            }
            if (dist <= d->r) {
                pl->fish878 = d;
                return;
            }
        }
        d++;
    }
    pl->fish878 = 0;
}

u16 Item_get_ck(int stg) {
    u16 total = 0;
    u16 r;
    u16 sum;
    u16 *p;
    int s = (u16)stg;
    p = Stage_item_probability_get(s);
    if (*p != 0xFFFF) {
        do {
            total += *p;
            p += 2;
        } while (*p != 0xFFFF);
    }
    r = (u16)ran_suu(0) % total;
    sum = 0;
    p = Stage_item_probability_get(s);
    if (*p != 0xFFFF) {
        do {
            sum += *p;
            if (r < sum) {
                return p[1];
            }
            p += 2;
        } while (*p != 0xFFFF);
    }
    return 0xFFFF;
}

void Pl_item_idx_calc(PLW *pl) {
    s16 i;
    pl->work888 = 0;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id != 0 && pl->item[i].num > 0 && Item_data[pl->item[i].id][1] == 1) {
            pl->work888 = (u8)i;
            return;
        }
    }
}
