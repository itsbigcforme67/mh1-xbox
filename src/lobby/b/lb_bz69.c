/* lb_bz69 - lobby UI/client 0x005B2880-0x005B2898: cnWrap_ScreenFadeIn, cnWrap_ScreenFadeOut (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void cnWrap_ScreenFadeIn(void) {
    fade_set(2);
}

void cnWrap_ScreenFadeOut(void) {
    fade_set(1);
}
