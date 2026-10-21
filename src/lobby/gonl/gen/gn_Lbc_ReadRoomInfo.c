/* G online (PC): Lbc_ReadRoomInfo, written from Monster Hunter G's code at lobby 0x581830 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
void * memset();
extern u8 cw;
s32 cnLBS_Read_RoomAllocation(); /* extern */
extern s32 CallBack_Result_Lobby_ReadRoomAllocation;
extern s32 g_lb_6DE120;
extern s16 RoomInfo;

s32 Lbc_ReadRoomInfo(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        memset(&g_lb_6DE120, 0, 0xAE0);
        RoomInfo = 1;
        (*(s16 *)((u8 *)&RoomInfo + 0x15C)) = 2;
        (*(s16 *)((u8 *)&RoomInfo + 0x2B8)) = 3;
        (*(s16 *)((u8 *)&RoomInfo + 0x414)) = 4;
        (*(s16 *)((u8 *)&RoomInfo + 0x570)) = 5;
        (*(s16 *)((u8 *)&RoomInfo + 0x6CC)) = 6;
        (*(s16 *)((u8 *)&RoomInfo + 0x828)) = 7;
        (*(s16 *)((u8 *)&RoomInfo + 0x984)) = 8;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x1E;
        cnLBS_Read_RoomAllocation(0, 0xBB, &CallBack_Result_Lobby_ReadRoomAllocation);
block_12:
    default:
        return 2;
    case 1:
        Check_CallBackWait(temp_v1 + 0xB9D, arg1, arg2, arg3);
        goto block_12;
    case 2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 3:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

