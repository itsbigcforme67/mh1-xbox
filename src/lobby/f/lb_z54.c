/* lb_z54 - auto-drafted 0x005F73C0-0x005F7400: BsCsInit (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsCur;

void BsCsInit(void) {
    F(s8, bsCur, 0) = 1;
    F(s8, bsCur, 1) = 0;
    F(s16, bsCur, 0x10) = 0x96;
    F(s16, bsCur, 0x12) = 0x64;
    F(s32, bsCur, 0x14) = 0;
    F(s32, bsCur, 0x18) = 0;
}
