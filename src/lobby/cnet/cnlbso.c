/* cnlbs, run 15: cnLBS_Read_LobbyExplain .. cnLBS_Read_LobbyExplain (lobby.bin 0x005A4780-0x005A47F4): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Read_LobbyExplain(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExplain(1, arg0);
        return slot;
    }
    return -1;
}
