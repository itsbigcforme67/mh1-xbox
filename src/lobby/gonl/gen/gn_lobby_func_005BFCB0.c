/* G online (PC): lobby_func_005BFCB0, written from Monster Hunter G's code at lobby 0x5BFCB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_lb_731162;

void lobby_func_005BFCB0(void *arg0) {
    u8 temp_a2;

    temp_a2 = (*(u8 *)((u8 *)(arg0) + 0x23));
    switch (temp_a2) {                              /* irregular */
    case 1:
        (*(u8 *)((u8 *)(arg0) + 0x23)) = 0U;
        if (g_lb_731162 == 0) {
        case 2:
            (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
            return;
        }
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 1;
        return;
    }
}

