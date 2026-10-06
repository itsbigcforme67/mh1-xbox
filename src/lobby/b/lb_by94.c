/* lb_by94 - agent B promoted near-match 0x005BB7B0-0x005BB834: CallBack_Result_Lobby_JoinUser (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
typedef struct { u8 pad[0x15C]; } LINFO;
extern LINFO LobbyInfo[];
extern char D_3A14C6[];

void CallBack_Result_Lobby_JoinUser(CNET_RES res) {
    int n;

    if (F(u8, cw, 0x2C31) != 5) {
        n = cnLbc_CheckInFloorOrder(1);
        if (res.val == 0) {
            cnLBS_Get_mhLobbyJoinUser(n & 0xFFFF, (u8 *)&LobbyInfo[n - 1] + 2, (u8 *)&LobbyInfo[n - 1] + 0xE);
            return;
        }
        *(s16 *)(D_3A14C6 + n * 0x15C) = 0;
    }
}
