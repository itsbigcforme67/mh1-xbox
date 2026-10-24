/* G online (PC): cnLbc_CheckInFloorOrder, written from Monster Hunter G's code at lobby 0x5867F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ClassInfo;

u8 cnLbc_CheckInFloorOrder(s32 arg0) {
    s32 temp_v1;

    temp_v1 = arg0 & 0xFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        return ClassInfo;
    case 1:
        return (*(u8 *)((u8 *)&ClassInfo + 0x4));
    case 2:
        return (*(u8 *)((u8 *)&ClassInfo + 0x8));
    default:
        return 0U;
    }
}

