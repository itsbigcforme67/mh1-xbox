#include "lobby_a.h"
extern char jtbl_2502[];
extern char jtbl_2502[];
extern char CallBack_Result_Lobby_LobbyExit[];
extern char jtbl_2502[];
extern char jtbl_2502[];
void lbc_in_lobby_03_01(void) {
    int temp_a0;
    int temp_a0_2;
    int temp_v1;
    int temp_v1_2;
    int var_a0;
    u8 temp_a1;
    int temp_a2;

    temp_v1 = (int)cw;
    temp_a1 = F(u8, temp_v1, 0x2C35);
    temp_a2 = temp_v1 + 0x2C35;
    if (temp_a1 < 6U) {
        var_a0 = (int)&jtbl_2502;
        switch (temp_a1) {
        case 0:
            F(u8, temp_v1, 0x2C35) = (u8) (temp_a1 + 1);
            Lbc_init_network_work(&jtbl_2502, temp_a1, temp_a2);
            F(s32, (int)cw, 0x2C4C) = 0x26;
            SetDialogData_HTML((int)cw + 0x32D1);
            /* fallthrough */
        case 1:
            F(s8, pNet, 0xC) = 1;
            temp_a0 = (int)cw;
            F(s32, temp_a0, 0x2C4C) = (F(s32, temp_a0, 0x2C4C) - 1);
            var_a0 = (int)cw;
            if (F(s32, var_a0, 0x2C4C) <= 0) {
                F(u8, var_a0, 0x2C35) = (u8) (F(u8, var_a0, 0x2C35) + 1);
            case 2:
                temp_v1_2 = (int)cw;
                F(u8, temp_v1_2, 0x2C35) = (u8) (F(u8, temp_v1_2, 0x2C35) + 1);
                CallBackWaitInit(var_a0);
                F(s8, (int)cw, 0x2C45) = 0x16;
                cnLBS_LobbyExit(&CallBack_Result_Lobby_LobbyExit);
                return;
            }
            break;
        case 3:
            Check_CallBackWait(&jtbl_2502);
            return;
        case 4:
            fade_set(1);
            temp_a0_2 = (int)cw;
            F(u8, temp_a0_2, 0x2C35) = (u8) (F(u8, temp_a0_2, 0x2C35) + 1);
            return;
        case 5:
            if ((Fade_busy_ck(&jtbl_2502) & 0xFF) != 1) {
                F(s8, (int)cw, 0x35D5) = 0;
                Lbc_set_prim(0, 0, 0);
                To_EnterPlaza2Lobby();
            }
            break;
        }
    }
}
