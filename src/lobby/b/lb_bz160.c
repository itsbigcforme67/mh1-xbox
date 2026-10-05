/* lb_bz160 - lobby UI/client 0x005C08F0-0x005C092C: Lbc_RequestNetComment (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
typedef struct BRPD { unsigned __int128 q[29]; } BRPD;
extern BRPD BrPersonalData;
extern BRPD tmpPersonalData;
typedef struct { u8 pad2C08[0x2C08]; s8 x2C08; u8 pad2C09[0x2C43 - 0x2C09]; u8 x2C43; } CWS_b3;
extern u8 sendDat[];
extern u8 D_3C73B4[];
void CallBack_Result_netComment();
void CallBack_Result_netCommentSend();
int cnLBS_Get_RoomMemberList();

int Lbc_RequestNetComment(int a) {
    sendDat[0] = 7;
    cnLBS_Send_ChatBinaryTU(a, sendDat, 1, CallBack_Result_netComment);
    return 0;
}
