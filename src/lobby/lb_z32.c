/* lb_z32 - auto-drafted 0x005F94B0-0x005F9514: historyBack (first drafted by tools/lbauto.py). */
#include "lobby.h"

void historyBack(void) {
    s32 temp_v0;

    if (BsRouteBackCheck() == 0) {
        To_BodyMain_ActDsp();
        return;
    }
    temp_v0 = BsRouteBack();
    if (temp_v0 == 0) {
        To_BodyMain_ActDsp();
        return;
    }
    SetNextURL(temp_v0);
    To_BodyMain_RcvSrc();
}
