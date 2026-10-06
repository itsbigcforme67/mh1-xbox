#include "lobby_b.h"
extern char s64[];
extern char unksp19[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
extern char my_user_id[];
extern char unksp19[];
extern char my_user_id[];
extern char unksp19[];
extern char unksp19[];
extern char unksp19[];
void CallBack_Result_LoginLobbyServer(CNET_RES res) {
    int temp_a0_2;
    int temp_v1;
    int temp_a0;
    int temp_v0;
    if (res.val != -1) {
        temp_v1 = (s8)unksp19;
        switch (temp_v1) {                          /* switch 1 */
        case 4:                                     /* switch 1 */
            F(s8, (u8 *)cw, 0x2C33) = 2;
            F(u8, (u8 *)cw, 0x2C34) = 0U;
            cnLBS_Get_LoginWarningMessage((u8 *)cw + 0x35FE, unksp19);
            return;
        case 1:                                     /* switch 1 */
            switch (F(u8, &CnetWork, 5)) { /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                F(s8, (u8 *)cw, 0x2C33) = 4;
                F(u8, (u8 *)cw, 0x2C34) = 0U;
                F(s8, (u8 *)cw, 0x2C35) = 0;
                fade_set(1, F(u8, &CnetWork, 5));
                return;
            case 2:                                 /* switch 2 */
            case 1:                                 /* switch 2 */
                F(s8, (u8 *)cw, 0x2C33) = 4;
                F(u8, (u8 *)cw, 0x2C34) = 6U;
                F(s8, (u8 *)cw, 0x2C35) = 0;
                CallBackWaitInit(4, F(u8, &CnetWork, 5));
                temp_v0 = (int)cw;
                cnLBS_Send_LoginUserAccount(temp_v0 + 0x440, temp_v0 + 0x448, &my_user_mini_data);
                return;
            case 3:                                 /* switch 2 */
                F(s8, (u8 *)cw, 0x2C33) = 4;
                F(u8, (u8 *)cw, 0x2C34) = 6U;
                F(s8, (u8 *)cw, 0x2C35) = 0;
                CallBackWaitInit(4, F(u8, &CnetWork, 5));
                cnLBS_Send_LoginUserAccount(&my_user_id, &my_user_handle, &my_user_mini_data);
                return;
            }
            break;
        case 2:                                     /* switch 1 */
            cnetGet_Login_DecideUserID((u8 *)cw + 0x440, unksp19);
            memcpy(&my_user_id, (u8 *)cw + 0x440, 8);
            cnetGet_Login_DecideUserHandle((u8 *)cw + 0x448);
            memcpy(&my_user_handle, (u8 *)cw + 0x448, 0x12);
            temp_a0 = (int)cw;
            F(u8, temp_a0, 0x2C34) = (u8) (F(u8, temp_a0, 0x2C34) + 1);
            return;
        case 5:                                     /* switch 1 */
            F(s8, (u8 *)cw, 0x2C33) = 1;
            F(u8, (u8 *)cw, 0x2C34) = 0U;
            return;
        case 3:                                     /* switch 1 */
            F(s8, (u8 *)cw, 0x2C33) = 3;
            F(u8, (u8 *)cw, 0x2C34) = 0U;
            return;
        case 0:                                     /* switch 1 */
            F(s8, (u8 *)cw, 0x2C33) = 7;
            F(u8, (u8 *)cw, 0x2C34) = 0U;
            return;
        }
    } else {
        temp_a0_2 = (s8)unksp19;
        switch (temp_a0_2) {                        /* switch 3; irregular */
        case 7:                                     /* switch 3 */
            F(s8, (u8 *)cw, 0x2C33) = 6;
            F(u8, (u8 *)cw, 0x2C34) = 0U;
            F(s8, (u8 *)cw, 2) = 1;
            fade_set(1, (u8) unksp19);
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1);
            return;
        case 8:                                     /* switch 3 */
            F(s8, (u8 *)cw, 0x2C33) = 6;
            F(u8, (u8 *)cw, 0x2C34) = 0U;
            F(s8, (u8 *)cw, 2) = 2;
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, unksp19);
            return;
        case 9:                                     /* switch 3 */
            To_LogOut(4, unksp19);
        default:                                    /* switch 1 */
            break;
        }
    }
}
