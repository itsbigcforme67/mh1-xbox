/* lb_by118 - agent B promoted near-match 0x005B9710-0x005B98A4: lobby_return_to_lobby (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char CallBack_ReadCurrentPlace[];
extern char CallBack_Result_Plaza_ReadAllocation2[];
extern char CallBack_Result_Plaza_ReadLobbyAllocation2[];
typedef struct { u8 pad0[0x2C08]; s8 x2C08; u8 pad2C09[0x29]; u8 x2C32; u8 pad2C33[0x12]; s8 x2C45; u8 pad2C46[0x98F]; s8 x35D5; } CWS_rl2;
#define CWX ((CWS_rl2 *)cw)

void lobby_return_to_lobby(void) {
    switch (CWX->x2C32) {
    case 0:
        CWX->x2C32++;
        CWX->x2C08 = 0;
        CWX->x2C45 = 1;
        CallBackWaitInit();
        cnLBS_Read_CurrentPlace(&CallBack_ReadCurrentPlace);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        CWX->x2C32++;
        cnLBS_Read_PlazaAllocation(0, 7, &CallBack_Result_Plaza_ReadAllocation2);
        Set_userdata((u8 *)&player_work + *(u8 *)0x3F34C1 * 0xA00);
        Lbc_SendMiniData();
        break;
    case 3:
        CWX->x2C32++;
        CWX->x35D5 = 1;
        MH_lobbyClear();
        cnLBS_Read_LobbyAllocation(0, 7, &CallBack_Result_Plaza_ReadLobbyAllocation2);
        break;
    case 4:
        switch (Lbs_request_enter_lobby2()) {
        case 0:
            CWX->x2C32++;
            break;
        case 1:
            break;
        }
        break;
    case 5:
        switch (Lbc_DownloadQuest()) {
        case 0:
            CWX->x2C08 = 1;
            To_EnterLobby();
            return;
        case 1:
            break;
        }
        break;
    }
}
