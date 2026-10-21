/* G online (PC): func_00220F40, written from Monster Hunter G's code at lobby 0x220F40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SoftKeyboard_set();
s32 func_0021E370();
void func_00220F40(s32 arg0, s32 arg1, s32 arg2, void *arg3) {
    SoftKeyboard_set(arg0 & 0xFF, arg1, arg2, arg3);
    if (arg0 & 0x80000000) {
        func_0021E370(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    }
    func_0021E370(-1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

