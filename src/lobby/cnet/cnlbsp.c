/* cnlbs, run 16: _cnet_RecvFromLbs_AnswerRoomSetName .. _cnet_CallBack_Result_Plaza_PlazaStatus_005A6E60 (lobby.bin 0x005A6CB0-0x005A6EC4): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_AnswerRoomSetName(void) {

}

void _cnet_RecvFromLbs_AnswerRoomSetRule(void) {

}

void _cnet_RecvFromLbs_AnswerRoomSetFinish(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_BothRoomExit(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticePlazaRemove(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ServerMessage();
    }
    _cnetEvent_JumpCallBack(9, 0);
}

void _cnet_RecvFromLbs_NoticeLobbyRemove(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ServerMessage();
    }
    _cnetEvent_JumpCallBack(0xA, 0);
}

void _cnet_RecvFromLbs_NoticeRoomRemove(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_ServerMessage();
    }
    _cnetEvent_JumpCallBack(0xB, 0);
}

void _cnet_RecvFromLbs_AnswerRoomRestTime(void) {
    u16 t;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_WordWord(&t, CNWP(0x302FE));
    }
    _cnet_Return_CallBack(0);
}

void __cnet_CallBack_Result_Plaza_NumOfPlaza_005A6E20(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[2].res = 1;
        return;
    }
    CnetSys_w.burst[2].res = 2;
}

void _cnet_CallBack_Result_Plaza_PlazaStatus_005A6E60(CNET_RES res) {
    CNET_RES r;

    if (res.val == 0) {
        r.val = 2;
        r.id = 0xB;
        CnetSys_w.burst[2].cb(r, &r);
        CnetSys_w.burst[2].res = 1;
        return;
    }
    CnetSys_w.burst[2].res = 2;
}
