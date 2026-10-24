/* G online (PC): Lb_Matching, written from Monster Hunter G's code at lobby 0x55CCD0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 Lbs_MatchStart();                         /* extern */

void Lb_Matching(void) {
    u8 temp_a0;

    temp_a0 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xF751));
    if (temp_a0 == 1) {
        Lbs_MatchStart(temp_a0);
    }
}

