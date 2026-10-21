/* G online (PC): lobby_func_005BFF80, written from Monster Hunter G's code at lobby 0x5BFF80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_lb_731168;

void lobby_func_005BFF80(void *arg0) {
    if (g_lb_731168 != 0) {
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        (*(s8 *)((u8 *)(arg0) + 0x23)) = 0;
        (*(s32 *)((u8 *)(arg0) + 0xC)) = 0;
        return;
    }
    (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
    (*(u8 *)((u8 *)(arg0) + 0x22)) = 0U;
}

