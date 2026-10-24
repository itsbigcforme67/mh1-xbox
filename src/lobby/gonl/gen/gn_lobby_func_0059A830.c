/* G online (PC): lobby_func_0059A830 (G lobby 0x59A830), HAND-FIXED: a jump to Lb_send_pl_status with $a1 = 1 (set in
 * the jump's delay slot, which m2c drops). */
#include "../gonl.h"

s32 Lb_send_pl_status();

s32 lobby_func_0059A830(s32 a0) {
    return Lb_send_pl_status(a0, 1);
}
