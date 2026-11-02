/* cnlbs, run 6: cnLBS_Get_LoginWarningMessage .. __cnet_SendReq_TopInformation (lobby.bin 0x005AAA40-0x005AACCC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

typedef struct { s8 val; u8 pad[6]; } R7;

#if !defined(G_PART) || G_PART == 1
void cnLBS_Get_LoginWarningMessage(CNET_H1004 *d) {
    *d = CnetSys_w.warnmsg;
}
#endif

#if !defined(G_PART) || G_PART == 1
int cnLBS_Answer_LoginWarningMessage(void) {
    __cnet_SendAns_WarningMessage();
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 3
void __cnet_SendAns_WarningMessage(int arg0) {
    SetSendCommand(&send_work, MH2V(GV(0x14, 0x12), 0x12));
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}
#endif

#if !defined(G_PART) || G_PART == 2
int cnLBS_Send_LoginFinish(void) {
    __cnet_SendSet_LoginFinish();
    return 0;
}
#endif

#if !defined(G_PART)
void _cnet_RecvFromLbs_AnswerBillEstimate(void) {

}
#endif

#if !defined(G_PART) || G_PART == 2
void _cnet_RecvFromLbs_AnswerUserBinary(void) {
    _cnet_Return_CallBack(0);
}
#endif

#if !defined(G_PART) || G_PART == 2
int cnLBS_Read_TopInformation(cb)
int cb;
{
    int slot = __cnetSub_Set_BgProcess(1, 0, cb);

    CnetSys_w.xff0 = 0;
    CnetSys_w.xff4 = 0x1000;
    CnetSys_w.xff8 = CnetSys_w.loginbuf.b;
    memset(&CnetSys_w.topinfo, 0, 0x1004);
    memset(&CnetSys_w.loginbuf, 0, 0x2000);
    CnetSys_w.xff0 = 0;
    CnetSys_w.xff4 = 0x1000;
    CnetSys_w.xff8 = CnetSys_w.loginbuf.b;
    memset(&CnetSys_w.topinfo, 0, 0x1004);
    memset(&CnetSys_w.loginbuf, 0, 0x2000);
    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_TopInformation();
        return slot;
    }
    return -1;
}
#endif

#if !defined(G_PART) || G_PART == 2
int cnLBS_Get_TopInformation(CNET_B1004 *d) {
    *d = CnetSys_w.topinfo;
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 4
int __cnet_SendReq_TopInformation(void) {
    int cmd = SetSendCommand(&send_work, MH2V(GV(0x1F, 0x1B), 0x1B)) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
#endif
