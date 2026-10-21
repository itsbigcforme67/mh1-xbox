/* G online (PC): lobby_func_005E6C80, written from Monster Hunter G's code at lobby 0x5E6C80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 __cnetSub_Set_BgProcess();
s32 lobby_func_005E6D60();
void * memset();
extern u8 pFunc;
extern u8 g_lb_733CAA;

s32 lobby_func_005E6C80(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_v0;
    s32 var_v0;

    memset(&g_lb_733CAA, 0, 0x190);
    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg2);
    var_v0 = -1;
    if (temp_v0 != -1) {
        *(&(*(u8 *)((u8 *)&pFunc + 0xEE)) + (temp_v0 * 0x1C)) = lobby_func_005E6D60(arg0, arg1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        var_v0 = temp_v0;
    }
    return var_v0;
}

