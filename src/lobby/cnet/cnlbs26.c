/* cnlbs, run 27: cnLBS_Get_TopInformation .. __cnet_SendReq_TopInformation (lobby.bin 0x005AAC40-0x005AACCC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

int cnLBS_Get_TopInformation(CNET_B1004 *d) {
    *d = CnetSys_w.topinfo;
    return 0;
}

int __cnet_SendReq_TopInformation(void) {
    int cmd = SetSendCommand(&send_work, 0x1F) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
