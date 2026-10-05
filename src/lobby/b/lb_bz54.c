/* lb_bz54 - lobby UI/client 0x005C1210-0x005C1254: CallBack_Event_LobbyJoinUser (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char LobbyInfo[];

void CallBack_Event_LobbyJoinUser(void) {
    u16 sp1E;
    int temp_v0;

    cnetGet_Room_LastRecvNumber(&sp1E);
    temp_v0 = (int)&LobbyInfo + ((sp1E - 1) * 0x15C);
    cnLBS_Get_mhLobbyJoinUser(sp1E, temp_v0 + 2, temp_v0 + 0xE);
}
