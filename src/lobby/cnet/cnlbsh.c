/* cnlbs, run 8: cnLBS_Get_AllocationProgressCount .. _cnet_CallBack_Result_RoomRuleCaption (lobby.bin 0x005A7B00-0x005A7B98): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

int cnLBS_Get_AllocationProgressCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x1032);
    return 0;
}

void _cnet_CallBack_Result_Rule_NumOfRule(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[5].res = 1;
        return;
    }
    CnetSys_w.burst[5].res = 2;
}

void _cnet_CallBack_Result_RoomRuleCaption(void) {
    CNET_RES r;

    r.val = 2;
    r.id = 0xB;
    CnetSys_w.burst[5].cb(r, &r);
}
