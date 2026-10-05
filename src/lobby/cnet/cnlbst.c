/* cnlbs, run 20: cnLBS_Read_RoomJoinInfo .. cnLBS_Read_RoomJoinInfo (lobby.bin 0x005A5070-0x005A50E4): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Read_RoomJoinInfo(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomJoinInfo(arg0);
        return slot;
    }
    return -1;
}
