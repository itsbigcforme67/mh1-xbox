/* Player code (SLPM_654.95 0x00136FB0-0x001371B0): em_ninshiki_ck2, pl_ride_ck, ex_kabe_ck. */
#include "pl.h"
#include "game.h"
#include "plf.h"

int em_ninshiki_ck2(PLW *pl) {
    u8 a = pl->work81E;
    if (a != 0) {
        if (a != pl->work81F && pl->work7EE == 0) {
            pl->work7EE = 1;
            return 1;
        }
        return 0xFF;
    }
    return 0;
}

int pl_ride_ck(PLW *pl) {
    f32 d, lim;
    u8 k;
    d = flvecCalcLength((f32 *)((u8 *)pl + 0x618));
    k = pl->flag14;
    if (k == 0 && pl->flag15 == 0xC) lim = 10.0f;
    else if (k == 0 && pl->flag15 == 0x16) lim = 4.0f;
    else lim = 7.0f;
    if (!(d < lim)) {
        if (k == 0 && pl->flag15 == 0x16) return 1;
        Pl_act_set(pl, 0, 0x16, 0);
        return 1;
    }
    return 0;
}

int ex_kabe_ck(PLW *pl) {
    u16 a = pl->sw.an_now;
    if (a & 0x2000) {
        if (act_ck(pl, 0, 0x28) == 0) Pl_act_set(pl, 0, 0x28, 0xC);
        return 1;
    }
    if (a & 0x1000) {
        if (act_ck(pl, 0, 0x29) == 0) Pl_act_set(pl, 0, 0x29, 0xC);
        return 1;
    }
    if (pl->sw.trg & 0x40) {
        Pl_act_set(pl, 0, 0x30, 0);
        return 1;
    }
    return 0;
}
