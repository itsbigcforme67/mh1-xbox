/* Player code (SLPM_654.95 0x0014EFA0-0x0014F028): pl_flag_set / pl_flag_clr */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_flag_set(PLW *pl, int f) {
    if (!(f & 0x80000000)) {
        pl->act_flag = pl->act_flag | f;
    } else {
        pl->work394 = pl->work394 | (f & 0x7FFFFFFF);
    }
}

void pl_flag_clr(PLW *pl, int f) {
    if (!(f & 0x80000000)) {
        pl->act_flag = pl->act_flag & ~f;
    } else {
        pl->work394 = pl->work394 & ~(f & 0x7FFFFFFF);
    }
}
