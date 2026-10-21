/* G online (PC): lobby_func_0058EDD0, written from Monster Hunter G's code at lobby 0x58EDD0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 sprintf();                /* extern */
extern u8 g_lb_63E908;

void lobby_func_0058EDD0(s32 arg1) {
    sprintf(&g_lb_63E908, arg1);
}

