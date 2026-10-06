#include "lobby_a.h"
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
void lbc_login_finish_after(void) {
    s32 temp_a0_2;
    s32 temp_v1;
    u8 temp_a2;
    int temp_a0;
    int temp_a3;
    int temp_v1_2;

    temp_a0 = (int)cw;
    temp_a2 = F(u8, temp_a0, 0x2C34);
    temp_a3 = temp_a0 + 0x2C34;
    switch (temp_a2) {                              /* irregular */
    case 0:
        if (F(u8, &CnetWork, 5) == 3) {
            F(u8, temp_a0, 0x2C34) = (u8) (temp_a2 + 1);
            McOperationSet(7, 2);
            fade_set(2);
            F(s32, (u8 *)cw, 0x2C4C) = 3;
            str_stop_all();
            return;
        }
        F(u8, temp_a0, 0x2C34) = 2U;
        return;
    case 1:
        temp_v1 = F(s32, temp_a0, 0x2C4C);
        if (temp_v1 == 0) {
            if (McCardOperation(temp_a0, temp_a0 + 0x2C4C) & 0xFF) {
                temp_v1_2 = (int)cw;
                F(u8, temp_v1_2, 0x2C34) = (u8) (F(u8, temp_v1_2, 0x2C34) + 1);
                fade_set(1);
                return;
            }
            return;
        }
        F(s32, temp_a0, 0x2C4C) = (temp_v1 - 1);
        return;
    case 2:
        temp_a0_2 = Fade_busy_ck(temp_a0, 2) & 0xFF;
        if (temp_a0_2 != 1) {
            cnLBS_Send_LoginFinish();
            if (F(u8, &CnetWork, 5) == 3) {
                To_MyLobby();
                F(s8, (u8 *)cw, 0x35D2) = 1;
            } else {
                To_MyLobby();
                F(s8, (u8 *)cw, 0x35D2) = 0;
            }
            F(u8, &CnetWork, 5) = 2U;
            Q_camera_init();
        }
        break;
    }
}
