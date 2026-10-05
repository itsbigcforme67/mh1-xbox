/* Player code (SLPM_654.95 0x00152DE0-0x00152FE4): item pouch search/erase/supply */
#include "pl.h"
#include "game.h"
#include "plf.h"
s16 Pl_item_num_ck(PLW *, int);

long Pl_item_search_space(PLW *pl) {
    s16 i;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id == 0) {
            return 1;
        }
    }
    return 0;
}

int Pl_item_erase(PLW *pl, s16 slot) {
    u16 id;
    if (slot >= 20) {
        return 2;
    }
    id = pl->item[slot].id;
    pl->item[slot].num = 0;
    if (pl->item[slot].id == 0) {
        return 0;
    }
    pl->item[slot].id = 0;
    if (Item_data[id][0] == 4) {
        if ((u8)Pl_shell_set(pl, pl->work88E, 1) == 0xFF) {
            pl->work88E = 0xFF;
            pl->work8BC = 0;
            pl->work01D = 0;
            pl->work01C = 0;
        } else if (slot == pl->work88E) {
            pl->work8BC = 0;
            pl->work01D = 0;
            pl->work01C = 0;
        }
    }
    return 1;
}

s32 Pl_item_supply(PLW *pl, int flag, int id, s16 max) {
    s16 have = Pl_item_num_ck(pl, id);
    s16 m = max;
    if (have < m) {
        Pl_item_stack(pl, id, (s16)(m - have));
        return 1;
    }
    if ((u16)flag == 1) {
        Pl_item_stack(pl, id, (s16)(m - have));
        return 2;
    }
    return 3;
}
