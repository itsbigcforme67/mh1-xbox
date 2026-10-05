/* lb_h05 - lobby flags/adjust/stick 0x005CEEC0-0x005CEF28: Lb_Pl_stg_ck. Whole file in lb_h.c. */
#include "lobby.h"








int Lb_Pl_stg_ck(PLW *pl) {
    u8 s = pl->stg;
    if (s != game_w.stage) {
        return 0;
    }
    if (pl->id != game_w.master) {
        if (s == 0x4C || s == 0x4D) {
            return 1;
        }
        return 0;
    }
    return 1;
}
