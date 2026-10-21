/* G online (PC): lobby_func_005AEE00, written from Monster Hunter G's code at lobby 0x5AEE00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pFunc;
s32 __cnetSub_Set_BgProcess(); /* extern */
s16 lobby_func_005AEE80();                       /* extern */

s32 lobby_func_005AEE00(s32 arg0, s32 arg1) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0 != -1) {
        (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&pFunc + 0xEE)) + (temp_v0 * 0x1C))) = lobby_func_005AEE80(arg0);
        var_v0 = temp_v0;
    }
    return var_v0;
}

