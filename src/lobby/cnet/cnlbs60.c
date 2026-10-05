/* cnlbs, run 61: cnLBS_LogoutLobbyServer .. __cnet_SendSet_Logout (lobby.bin 0x005AD7E0-0x005AD89C): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_LogoutLobbyServer(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_Logout();
        return slot;
    }
    return -1;
}

int __cnet_SendSet_Logout(void) {
    int cmd = SetSendCommand(&send_work, 2) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
