/* G online (PC): lobby_func_005C0040, written from Monster Hunter G's code at lobby 0x5C0040 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u32 g_lb_731168;
extern u32 g_lb_73116C;

void lobby_func_005C0040(void *arg0) {
    u8 temp_v1;

    temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x23));
    if (temp_v1 == 1) {
        if ((u32) g_lb_73116C >= (u32) g_lb_731168) {
            (*(s8 *)((u8 *)(arg0) + 0x21)) = 1;
            (*(s8 *)((u8 *)(arg0) + 0x22)) = 0;
        }
    } else if (temp_v1 & 0x80) {
        (*(u8 *)((u8 *)(arg0) + 0x23)) = 0U;
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
        (*(s8 *)((u8 *)(arg0) + 0x22)) = 0;
    }
}

