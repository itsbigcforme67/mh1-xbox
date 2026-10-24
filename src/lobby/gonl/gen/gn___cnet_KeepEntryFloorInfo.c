/* G online (PC): __cnet_KeepEntryFloorInfo, written from Monster Hunter G's code at lobby 0x5AB760 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

void __cnet_KeepEntryFloorInfo(s32 arg0, s8 arg1) {
    s32 temp_a0;

    temp_a0 = arg0 & 0xFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        (*(s8 *)((u8 *)&CnetSys_w + 0x43BA)) = arg1;
        return;
    case 1:
        (*(s8 *)((u8 *)&CnetSys_w + 0x43BB)) = arg1;
        return;
    case 2:
        (*(s8 *)((u8 *)&CnetSys_w + 0x43BC)) = arg1;
        return;
    }
}

