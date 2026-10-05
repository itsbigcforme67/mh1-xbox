/* cnlbs, run 45: cnLBS_Send_UserMiniData .. _cnet_RecvFromLbs_AnswerMiniDataRegist (lobby.bin 0x005AA790-0x005AA818): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Send_UserMiniData(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_MiniDataRegist(arg0, arg1);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerMiniDataRegist(void) {
    _cnet_Return_CallBack(0);
}
