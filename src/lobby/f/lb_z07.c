/* lb_z07 - auto-drafted 0x005D9620-0x005D9648: HttpTaskCleanup (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

s32 HttpTaskCleanup(void) {
    sceHTTPSTerminate();
    sceHTTPTerminate();
    return 0;
}
