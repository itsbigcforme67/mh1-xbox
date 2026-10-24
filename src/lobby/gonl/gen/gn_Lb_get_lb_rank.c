/* G online (PC): Lb_get_lb_rank, written from Monster Hunter G's code at lobby 0x59D240 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 User_data;
s32 Lb_get_lb_rank(void) {
    u8 temp_v0;

    temp_v0 = (*(u8 *)((u8 *)&User_data + 0xC23));
    if ((s32) temp_v0 < 5) {
        return 0;
    }
    if ((s32) temp_v0 < 0xD) {
        return 1;
    }
    if ((s32) temp_v0 < 0x15) {
        return 2;
    }
    if ((s32) temp_v0 < 0x1B) {
        return 3;
    }
    return 4;
}

