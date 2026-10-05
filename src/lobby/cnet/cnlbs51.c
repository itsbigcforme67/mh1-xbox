/* cnlbs, run 52: __cnet_SendSet_ChatMessage .. cnLBS_Send_ChatBinary (lobby.bin 0x005AB960-0x005ABAB8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void __cnet_SendSet_ChatMessage(int arg0, int arg1, int arg2) {
    SetSendCommand(&send_work, 0xE8);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeChatMessage(void) {
    memset(CnetSys_w.chat_from, 0, 0x120);
    GetRecvData8(&CnetSys_w.chat_d, GetRecvData8(&CnetSys_w.chat_c, GetRecvData8(&CnetSys_w.chat_b, GetRecvData8(&CnetSys_w.chat_a, GetRecvDataOption3(CnetSys_w.chat_msg, 0x100, GetRecvDataOption3(CnetSys_w.chat_x, 0x10, GetRecvDataOption3(CnetSys_w.chat_from, 8, recv_work)))))));
    _cnetEvent_JumpCallBack(5, 0);
}

void cnLBS_Send_ChatBinary(void) {
    __cnet_SendSet_ChatBinary();
}
