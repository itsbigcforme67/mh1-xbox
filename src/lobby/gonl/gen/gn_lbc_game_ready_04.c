/* G online (PC): lbc_game_ready_04, written from Monster Hunter G's code at lobby 0x583F00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 CallBack_Result_Match_Logout();
s32 Check_CallBackWait();
extern u8 COM_R_No_0;
extern u8 COM_R_No_1;
extern u8 COM_R_No_2;
extern u8 COM_R_No_3;
extern u8 COM_R_No_4;
extern u8 COM_R_No_5;
extern u8 COM_R_No_6;
extern u8 CnetWork;
extern u8 System_timer;
extern u8 cw;
extern u8 net_game_invalid_flag;
extern u8 netr_ret;
s32 cnLBS_LogoutLobbyServer();        /* extern */
s32 cnWrap_BgmFadeOut();     /* extern */

void lbc_game_ready_04(void) {
    u8 temp_a1;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;

    temp_v1 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_a1) {
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_a1 + 1);
        cnWrap_BgmFadeOut(0xF, temp_a1, temp_v1 + 0xB9C);
        /* fallthrough */
    case 1:
        temp_v1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) + 1);
        /* fallthrough */
    case 2:
        temp_v1_3 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) + 1);
        /* fallthrough */
    case 3:
        temp_v1_4 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_4) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_4) + 0xB9C)) + 1);
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x31;
        cnLBS_LogoutLobbyServer(CallBack_Result_Match_Logout);
        return;
    case 4:
        Check_CallBackWait();
        return;
    case 5:
        (*(s32 *)((u8 *)&CnetWork + 0x8)) = 0;
        (*(s8 *)&net_game_invalid_flag) = 1;
        (*(s8 *)&COM_R_No_1) = 0;
        (*(s8 *)((u8 *)&CnetWork + 0x5)) = 3;
        (*(s8 *)&COM_R_No_0) = 4;
        (*(s8 *)&COM_R_No_2) = 0;
        (*(s8 *)&COM_R_No_3) = 0;
        (*(s8 *)&COM_R_No_4) = 0;
        (*(s8 *)&COM_R_No_5) = 0;
        (*(s8 *)&COM_R_No_6) = 0;
        (*(s32 *)&netr_ret) = 0;
        (*(s32 *)((u8 *)&CnetWork + 0x10)) = (s32) (*(u16 *)&System_timer);
        /* fallthrough */
    default:
        return;
    }
}

