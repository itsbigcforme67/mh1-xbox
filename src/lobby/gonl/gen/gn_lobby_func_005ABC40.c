/* G online (PC): lobby_func_005ABC40, written from Monster Hunter G's code at lobby 0x5ABC40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

void lobby_func_005ABC40(void *arg0) {
    (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
    (*(s8 *)((u8 *)(arg0) + 0x23)) = 0;
    if ((*(u16 *)((u8 *)&CnetSys_w + 0x43B6)) == 0) {
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 1;
        (*(u8 *)((u8 *)(arg0) + 0x22)) = 0U;
    }
}

