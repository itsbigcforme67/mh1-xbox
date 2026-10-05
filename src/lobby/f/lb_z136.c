/* lb_z136 - auto-drafted 0x005FD9D0-0x005FDA4C: RetryShadowPost (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 bsNetErrOccur;
extern u8 * bsCur;
extern u8 * bsSys;
extern u8 bsRetryCtr;

s32 RetryShadowPost(void) {
    s8 sx1;
    u8 temp_v1;

    temp_v1 = bsRetryCtr + 1;
    bsRetryCtr = temp_v1;
    if (((temp_v1 & 0xFF) != 2) && ((sx1 = shadowPost()) == 1)) {
        return 1;
    }
    F(s8, bsSys, 0x30) = 0;
    bsNetErrOccur = 3;
    To_NetErrorDialog(0);
    F(s8, bsCur, 2) = 1;
    return 2;
}
