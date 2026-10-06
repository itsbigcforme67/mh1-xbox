/* plx12 - SLPM_654.95 0x0013EA40-0x0013EB0C: pl_mv060 (a short held pose of the hunter: sets the action flags and the motion 8 (12 for the
 * character 0x25), waits 12 frames, then returns to the normal action 0x1D). Field names are guesses. */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"

void pl_mv060(PLW *pl) {
    s16 v;
    u8 s;

    pl->work08++;
    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (pl->char0 == 0x25) {
            v = 0xC;
        } else {
            v = 8;
        }
        Pl_basic_flagset(pl, 0x8001, 0, 0);
        pl_chr_set2(pl, 8, v, 0);
        pl->work08 = 0;
        break;
    case 1:
        if (pl->work08 >= 0xC) {
            Pl_act_set(pl, 0, 0x1D, 0);
        }
        break;
    }
}
