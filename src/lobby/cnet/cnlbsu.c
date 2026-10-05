/* cnlbs, run 21: __cnet_SendReq_PieceCount .. __cnet_SendReq_PieceName (lobby.bin 0x005A85D0-0x005A8748): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

int __cnet_SendReq_PieceCount(kind)
int kind;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x32) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x46) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x7B) & 0xFFFF;
        break;
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PieceName(kind, arg1)
int kind;
int arg1;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x34) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x48) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x7D) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
