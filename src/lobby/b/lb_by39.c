/* lb_by39 - agent B promoted near-match 0x005C09B0-0x005C0A08: cnLbc_CheckInFloorOrder (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 ClassInfo[];

s32 cnLbc_CheckInFloorOrder(s32 arg0) {
    s32 r;
    switch (arg0 & 0xFF) {
    case 0:
        r = ClassInfo[0];
        break;
    case 1:
        r = ClassInfo[4];
        break;
    case 2:
        r = ClassInfo[8];
        break;
    }
    return r;
}
