/* lb_z104 - auto-drafted 0x005E10B0-0x005E1100: stockBaseURL (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s32 bsSys;

void stockBaseURL(s32 arg0) {
    memset(bsSys + 0x13B, 0, 0x100);
    strcpy(bsSys + 0x13B, arg0);
    BsUrlBaseSet(bsSys + 0x13B);
}
