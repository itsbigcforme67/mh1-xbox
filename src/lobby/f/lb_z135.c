/* lb_z135 - auto-drafted 0x005E88A0-0x005E88CC: BsUrlCopy_SS (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

s32 BsUrlCopy_SS(s32 arg0, s32 arg1) {
    s32 temp_v0;

    temp_v0 = strncpy(arg0, arg1, 0xFF);
    F(s8, arg0, 0xFF) = 0;
    return temp_v0;
}
