/* G online (PC): cnLBS_Send_ChatBinaryTU, written from Monster Hunter G's code at lobby 0x5B0690 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 __cnetSub_Set_BgProcess();
s32 lobby_func_005B0730();
extern u8 pFunc;

s32 cnLBS_Send_ChatBinaryTU(s32 arg0, void *arg1, void *arg2, s32 arg3, s32 arg4) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg4);
    var_v0 = -1;
    if (temp_v0 != -1) {
        (*(u16 *)((u8 *)&(*(u16 *)((u8 *)&pFunc + 0xEE)) + (temp_v0 * 0x1C))) = lobby_func_005B0730(arg0, arg1, arg2, arg3);
        var_v0 = temp_v0;
    }
    return var_v0;
}

