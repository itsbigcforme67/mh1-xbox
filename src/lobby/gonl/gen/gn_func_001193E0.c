/* G online (PC): func_001193E0, written from Monster Hunter G's code at lobby 0x1193E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern void *lpPit;

void func_001193E0(void) {
    (*(s8 *)((u8 *)(lpPit) + 0x94)) = 1;
}

