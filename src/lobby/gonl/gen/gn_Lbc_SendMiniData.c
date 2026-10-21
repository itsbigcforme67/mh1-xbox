/* G online (PC): Lbc_SendMiniData, written from Monster Hunter G's code at lobby 0x582CE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_set_mini_data();
s32 lobby_func_005E6B80();
extern u8 cw;
extern u8 game_w;
extern u8 lb_sys;
s32 cnLBS_Send_UserMiniData(); /* extern */
s32 memcmp();          /* extern */
s32 memcpy();      /* extern */
extern u8 CallBack_Result_SendUserMiniData;
extern u8 lbCommer;
extern u8 my_user_mini_data;

s32 Lbc_SendMiniData(void) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s32 *)(g_frame + 0x10))
#define sp12 (*(u8 *)(g_frame + 0x12))
    s32 var_a0;

    Lb_set_mini_data(&sp10);
    if (memcmp(&sp10, &my_user_mini_data, 0x40) == 0) {
        return 0;
    }
    if (sp12 != (*(u8 *)((u8 *)&my_user_mini_data + 0x2))) {
        if (sp12 != 6) {
            var_a0 = 0;
        } else {
            var_a0 = 3;
        }
        lobby_func_005E6B80(var_a0, (s32) &CallBack_Result_SendUserMiniData);
    }
    memcpy((*(s32 *)&cw) + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x5C) + 0x3A8, &sp10, 0x40);
    memcpy((*(s32 *)&cw) + 0x1DC, &sp10, 0x40);
    memcpy(&my_user_mini_data, &sp10, 0x40);
    memcpy((u8 *)&lbCommer + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0x5C) + 0x1C, &sp10, 0x40);
    cnLBS_Send_UserMiniData(&sp10, 0x40, &CallBack_Result_SendUserMiniData);
    (*(s8 *)((u8 *)&lb_sys + 0x7F)) = 0;
    return 0;
}

