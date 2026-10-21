/* G online (PC): lobby_func_005ABB50, written from Monster Hunter G's code at lobby 0x5ABB50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 __cnetSub_Get_RestBgWork();                     /* extern */
s32 cnLBS_Read_PlazaCount();           /* extern */
extern u8 _cnet_CallBack_Result_RoomSetFinish;

void lobby_func_005ABB50(void *arg0) {
    if (__cnetSub_Get_RestBgWork() > 0) {
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        cnLBS_Read_PlazaCount(&_cnet_CallBack_Result_RoomSetFinish);
    }
}

