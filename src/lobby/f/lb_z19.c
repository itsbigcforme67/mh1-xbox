/* lb_z19 - auto-drafted 0x005E88D0-0x005E8914: BsUrlSet, BsUrlBaseSet (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char BsCacheCurrentBaseUrlstr[];

void BsUrlSet(s32 *arg0, s32 arg1) {
    *arg0 = arg1;
}

void BsUrlBaseSet(s32 arg0) {
    BsUrlBaseClear();
    BsUrlCopy_SS(&BsCacheCurrentBaseUrlstr, arg0);
}
