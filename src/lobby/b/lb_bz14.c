/* lb_bz14 - lobby UI/client 0x005B3840-0x005B3894: lm_logout_i (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

s32 lm_logout_i(void) {
    if (F(u8, (u8 *)cw, 0x35D3) == 1) {
        Lb_put_set01(1);
        return 1;
    }
    SetDialogData(0x27, 2);
    SetDialogYesNo(1);
    return 0;
}
