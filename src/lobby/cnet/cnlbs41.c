/* cnlbs, run 42: cnLBS_ShutDownLobbyServer .. __cnet_SendSet_ShutDown (lobby.bin 0x005AD900-0x005AD9BC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_ShutDownLobbyServer(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_ShutDown();
        return slot;
    }
    return -1;
}

int __cnet_SendSet_ShutDown(void) {
    int cmd = SetSendCommand(&send_work, 4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
