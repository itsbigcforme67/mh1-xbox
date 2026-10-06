/* lb_by110 - agent B promoted near-match 0x005BAC00-0x005BACAC: Lbs_GetLobbyMemberList (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
int cnLBS_Read_LobbyMemberList(u16 id, void *cb);
extern char CallBack_Result_Plaza_LobbyMember2[];
typedef struct { u8 pad0[0x2C45]; s8 x2C45; } CWS_gl;
#define CWX ((CWS_gl *)cw)

s32 Lbs_GetLobbyMemberList(arg0)
s16 arg0;
{
    switch (((u8 *)pNet)[0x12]) {
    case 0:
        CWX->x2C45 = 0xA;
        ((u8 *)pNet)[0x12]++;
        cnLBS_Read_LobbyMemberList(arg0 + 1, &CallBack_Result_Plaza_LobbyMember2);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        ((u8 *)pNet)[0x12] = 0;
        return 1;
    }
    return 0;
}
