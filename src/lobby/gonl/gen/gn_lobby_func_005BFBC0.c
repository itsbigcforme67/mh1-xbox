/* G online (PC): lobby_func_005BFBC0, written from Monster Hunter G's code at lobby 0x5BFBC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_lb_731162;

void lobby_func_005BFBC0(void *arg0) {
    u8 temp_a1;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 0x23));
    switch (temp_a1) {                              /* irregular */
    case 1:
        (*(u8 *)((u8 *)(arg0) + 0x23)) = 0U;
        if (g_lb_731162 == 0) {
        case 2:
            (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
            return;
        }
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        (*(s32 *)((u8 *)(arg0) + 8)) = 0;
        (*(s32 *)((u8 *)(arg0) + 0xC)) = 0;
        (*(s8 *)((u8 *)(arg0) + 0x24)) = 0;
        return;
    }
}

