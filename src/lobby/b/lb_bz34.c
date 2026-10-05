/* lb_bz34 - lobby UI/client 0x005BA240-0x005BA3AC: To_EnterPlaza, To_EnterPlaza2Lobby (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char ClassInfo[];
extern char LobbyInfo[];
extern char put_back[];
extern char CallBack_Result_Plaza_ReadAllocation2[];

void To_EnterPlaza(void) {
    F(s8, (u8 *)cw, 0x2C31) = 2;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    Init_InterruptFlag();
    F(s8, (u8 *)cw, 0x2C41) = 0;
    F(s8, (u8 *)cw, 0x2C42) = 0;
    F(s8, &ClassInfo, 4) = 0;
    F(s8, &ClassInfo, 8) = 0;
    memset(&LobbyInfo, 0, 0x1308);
    Plaza_chat_clear();
    Lb_clearChatList();
    fade_set(2);
    SetDialogData(0xB, 5);
}

void To_EnterPlaza2Lobby(void) {
    F(s8, (u8 *)cw, 0x2C31) = 2;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    Init_InterruptFlag();
    Lbc_init_network_work();
    F(s8, (u8 *)cw, 0x2C08) = 0;
    F(s8, &ClassInfo, 4) = 0;
    F(s8, &ClassInfo, 8) = 0;
    memset(&LobbyInfo, 0, 0x1308);
    Pit_reset();
    all_reset();
    Lbs_load();
    Plaza_chat_clear();
    Lb_clearChatList();
    Lbc_set_prim(&put_back, 0, 0);
    cnLBS_Read_PlazaAllocation(0, 7, &CallBack_Result_Plaza_ReadAllocation2);
    SetDialogData(0xC, 5);
}
