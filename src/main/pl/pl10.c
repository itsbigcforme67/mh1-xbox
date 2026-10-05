/* Player code (SLPM_654.95 0x00138BE0-0x00138D48): basic_kabe_ck, climbing/wall check
   (wall_act_ck result and three front_land_ck probes pick the wall action). */
#include "pl.h"
#include "game.h"
#include "plf.h"

u8 basic_kabe_ck(PLW *pl) {
    int sp3C;
    u8 r;

    r = 0;
    switch ((s8)wall_act_ck(pl, 0)) {
    case 0:
        break;
    case 1:
        Pl_act_set(pl, 0, 0x2B, 0xC);
        r = 1;
        break;
    }
    if (front_land_ck(55.0f, 60.0f, 120.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x1E, 0xC);
        r = 1;
    } else if (front_land_ck(55.0f, 180.0f, 30.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x1B, 0xC);
        r = 1;
    } else if (front_land_ck(55.0f, 210.0f, 90.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x15, 0xC);
        r = 1;
    }
    return r;
}
