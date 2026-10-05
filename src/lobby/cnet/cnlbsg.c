/* cnlbs, run 7: __cnet_SendReq_PersonalDataRegisted .. _cnet_RecvFromLbs_AnswerPersonalDataRegisted (lobby.bin 0x005A37F0-0x005A3848): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int __cnet_SendReq_PersonalDataRegisted(void) {
    int cmd = SetSendCommand(&send_work, 0xBA) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerPersonalDataRegisted(void) {
    _cnet_Return_CallBack(0);
}
