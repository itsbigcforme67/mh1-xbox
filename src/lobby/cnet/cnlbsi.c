/* cnlbs, run 9: cnLBS_Read_PlazaJoinUser .. cnLBS_Read_PlazaJoinUser (lobby.bin 0x005A3DB0-0x005A3E24): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Read_PlazaJoinUser(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceJoinUser(0, arg0);
        return slot;
    }
    return -1;
}
