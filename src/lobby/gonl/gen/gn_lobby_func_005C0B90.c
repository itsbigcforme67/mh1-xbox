/* G online (PC): lobby_func_005C0B90, written from Monster Hunter G's code at lobby 0x5C0B90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_lb_734E34;

s32 lobby_func_005C0B90(u8 *arg0) {
    *arg0 = g_lb_734E34;
    return 0;
}

