/* lb_by95 - agent B promoted near-match 0x005C0680-0x005C079C: net_ToNetworkLobby (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { u8 pad0[0x2C0E]; s8 x2C0E; u8 pad2C0F[0x22]; s8 x2C31; s8 x2C32; s8 x2C33; s8 x2C34; s8 x2C35; s8 x2C36; s8 x2C37; s8 x2C38; u8 pad2C39[0xB]; s8 x2C44; u8 pad2C45[0x339]; u8 x2F7E; u8 pad2F7F[0x656]; s8 x35D5; } CWS_nn;
extern u8 RecvMailInfo[];
extern s8 BsLbsErrNum;
#define CWX ((CWS_nn *)cw)

void net_ToNetworkLobby(void) {
    int i;
    u8 *p;

    pNet = network_work;
    cw = client_work;
    CWX->x2C31 = 0;
    CWX->x2C32 = 0;
    CWX->x2C33 = 0;
    CWX->x2C34 = 0;
    CWX->x2C35 = 0;
    CWX->x2C36 = 0;
    CWX->x2C37 = 0;
    CWX->x2C38 = 0;
    cnLbc_SetIspRestTime(cw + 0x35F8);
    *(s32 *)(cw + 0x35F4) = 0xE10;
    CWX->x2C0E = 0;
    CWX->x2C44 = 0;
    CWX->x35D5 = 0;
    Lb_clearChatList();
    BsLbsErrNum = 0;
    cnLBS_Init_LoginLobbyServer();
    i = 0;
    p = RecvMailInfo;
    CWX->x2F7E = 0;
    do {
        if (*(s8 *)(p + 1) != 0) {
            CWX->x2F7E = CWX->x2F7E + 1;
        }
        i++;
        p += 0x9A;
    } while (i < 8);
    memset(cw + 0x3019, 0, 0x9A);
    memset(cw + 0x2F7F, 0, 0x9A);
    Lb_clearChatList();
}
