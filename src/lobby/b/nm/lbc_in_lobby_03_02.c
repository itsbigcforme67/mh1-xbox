#include "lobby_f.h"
extern char CallBack_Result_Lobby_RoomExit[];
typedef struct { u8 pad0000[0x2C35]; u8 x2C35; } CWS_lbc_in_lobby_03_02;
void lbc_in_lobby_03_02(void) {
    u8 var_a0;
    var_a0 = ((CWS_lbc_in_lobby_03_02 *)cw)->x2C35;
    switch (var_a0) {                               /* irregular */
    case 0:
        Lbc_init_network_work(var_a0, ((CWS_lbc_in_lobby_03_02 *)cw));
        ((CWS_lbc_in_lobby_03_02 *)cw)->x2C35 = (u8) (((CWS_lbc_in_lobby_03_02 *)cw)->x2C35 + 1);
        F(s32, (u8 *)cw, 0x2C4C) = 0x44;
        nwSetEff_FreeDialog(0x14, (u8 *)cw + 0x32D1);
        /* fallthrough */
    case 1:
        var_a0 = (u8) (u8 *)cw;
        F(s32, var_a0, 0x2C4C) = (F(s32, var_a0, 0x2C4C) - 1);
        if (F(s32, (u8 *)cw, 0x2C4C) <= 0) {
        case 2:
            F(u8, (u8 *)cw, 0x2C35) = (u8) (F(u8, (u8 *)cw, 0x2C35) + 1);
            CallBackWaitInit((u8 *) var_a0, (u8 *)cw);
            F(s8, (u8 *)cw, 0x2C45) = 0x18;
            cnLBS_RoomExit(&CallBack_Result_Lobby_RoomExit);
            return;
        }
        return;
    case 3:
        Check_CallBackWait(var_a0, ((CWS_lbc_in_lobby_03_02 *)cw));
        break;
    }
}
