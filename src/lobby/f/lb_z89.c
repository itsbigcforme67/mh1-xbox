/* lb_z89 - auto-drafted 0x00601420-0x00601458: tagAct_348 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsw;

s32 tagAct_348(void) {
    void *temp_a0;

    temp_a0 = bsw;
    if (F(s8, temp_a0, 0x186) != -0xA) {
        return 0;
    }
    F(s8, temp_a0, 0x18A) = 2;
    return 0;
}
