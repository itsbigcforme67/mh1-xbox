/* Player code (SLPM_654.95 0x0014B200-0x0014B2DC): pl_egg07: egg-carrying action state */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_egg07(PLW *pl, s32 arg1) {
    u8 s;

    egg_set(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x39, 0, 0);
        } else {
            pl_chr_set2(pl, 0x3A, 0, 0);
        }
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 5, 0, 2);
        }
        break;
    }
}
