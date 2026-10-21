/* G online (PC): lobby_func_005C0620, written from Monster Hunter G's code at lobby 0x5C0620 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_lb_734DF4;
extern u8 g_lb_734DFC;

s32 lobby_func_005C0620(u8 *arg0, s32 **arg1) {
    if (arg0 != NULL) {
        *arg0 = g_lb_734DF4;
    }
    if (arg1 != NULL) {
        *arg1 = &g_lb_734DFC;
    }
    return 0;
}

