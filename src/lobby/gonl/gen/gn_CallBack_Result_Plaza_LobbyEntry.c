/* G online (PC): CallBack_Result_Plaza_LobbyEntry, written from Monster Hunter G's code at lobby 0x57F220 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 CallBack_Result_Plaza_LobbyMember();
s32 Check_CallBackWait();
s32 cnLbc_CheckInFloorOrder();
extern u8 cw;
s32 cnLBS_Read_LobbyMemberList(); /* extern */

s32 CallBack_Result_Plaza_LobbyEntry(void) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0xE;
        cnLBS_Read_LobbyMemberList(cnLbc_CheckInFloorOrder(1) & 0xFFFF, CallBack_Result_Plaza_LobbyMember);
block_12:
        return 2;
    case 0x1:
        Check_CallBackWait();
        goto block_12;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    default:
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

