/* cnlbs, run 21: __cnet_SendReq_RoomJoinInfo .. __cnet_SendReq_RoomJoinInfo (lobby.bin 0x005A5170-0x005A51D8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int __cnet_SendReq_RoomJoinInfo(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x8D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
