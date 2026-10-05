/* Player code (SLPM_654.95 0x0013A5D0-0x0013A620): scope_add, clamp the pachinger/scope yaw offset (x8EE) to +-0x2000. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void scope_add(PLW *pl, int arg1) {
    pl->x8EE = pl->x8EE + arg1;
    if ((s16)arg1 >= 0) {
        if (pl->x8EE >= 0x2000) {
            pl->x8EE = 0x2000;
        }
    } else if (pl->x8EE < -0x1FFF) {
        pl->x8EE = -0x2000;
    }
}
