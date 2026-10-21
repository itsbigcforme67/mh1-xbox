/* G online (PC): cnLBS_PlazaEntry, written from Monster Hunter G's code at lobby 0x5ACB70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 __cnet_KeepEntryFloorInfo();
s32 __cnet_SendSet_ChatMessageTU();
extern u8 pFunc;
s32 __cnetSub_Set_BgProcess(); /* extern */

s32 cnLBS_PlazaEntry(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_v0;
    s32 var_v0;

    temp_v0 = __cnetSub_Set_BgProcess(1, 0, arg3);
    var_v0 = -1;
    if (temp_v0 != -1) {
        __cnet_KeepEntryFloorInfo(0, arg0);
        __cnet_KeepEntryFloorInfo(1, arg1);
        __cnet_KeepEntryFloorInfo(2, arg2);
        (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&pFunc + 0xEE)) + (temp_v0 * 0x1C))) = __cnet_SendSet_ChatMessageTU(arg0, arg1, arg2);
        var_v0 = temp_v0;
    }
    return var_v0;
}

