/* G online (PC): Lbs_GuestEnterRoom, written from Monster Hunter G's code at lobby 0x582880 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 cnLbc_CheckInFloorOrder();
extern u8 ClassInfo;
extern u8 RoomRule;
extern u8 cw;
extern u8 lb_sys;
s32 To_EnterRoom();                          /* extern */
s32 gnet_room_entry(); /* extern */
extern s32 CallBack_Result_Lobby_RoomEntry;

s32 Lbs_GuestEnterRoom(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_a0;
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a0 = temp_v1 + 0xB9D;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x25;
        (*(s8 *)((u8 *)&ClassInfo + 0x8)) = (*(u8 *)((u8 *)&lb_sys + 0x7E)) + 1;
        gnet_room_entry(cnLbc_CheckInFloorOrder(2, arg1, arg2, arg3) & 0xFFFF, &(*(s32 *)((u8 *)&RoomRule + 0x2)), &CallBack_Result_Lobby_RoomEntry);
block_12:
    default:
        return 2;
    case 1:
        Check_CallBackWait(temp_a0, arg1, arg2, arg3);
        goto block_12;
    case 2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        To_EnterRoom(temp_a0);
        return 0;
    case 3:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

