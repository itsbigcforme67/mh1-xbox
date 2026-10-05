/* lb_z57 - auto-drafted 0x005F9440-0x005F94AC: historyForward (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsSys;

void historyForward(void) {
    s32 temp_v0;

    if (BsRouteForwardCheck() == 0) {
        To_BodyMain_ActDsp();
        return;
    }
    temp_v0 = BsRouteForward();
    if (temp_v0 == 0) {
        To_BodyMain_ActDsp();
        return;
    }
    F(s8, bsSys, 0x33) = 0;
    SetNextURL(temp_v0);
    To_BodyMain_RcvSrc();
}
