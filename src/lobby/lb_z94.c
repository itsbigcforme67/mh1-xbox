/* lb_z94 - auto-drafted 0x00603E30-0x00603E94: Init_Image (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

void Init_Image(void) {
    F(s16, bsw, 0xDF8) = 0x1E;
    F(s16, bsw, 0xDF6) = 0x1E;
    memset(bsw + 0xAF6, 0, 0x100);
    memset(bsw + 0xBF6, 0, 0x100);
    memset(bsw + 0xCF6, 0, 0x100);
}
