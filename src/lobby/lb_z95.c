/* lb_z95 - auto-drafted 0x00604990-0x006049D8: input_type_clear (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

void input_type_clear(void) {
    memset(bsw + 0x2E4, 0, 0x40C);
    F(s16, bsw, 0x4E8) = 0x100;
    F(s16, bsw, 0x6EE) = 0x1E;
    F(s16, bsw, 0x6EC) = 0x1E;
}
