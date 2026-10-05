/* cnlbs, run 11: cnLBS_Set_RoomRuleFinish .. cnLBS_Set_RoomRuleFinish (lobby.bin 0x005A53E0-0x005A5448): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Set_RoomRuleFinish(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomSetFinish();
        return slot;
    }
    return -1;
}
