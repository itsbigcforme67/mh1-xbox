/* lb_bz37 - lobby UI/client 0x005BBD00-0x005BBD0C: Lbs_GetClassAdd (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char ClassInfo[];

u16 Lbs_GetClassAdd(void) {
    return F(u16, &ClassInfo, 0xA);
}
