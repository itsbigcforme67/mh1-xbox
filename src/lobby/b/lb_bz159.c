/* lb_bz159 - lobby UI/client 0x005BD700-0x005BD78C: CallBack_Result_InRoom00_Member (first drafted by tools/lbauto.py). */
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
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; } CWS_im;

void CallBack_Result_InRoom00_Member(CNET_RES res) {
    int i;
    int off;

    if (((CWS_im *)cw)->x2C31 != 5) {
        memset(cw + 0x4BC, 0, 0xBF0);
        if (res.val == 0) {
            i = 0;
            off = 0;
            do {
                u8 *p = cw + off;
                cnLBS_Get_RoomMemberList((u16)i, p + 0x73C, p + 0x744, p + 0x756);
                i++;
                off += 0x2FC;
            } while (i < 4);
        }
    }
}
