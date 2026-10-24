/* G online (PC): Lbs_GetClassAdd, written from Monster Hunter G's code at lobby 0x581B10 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ClassInfo;

u16 Lbs_GetClassAdd(void) {
    return (*(u16 *)((u8 *)&ClassInfo + 0xA));
}

