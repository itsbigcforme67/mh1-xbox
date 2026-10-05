/* cnlbs, run 11: cnLBS_Read_PlazaExplain .. cnLBS_Read_PlazaExplain (lobby.bin 0x005A3FF0-0x005A4064): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Read_PlazaExplain(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExplain(0, arg0);
        return slot;
    }
    return -1;
}
