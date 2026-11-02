/* lb_bz73 - lobby UI/client 0x005B2600-0x005B2628: cpn_PutCNData, cnWrap_BgmStop (first drafted by tools/lbauto.py). */
#include "g_alias.h"
#include "lobby_a.h"
extern char CNFile[];
extern char Friend_data[];

#if !defined(G_PART) || G_PART == 2
void cpn_PutCNData(void) {
    memcpy(&CNFile, Friend_data, MH2V(GV(2400, 80), 0x50));
}
#endif

#if !defined(G_PART) || G_PART == 1
void cnWrap_BgmStop(void) {
    str_stop(0);
}
#endif
