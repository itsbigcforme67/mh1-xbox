/* G online (PC): gonl_send_pl_status0 = G lobby 0x59A820 (named check_sender0 in G's split after MH1's function at that
 * place, which is other code), HAND-FIXED: a jump to Lb_send_pl_status with $a1 = 0 (set in the jump's delay slot, which
 * m2c drops); lobby_func_0059A830 next to it passes 1. */
#include "../gonl.h"

s32 Lb_send_pl_status();

s32 gonl_send_pl_status0(s32 a0) {
    return Lb_send_pl_status(a0, 0);
}
