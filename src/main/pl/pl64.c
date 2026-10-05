/* Player code (SLPM_654.95 0x00152B60-0x00152BB8): Pl_item_num_ck */
#include "pl.h"
#include "game.h"
#include "plf.h"

s16 Pl_item_num_ck(PLW *pl, int id) {
    s16 i;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id == (u16)id) {
            return pl->item[i].num;
        }
    }
    return 0;
}
