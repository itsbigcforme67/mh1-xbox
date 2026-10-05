/* Player code (SLPM_654.95 0x0014A610-0x0014A6A8): egg_set (egg carrying: item bits in work56B) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void egg_set(PLW *pl) {
    s32 id;

    if ((Pl_master_ck(pl) == 1) && (act_ck(pl, 5, 0xA) == 0)) {
        id = Pl_hold_item_ck(pl) & 0xFFFF;
        if (id != 0xFFFF) {
            pl->work56B = ((Check_hold_item(id) & 0xFF) - 1) * 0x10;
        } else {
            pl->work56B = 0;
        }
    }
    pl->work56B = (pl->work56B & 0xF0) | 4;
}
