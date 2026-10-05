/* lb_z47 - auto-drafted 0x005E6D60-0x005E6D6C: BsRouteCurrent (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern s32 BcRoute_cur;

s32 BsRouteCurrent(void) {
    return BcRoute_cur + 4;
}
