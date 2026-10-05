/* cnlbs, run 4: __cnet_Send_ConditionSearchUserCertify .. __cnet_Send_ConditionSearchUserCertify (lobby.bin 0x005A3180-0x005A31E8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

int __cnet_Send_ConditionSearchUserCertify(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xEE) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
