/* lb_z121 - auto-drafted 0x005D9710-0x005D9740: HttpTaskPush (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 HttpTaskPush(int arg0) {
    if (F(s8, arg0, MH2V(0x68, 0x7C)) != 0) {
        memset(arg0, 0, MH2V(0xC0, 0xE4));
    }
    return 0;
}
