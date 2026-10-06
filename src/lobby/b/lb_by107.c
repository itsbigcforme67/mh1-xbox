/* lb_by107 - agent B promoted near-match 0x005B98B0-0x005B998C: CallBack_Result_Plaza_ReadLobbyAllocation2 (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
int cnLBS_Get_LobbyStatus(u16 id, void *p);
int cnLBS_Get_mhLobbyJoinUser(u16 id, void *a, void *b);
int cnLBS_Get_LobbyName(u16 id, void *p);
typedef struct { s8 x0; u8 pad1[5]; u16 x6; u8 pad8[0x18]; } CLSI;
extern CLSI ClassInfo;
typedef struct { s16 x0; u8 pad2[0x15A]; } PLZ;
extern PLZ LobbyInfo[];

void CallBack_Result_Plaza_ReadLobbyAllocation2(CNET_RES res) {
    int i;
    PLZ *p;

    if ((F(u8, (u8 *)cw, 0x2C31) != 5) && ((res.val != 2) || (res.id != 0xB)) && (res.val == 0)) {
        cnLBS_Get_LobbyCount(&ClassInfo.x6);
        i = 0;
        if (0 < ClassInfo.x6) {
            p = LobbyInfo;
            do {
                p->x0 = i + 1;
                cnLBS_Get_LobbyStatus(i + 1, (u8 *)p + 0x10);
                cnLBS_Get_mhLobbyJoinUser(i + 1, (u8 *)p + 2, (u8 *)p + 0xE);
                cnLBS_Get_LobbyName(i + 1, (u8 *)p + 0x14);
                i++;
                p++;
            } while (i < ClassInfo.x6);
        }
    }
}
