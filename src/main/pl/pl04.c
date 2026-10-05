/* Player code (SLPM_654.95 0x00134950-0x00134A34): Pl_item_charge. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void Pl_item_charge(PLW *pl) {
    s16 i;
    s16 *p;
    s16 n;
    u8 c = pl->work8BF;
    if (c == 0) return;
    pl->work8BF = c - 1;
    for (i = 0; i < 20; i++) {
        pl->item[i].id = 0;
        pl->item[i].num = 0;
    }
    p = pl_supp_tbl[pl->work8BF + pl->kind * 6];
    n = *p;
    while (n != -1) {
        u16 a = p[1];
        p += 2;
        Pl_item_supply(pl, 0, a, n);
        n = *p;
    }
}
