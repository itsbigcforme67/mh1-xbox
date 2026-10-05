/* cnlbs, run 37: _cnet_RecvFromLbs_RequestRegurationVersion .. _cnetEvent_JumpCallBack (lobby.bin 0x005AD0F0-0x005AD1AC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_RequestRegurationVersion(void) {

}

void _cnet_RecvFromLbs_NoticeRegurationAddress(void) {

}

void _cnet_RecvFromLbs_AnswerRegurationData(void) {
    _cnet_RecvFromLbs_AnswerBrowserMethodGet();
}

void cnLBS_Send_RegurationAgree(void) {

}

void _cnet_RecvFromLbs_AnswerRegurationAgree(void) {

}

void cnLBS_Set_CallBackNoticeEvent(int idx, void (*fn)()) {
    pFunc[idx] = fn;
}

void _cnetEvent_JumpCallBack(idx)
int idx;
{
    CNET_RES r;
    void (*fn)();

    r.id = idx;
    r.val = 1;
    fn = pFunc[(u16)idx];
    if (fn != 0) fn(r, 0);
}
