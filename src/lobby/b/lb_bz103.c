/* lb_bz103 - lobby UI/client 0x005B6C70-0x005B6CB8: cmcs_03 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s8 USER_PL_ID;
extern u8 COM_R_No_1;

void cmcs_03(void) {
    int temp_v1;

    temp_v1 = (int)cw;
    COM_R_No_1 = (u8) (COM_R_No_1 + 1);
    LobbyToMcsInit(F(s8, temp_v1, 0x2C47), USER_PL_ID, temp_v1 + 0x35E0);
    LobbyToMcsInitSocket(*(s32 *)0x4E36F4);
    Vs_Cnt_0 = 0xE10;
}
