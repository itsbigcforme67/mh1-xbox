/* cnlbs, run 29: cnLBS_Get_LoginWarningMessage .. _cnet_RecvFromLbs_AnswerUserBinary (lobby.bin 0x005AAA40-0x005AAB38): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void cnLBS_Get_LoginWarningMessage(CNET_H1004 *d) {
    *d = CnetSys_w.warnmsg;
}

int cnLBS_Answer_LoginWarningMessage(void) {
    __cnet_SendAns_WarningMessage();
    return 0;
}

void __cnet_SendAns_WarningMessage(int arg0) {
    SetSendCommand(&send_work, 0x14);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int cnLBS_Send_LoginFinish(void) {
    __cnet_SendSet_LoginFinish();
    return 0;
}

void _cnet_RecvFromLbs_AnswerBillEstimate(void) {

}

void _cnet_RecvFromLbs_AnswerUserBinary(void) {
    _cnet_Return_CallBack(0);
}
