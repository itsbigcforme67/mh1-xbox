/* lb_z121 - auto-drafted 0x005D9710-0x005D9740: HttpTaskPush (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 HttpTaskPush(int arg0) {
    if (F(s8, arg0, 0x68) != 0) {
        memset(arg0, 0, 0xC0);
    }
    return 0;
}
