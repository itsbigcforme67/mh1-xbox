/* cnlbs, run 38: __cnet_SendReq_MatchEntryUser .. __cnet_SendReq_MatchEntryUser (lobby.bin 0x005A9A80-0x005A9AE8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int __cnet_SendReq_MatchEntryUser(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x9D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
