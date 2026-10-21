/* G online (PC): getUserInfo, written from Monster Hunter G's code at lobby 0x56C240 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lbs_SeekId();
s32 Lbc_RequestNetComment();                 /* extern */

s32 getUserInfo(s32 arg0) {
    s32 temp_v0;

    temp_v0 = Lbs_SeekId(0x64, arg0);
    switch (temp_v0) {                              /* irregular */
    case 0:
        Lbc_RequestNetComment(arg0);
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

