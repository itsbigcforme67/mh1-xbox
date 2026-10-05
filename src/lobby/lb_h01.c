/* lb_h01 - lobby flags/adjust/stick 0x005CE400-0x005CE56C: Lb_stick_pow_get. Whole file in lb_h.c. */
#include "lobby.h"








u8 Lb_stick_pow_get(PLW *pl) {
    u8 r = 0;
    u8 c;
    if (lb_sys.x6C == 1 && pl->id == game_w.master) {
        return 0;
    }
    PLU8(pl, 0x8C8) = 0;
    if (game_w.master == pl->id) {
        u16 p = pl->sw.pow[0];
        if (p >= 0x78) {
            r = 3;
        } else if (p >= 0x55) {
            r = 1;
        } else if (p >= 0x28) {
            r = 1;
        }
    } else {
        c = pl->flag15;
        if (c == 0 || c == 0x55) {
            r = 0;
        } else if (c == 2 || c == 0x5B) {
            r = 1;
        } else {
            r = 5;
        }
    }
    if (PLU8(pl, 0x388) == 0 && pl->flag12 == 0 && (pl->sw.now & 0x10) && (r & 0xFF) && ((s16)Lb_act_ck(pl, 0, 0x24) == 0 || pl->work760 == 0)) {
        return 5;
    }
    return r;
}
