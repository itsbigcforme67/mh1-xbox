/* lb_z108 - auto-drafted 0x005FDA50-0x005FDA94: BsHtmlMemcpy2 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void BsHtmlMemcpy2(int arg0, int arg1, s32 arg2) {
    memcpy((*(s32 *)arg0));
    (*(s32 *)arg0) += arg2 - 1;
}
