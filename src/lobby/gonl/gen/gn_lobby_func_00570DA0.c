/* G online (PC): lobby_func_00570DA0, written from Monster Hunter G's code at lobby 0x570DA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005E6B80();
extern u8 g_main_2EF2A8;
extern u8 g_main_2EF2A8;
s32 cnLBS_Send_UserMiniData(); /* extern */
extern u8 CallBack_Result_SendUserMiniData;
extern u8 my_user_mini_data;

s32 lobby_func_00570DA0(u8 arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    s32 temp_a0;
    s32 var_a0;
    s32 var_v1;

    temp_a0 = arg0 & 0xFF;
    if ((*(u8 *)((u8 *)(&my_user_mini_data) + 2)) == temp_a0) {
        return 0;
    }
    if ((*(u8 *)((u8 *)(&my_user_mini_data) + 2)) != 6) {
        var_v1 = 0;
    } else {
        var_v1 = 3;
    }
    if (temp_a0 != 6) {
        var_a0 = 0;
    } else {
        var_a0 = 3;
    }
    if ((var_a0 & 0xFF) != (var_v1 & 0xFF)) {
        lobby_func_005E6B80(var_a0, (s32) &CallBack_Result_SendUserMiniData, arg2, arg3);
    }
    (*(u8 *)((u8 *)(&my_user_mini_data) + 2)) = arg0;
    (*(u8 *)&g_main_2EF2A8) = arg0;
    cnLBS_Send_UserMiniData(&my_user_mini_data, 0x40, &CallBack_Result_SendUserMiniData);
    return 1;
}

