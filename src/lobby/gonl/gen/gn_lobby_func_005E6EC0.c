/* G online (PC): lobby_func_005E6EC0, written from Monster Hunter G's code at lobby 0x5E6EC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern s32 g_lb_733E3A;

s32 lobby_func_005E6EC0(s32 **arg0) {
    *arg0 = &g_lb_733E3A;
    return 0;
}

