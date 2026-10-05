/* lb_z26 - auto-drafted 0x005F24A0-0x005F24C4: MainBsDispose (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void MainBsDispose(void) {
    BsCacheCleanup();
    HttpTaskCleanup();
}
