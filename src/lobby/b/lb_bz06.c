/* lb_bz06 - lobby UI/client 0x005B2630-0x005B2688: cnWrap_BgmVolume, cnWrap_BgmFadeOut, cnWrap_BgmRequest (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void cnWrap_BgmVolume(void) {

}

void cnWrap_BgmFadeOut(s32 arg0) {
    s32 temp_s0;

    temp_s0 = arg0 & 0xFFFF;
    str_fadeout(0, temp_s0);
    str_fadeout(1, temp_s0);
}

void cnWrap_BgmRequest(void) {

}
