/* G online (PC): Lb_cursorUD, written from Monster Hunter G's code at lobby 0x59D2B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 cnWrap_SoundRequest();
s32 Lb_cursorUD(s32 arg0, s32 arg1) {
    s32 temp_v1;
    s32 var_s1;

    var_s1 = arg0;
    temp_v1 = Get_sw2(0) & 0xFFFF;
    if (temp_v1 & 0x2000) {
        if (arg1 >= 2) {
            cnWrap_SoundRequest(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        if (var_s1 == 0) {
            var_s1 = arg1 - 1;
        } else {
            var_s1 -= 1;
        }
    } else if (temp_v1 & 0x1000) {
        if (arg1 >= 2) {
            cnWrap_SoundRequest(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        var_s1 += 1;
        if (var_s1 >= arg1) {
            var_s1 = 0;
        }
    }
    return var_s1;
}

