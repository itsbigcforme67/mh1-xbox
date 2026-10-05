/* Player code (SLPM_654.95 0x0014A550-0x0014A604): pl_demo, dispatcher of the demo/cutscene action states (flag15 0..5 selects pl_demo00N). */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_demo000();
void pl_demo001();
void pl_demo002();
void pl_demo003();
void pl_demo004();
void pl_demo005();

void pl_demo(PLW *pl) {
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    switch (pl->flag15) {
    case 0:
        pl_demo000(pl, 0);
        break;
    case 1:
        pl_demo001(pl, 0);
        break;
    case 2:
        pl_demo002(pl, 0);
        break;
    case 3:
        pl_demo003(pl, 0);
        break;
    case 4:
        pl_demo004(pl, 0);
        break;
    case 5:
        pl_demo005(pl, 0);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
