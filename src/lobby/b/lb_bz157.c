/* lb_bz157 - lobby UI/client 0x005B9400-0x005B9458: lbc_browser_03 (first drafted by tools/lbauto.py). */
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

void lbc_browser_03(void) {
    tmpPersonalData = BrPersonalData;
    ((CWS_b3 *)cw)->x2C43 = ((CWS_b3 *)cw)->x2C43 + 1;
    ((CWS_b3 *)cw)->x2C08 = 1;
    fade_set(2);
}
