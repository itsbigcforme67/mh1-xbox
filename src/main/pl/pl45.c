/* Player code (SLPM_654.95 0x0014B2E0-0x0014B3E4): pl_egg, dispatcher of the egg-carrying action states (flag15 0..10 selects pl_eggNN). */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_egg00();
void pl_egg01();
void pl_egg02();
void pl_egg03();
void pl_egg05();
void pl_egg07();

void pl_egg(PLW *pl) {
    pl->flag12 = 0;
    switch (pl->flag15) {
    case 0:
        pl_egg00(pl, 0);
        break;
    case 1:
        pl_egg01(pl, 0);
        break;
    case 2:
        pl_egg02(pl, 0);
        break;
    case 3:
        pl_egg03(pl, 0);
        break;
    case 4:
        pl_egg03(pl, 1);
        break;
    case 5:
        pl_egg05(pl, 0);
        break;
    case 6:
        pl_egg05(pl, 1);
        break;
    case 7:
        pl_egg07(pl, 0);
        break;
    case 8:
        pl_egg07(pl, 1);
        break;
    case 9:
        pl_egg00(pl, 1);
        break;
    case 10:
        pl_egg03(pl, 2);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
