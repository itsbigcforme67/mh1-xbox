/* G online (PC): lobby_func_0059F6F0, written from Monster Hunter G's code at lobby 0x59F6F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 helpLineStr;
extern s16 g_lb_654CFC;
extern s16 g_lb_654CFE;

void lobby_func_0059F6F0(void) {
    (*(s32 *)((u8 *)&helpLineStr + 0x4)) = 0;
    (*(s32 *)((u8 *)&helpLineStr + 0x8)) = 0;
    g_lb_654CFC = 0x39;
    g_lb_654CFE = 0x16C;
}

