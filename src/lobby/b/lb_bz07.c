/* lb_bz07 - lobby UI/client 0x005B28A0-0x005B291C: cnWrap_ScreenReset, cnWrap_ScreenFadeCheck (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void cnWrap_ScreenReset(void) {
    fade_reset();
}

s32 cnWrap_ScreenFadeCheck(void) {
    s32 temp_v1;

    temp_v1 = Fade_busy_ck() & 0xFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 0;
    default:
        return 0;
    }
}
