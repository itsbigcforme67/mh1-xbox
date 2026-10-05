/* Player code (SLPM_654.95 0x0014DC90-0x0014DEA0): Oki/Taru/Ana item set handlers */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void Oki_item_set(PLW *pl) {
    switch (pl->work88A) {
    case 0x1F:
        func_628FB0(pl, 4, 0);
        break;
    case 0x20:
        func_633B50(pl->pos, 0, pl->stg, pl);
        break;
    case 0x12:
        func_5496E0(pl, 3, 0);
        break;
    case 0x16:
        func_5496E0(pl, 3, 1);
        break;
    case 0x18:
        func_5496E0(pl, 3, 2);
        break;
    case 0x17:
        func_5496E0(pl, 3, 3);
        break;
    }
    Pl_item_cnt_up(pl);
    if (Game_clear_ck(1) == 0) {
        Pl_item_stack(pl, pl->work88A, -1);
    }
}

void Taru_item_set(PLW *pl) {
    switch (pl->work88A) {
    default:
        break;
    case 0x20:
        func_633B50(pl->pos, 0, pl->stg, pl);
        break;
    }
    Pl_item_cnt_up(pl);
    if (Game_clear_ck(1) == 0) {
        Pl_item_stack(pl, pl->work88A, -1);
    }
}

void Ana_item_set(PLW *pl) {
    if (pl->work88A == 0x1E) {
        func_634460(pl, 0);
    } else {
        func_634460(pl, 1);
    }
    Pl_item_cnt_up(pl);
    if (Game_clear_ck(1) == 0) {
        Pl_item_stack(pl, pl->work88A, -1);
    }
}
