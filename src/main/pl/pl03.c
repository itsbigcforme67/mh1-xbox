/* Player code (SLPM_654.95 0x001359C0-0x00135B00): blend_set, blend_calc. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void blend_set(PLW *pl, s16 a, s16 b) {
    if (pl->work8C9 == 0) {
        pl->work720[0] = 0;
        pl->work720[1] = 0;
        return;
    }
    pl->work720[0] = 1;
    pl->work720[1] = 1;
    if (pl->work8C9 > 0) {
        if ((u16)a == 0x581) {
            pl->work724[0] = 0x580;
            pl->work720[0] = 0;
        } else {
            pl->work724[0] = a;
        }
        pl->work724[1] = (u16)a + 100;
        pl->work72C[0] = pl->work8C9;
        pl->work72C[1] = pl->work8C9;
    } else {
        if ((u16)b == 0x582) {
            pl->work724[0] = 0x580;
            pl->work720[0] = 0;
        } else {
            pl->work724[0] = b;
        }
        pl->work724[1] = (u16)b + 100;
        pl->work72C[0] = -pl->work8C9;
        pl->work72C[1] = -pl->work8C9;
    }
    if (pl->work72C[0] > 0x64) pl->work72C[0] = 0x64;
    if (pl->work72C[1] > 0x64) pl->work72C[1] = 0x64;
}

void blend_calc(PLW *pl, int d) {
    pl->work8C9 = pl->work8C9 + d;
    if (pl->work8C9 >= 100) pl->work8C9 = 100;
    if (pl->work8C9 < -99) pl->work8C9 = -100;
}
