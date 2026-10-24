/* G online (PC): Lbs_RoomExit, written from Monster Hunter G's code at lobby 0x583200 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Lobby_RoomExit();
s32 Check_CallBackWait();
s32 Lb_set_mini_data();
s32 Lbc_SendMiniData();
extern u8 cw;
extern u8 game_w;
extern u8 player_work;
s32 cnLBS_RoomExit(); /* extern */
extern s32 lbCommer;

s32 Lbs_RoomExit(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    u8 temp_a0;
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_3;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        /* fallthrough */
    case 1:
        temp_v1_3 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_3) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9D)) + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x28;
        cnLBS_RoomExit(CallBack_Result_Lobby_RoomExit);
block_11:
    default:
        return 0;
    case 2:
        Check_CallBackWait(temp_v1 + 0xB9D, arg1, arg2, arg3);
        goto block_11;
    case 3:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        temp_a0 = (*(u8 *)((u8 *)&game_w + 0x3A));
        *(&(*(u8 *)((u8 *)&player_work + 0x916)) + (temp_a0 * 0xA00)) = 0;
        Lbc_SendMiniData((s32) temp_a0, arg1, arg2, arg3);
        Lb_set_mini_data((*(void **)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x5C) + 0x3A8, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        Lb_set_mini_data((u8 *)&lbCommer + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x5C) + 0x1C, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return 1;
    }
}

