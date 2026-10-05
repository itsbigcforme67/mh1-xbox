/* cnlbs, run 26: _cnet_RecvFromLbs_AnswerCurrentPlace .. cnLBS_Init_LoginLobbyServer (lobby.bin 0x005AA040-0x005AA164): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_AnswerCurrentPlace(void) {
    if (CNW(s8, 0xFEC) == 0) {
        GetRecvData16(CNWP(0x3BA5C), GetRecvData16(CNWP(0x3BA5A), GetRecvData16(CNWP(0x3BA58), &recv_work)));
    }
    _cnet_Return_CallBack(0);
}

int __cnet_SendReq_CurrentPlace(void) {
    int cmd = SetSendCommand(&send_work, 0xD6) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void cnLBS_Init_LoginLobbyServer(void) {
    *(int *)((u8 *)CNWP(8)) = 0;
    *(int *)((u8 *)CNWP(0)) = 0;
    memset(CNWP(0x18), 0, 0xE00);
    memset(CNWP(0xE18), 0, 0x1B0);
    memset(CNWP(0x4058), 0, 0xA);
    memset(CNWP(0x1436), 0, 0x28);
}
