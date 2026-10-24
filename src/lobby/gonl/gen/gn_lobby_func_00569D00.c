/* G online (PC): lobby_func_00569D00, written from Monster Hunter G's code at lobby 0x569D00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
void lobby_func_00569D00(void) {
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 5;
}

