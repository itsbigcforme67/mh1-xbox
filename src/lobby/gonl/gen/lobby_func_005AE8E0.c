/* G online (PC): lobby_func_005AE8E0, written from Monster Hunter G's code at lobby 0x5AE8E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void lobby_func_005AE8E0(void *arg0) {
    u8 temp_a1;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 0x23));
    if (temp_a1 == 7) {
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        goto block_5;
    }
    if (temp_a1 & 0x80) {
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
        (*(u8 *)((u8 *)(arg0) + 0x22)) = 0U;
block_5:
        (*(u8 *)((u8 *)(arg0) + 0x23)) = 0U;
    }
}

