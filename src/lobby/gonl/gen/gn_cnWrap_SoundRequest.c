/* G online (PC): cnWrap_SoundRequest, written from Monster Hunter G's code at lobby 0x5C0F40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 se_req();          /* extern */

void cnWrap_SoundRequest(u32 arg0) {
    switch (arg0) {
    case 0:
        se_req(7, 0x13, 0);
        return;
    case 3:
        se_req(7, 0x14, 0);
        return;
    case 1:
        se_req(7, 0x17, 0);
        return;
    case 5:
        se_req(7, 0x27, 0);
        return;
    case 6:
        se_req(7, 0x13, 0);
        return;
    case 8:
        se_req(7, 0x1A, 0);
        return;
    case 9:
        se_req(7, 0x11, 0);
        return;
    case 10:
        se_req(7, 0x1E, 0);
        return;
    case 11:
        se_req(6, 0x40, 0);
        return;
    case 12:
        se_req(7, 0x1C, 0);
        return;
    case 14:
        se_req(7, 0x11, 0);
        return;
    case 13:
        se_req(6, 0x47, 0);
        return;
    case 15:
        se_req(7, 9, 0);
        return;
    case 16:
        se_req(7, 0x2D, 0);
        return;
    case 17:
        se_req(7, 0x1F, 0);
        return;
    case 18:
        se_req(1, 0x77, 0);
        return;
    case 19:
        se_req(7, 0x1C, 0);
        return;
    case 4:
    case 7:
        se_req(7, 0x15, 0);
        return;
    case 2:
        se_req(7, 0x10, 0);
        /* fallthrough */
    default:
        return;
    }
}

