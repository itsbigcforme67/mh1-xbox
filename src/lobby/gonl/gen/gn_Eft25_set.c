/* G online (PC): Eft25_set, written from Monster Hunter G's code at lobby 0x59D360 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnWrap_SoundRequest();
s32 Get_sw2();                               /* extern */

s32 Eft25_set(s32 arg0, s32 arg1) {
    s32 temp_v1;
    s32 var_s1;

    var_s1 = arg0;
    temp_v1 = Get_sw2(0) & 0xFFFF;
    if (temp_v1 & 0x800) {
        if (arg1 >= 2) {
            cnWrap_SoundRequest(1);
        }
        if (var_s1 == 0) {
            var_s1 = arg1 - 1;
        } else {
            var_s1 -= 1;
        }
    } else if (temp_v1 & 0x400) {
        if (arg1 >= 2) {
            cnWrap_SoundRequest(1);
        }
        var_s1 += 1;
        if (var_s1 >= arg1) {
            var_s1 = 0;
        }
    }
    return var_s1;
}

