#include "lobby_f.h"
extern char CallBack_Result_Lobby_LobbyExit[];
typedef struct { u8 pad0000[0x2C35]; u8 x2C35; } CWS_Lbs_LobbyExit;
s32 Lbs_LobbyExit(void) {
    u8 temp_v1;

    temp_v1 = F(u8, (u8 *)cw, 0x2C35);
    switch (temp_v1) {                              /* irregular */
    case 0:
        Lbc_init_network_work();
        fade_set(1);
        ((CWS_Lbs_LobbyExit *)cw)->x2C35 = (u8) (((CWS_Lbs_LobbyExit *)cw)->x2C35 + 1);
        /* fallthrough */
    case 1:
        ((CWS_Lbs_LobbyExit *)cw)->x2C35 = (u8) (((CWS_Lbs_LobbyExit *)cw)->x2C35 + 1);
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0x16;
        cnLBS_LobbyExit(&CallBack_Result_Lobby_LobbyExit);
block_12:
    default:
        return 0;
    case 2:
        Check_CallBackWait();
        goto block_12;
    case 3:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            F(u8, (u8 *)cw, 0x2C35) = 0U;
            return 1;
        }
        goto block_12;
    }
}
