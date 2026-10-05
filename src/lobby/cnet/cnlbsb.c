/* cnlbs, run 2: _cnet_RecvFromLbs_RequestAdminMessage .. cnLBS_Get_ConditionSearchUser (lobby.bin 0x005A2E50-0x005A2FA4): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void _cnet_RecvFromLbs_RequestAdminMessage(void) {
    GetRecvDataString(CNWP(0x30900), GetRecvDataString(CNWP(0x308EC), &recv_work));
    _cnetEvent_JumpCallBack(4, 0);
}

void cnLBS_AnswerAdminMessage(void) {
    SetSendCommand(&send_work, 0xF5);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int cnLBS_ConditionSearchUser(cond, cb)
CNET_COND *cond;
int cb;
{
    CNET_COND c;
    int slot;

    c = *cond;
    memset(&CnetSys_w.csearch, 0, 0x1CC4);
    slot = __cnetSub_Set_BgProcess(1, 0, cb);
    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_ConditionSearchUser(c);
        CnetSys_w.cs_slot = slot;
        return slot;
    }
    return -1;
}

int cnLBS_Get_ConditionSearchUser(void **arg0) {
    *arg0 = CNWP(0x39D8C);
    return 0;
}
