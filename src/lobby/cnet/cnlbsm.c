/* cnlbs, run 13: cnLBS_Read_LobbyJoinUser .. cnLBS_Read_LobbyJoinUser (lobby.bin 0x005A4540-0x005A45B4): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Read_LobbyJoinUser(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceJoinUser(1, arg0);
        return slot;
    }
    return -1;
}
