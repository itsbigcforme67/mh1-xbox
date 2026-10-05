/* lb_z22 - auto-drafted 0x005ED6E0-0x005ED6F8: zcalloc, zcfree (first drafted by tools/lbauto.py). */
#include "lobby.h"

void zcalloc(void) {
    _zlib_calloc();
}

void zcfree(void) {
    _zlib_free();
}
