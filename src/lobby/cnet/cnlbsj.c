/* cnlbs, run 10: _cnet_RecvFromLbs_AnswerRoomCreate .. cnLBS_Set_RoomRuleFinish (lobby.bin 0x005A5320-0x005A5448): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void _cnet_RecvFromLbs_AnswerRoomCreate(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Set_RoomRule(rule, cb)
CNET_RULE *rule;
int cb;
{
    CnetSys_w.rule = *rule;
    if (CnetSys_w.burst[6].state == 0) {
        CnetSys_w.burst[6].cb = (void *)cb;
        CnetSys_w.burst[6].state = 1;
        CnetSys_w.burst[6].run = __cnet_bgProg_RoomSetRule;
        CnetSys_w.burst[6].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Set_RoomRuleFinish(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomSetFinish();
        return slot;
    }
    return -1;
}
