/* lb_z63 - auto-drafted 0x005FD930-0x005FD9CC: DoFirstOpenRetry (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 bsFirstOpen;
extern u8 bsRetryCtr;
extern char FirstURL[];
extern char D_3A3B7C[];

s32 DoFirstOpenRetry(void) {
    u8 temp_v1;

    if (bsFirstOpen != 1) {
        return 0;
    }
    SetNextURL(&FirstURL);
    temp_v1 = bsRetryCtr + 1;
    bsRetryCtr = temp_v1;
    if ((temp_v1 & 0xFF) != 2) {
        if (strcmp(&FirstURL, &D_3A3B7C) == 0) {
            PostLbsInfoGetOrGameEnd();
            To_BodyMain_RcvSrc();
        } else {
            To_BodyMain_ReqSrc();
        }
        return 2;
    }
    return 0;
}
