/* G online (PC): lobby_func_005ADFF0, written from Monster Hunter G's code at lobby 0x5ADFF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Read_RoomJoinUser();
s32 lobby_func_005AE0B0();
s32 lobby_func_005AE0F0();
s32 lobby_func_005AE130();
s32 lobby_func_005AE3D0();
s32 __cnetSub_Get_RestBgWork();                     /* extern */
s32 cnLBS_Read_MatchEntryJoinUser(); /* extern */

void lobby_func_005ADFF0(void *arg0) {
    if (__cnetSub_Get_RestBgWork() >= 3) {
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        cnLBS_Read_MatchEntryJoinUser((*(u16 *)((u8 *)(arg0) + 0x14)), lobby_func_005AE0B0);
        cnLBS_Read_RoomJoinUser((s32) (*(u16 *)((u8 *)(arg0) + 0x14)), (s32) lobby_func_005AE0F0);
        lobby_func_005AE3D0((s32) (*(u16 *)((u8 *)(arg0) + 0x14)), (s32) lobby_func_005AE130);
    }
}

