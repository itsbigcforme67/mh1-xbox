/* lb_bz116 - lobby UI/client 0x005BCBB0-0x005BCC44: CallBack_Result_Lobby_RoomEntry (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_Lobby_RoomEntry(CNET_RES res) {
    u8 *temp_a1;
    u8 *temp_a1_2;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x15)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a1_2 = (u8 *)cw;
            F(u8, temp_a1_2, 0x2C35) = (u8) (F(u8, temp_a1_2, 0x2C35) + 1);
            F(s8, (u8 *)cw, 0x35D3) = 1;
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 3U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
    }
}
