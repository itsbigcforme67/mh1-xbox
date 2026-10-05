/* cnlbs, run 51: __cnet_Recv_UserID .. cnLBS_Send_ChatMessage (lobby.bin 0x005AB8D0-0x005AB914): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int __cnet_Recv_UserID(void) {
    GetRecvDataOption3(CNWP(0x1576), 8, &recv_work);
    return 0;
}

void cnLBS_Send_ChatMessage(int a, int b) {
    __cnet_SendSet_ChatMessage(0, a, b);
}
