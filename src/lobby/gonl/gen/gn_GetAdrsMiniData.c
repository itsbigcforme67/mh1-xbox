/* G online (PC): GetAdrsMiniData, written from Monster Hunter G's code at lobby 0x59E190 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 GetAdrsMiniData(s32 arg0) {
    return (*(s32 *)&cw) + ((arg0 & 0xFF) * 0x5C) + 0x3A8;
}

