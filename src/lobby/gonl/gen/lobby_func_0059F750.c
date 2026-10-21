/* G online (PC): lobby_func_0059F750, written from Monster Hunter G's code at lobby 0x59F750 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern s16 g_lb_654CFC;
extern s16 g_lb_654CFE;

void lobby_func_0059F750(s16 arg0, s16 arg1) {
    g_lb_654CFC = arg0;
    g_lb_654CFE = arg1;
}

