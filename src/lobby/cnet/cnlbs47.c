/* cnlbs, run 48: __cnet_SendReq_TopInformation .. __cnet_SendReq_TopInformation (lobby.bin 0x005AAC80-0x005AACCC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int __cnet_SendReq_TopInformation(void) {
    int cmd = SetSendCommand(&send_work, 0x1F) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
