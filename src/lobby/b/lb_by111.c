/* lb_by111 - agent B promoted near-match 0x005BAF50-0x005BB06C: CallBack_Result_Plaza_ReadLobbyAllocation (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
int cnLBS_Get_LobbyStatus(u16 id, void *p);
int cnLBS_Get_mhLobbyJoinUser(u16 id, void *a, void *b);
int cnLBS_Get_LobbyName(u16 id, void *p);
typedef struct { s8 x0; u8 pad1[5]; u16 x6; u8 pad8[0x18]; } CLSI;
extern CLSI ClassInfo;
typedef struct { s16 x0; u8 pad2[0x15A]; } PLZ;
extern PLZ LobbyInfo[];
typedef struct { u8 pad0[0x2C31]; u8 x2C31; u8 pad2C32[2]; u8 x2C34; u8 pad2C35[0x10]; u8 x2C45; } CWS_rl;
#define CWX ((CWS_rl *)cw)

void CallBack_Result_Plaza_ReadLobbyAllocation(CNET_RES res) {
    int sp3C;
    int i;
    PLZ *p;

    if ((CWX->x2C31 != 5) && (CWX->x2C45 == 0xB)) {
        if (res.val == 2 && res.id == 0xB) {
            cnLBS_Get_AllocationProgressCount(&sp3C);
            return;
        }
        if (res.val == 0) {
            CWX->x2C45 = 0;
            CWX->x2C34++;
            fade_set(1);
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
}
