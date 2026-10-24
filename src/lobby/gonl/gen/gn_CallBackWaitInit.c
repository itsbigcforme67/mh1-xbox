/* G online (PC): CallBackWaitInit (G lobby 0x579BC0), HAND-FIXED (m2c could not decompile it; MH1's is file-static in
 * lb_cli.c): the callback wait counter (cw+0xFA68, 0xE10 ticks) and its timed-out flag (cw+0xFA65), which
 * Check_CallBackWait counts down. */
#include "../gonl.h"

extern u8 *cw;

void CallBackWaitInit(void) {
    *(s32 *)(cw + 0xFA68) = 0xE10;
    *(s8 *)(cw + 0xFA65) = 0;
}
