/* cnlbs, run 40: cnLBS_LogoutLobbyServer .. _cnet_RecvFromLbs_NoticeLobbyFull (lobby.bin 0x005AD7E0-0x005ADB58): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_LogoutLobbyServer(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_Logout();
        return slot;
    }
    return -1;
}

int __cnet_SendSet_Logout(void) {
    int cmd = SetSendCommand(&send_work, 2) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerLogOut(void) {
    CNET_RES res;

    if (CnetSys_w.rcat == 2) {
        if (CnetSys_w.rres == 0) {
            res.val = 0;
        } else {
            res.val = -1;
            __cnet_Recv_ServerMessage(CnetSys_w.rcat);
        }
        __cnetSub_Return_BgProcess(res, 1, 0);
    }
}

int cnLBS_ShutDownLobbyServer(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_ShutDown();
        return slot;
    }
    return -1;
}

int __cnet_SendSet_ShutDown(void) {
    int cmd = SetSendCommand(&send_work, 4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerShutDown(void) {
    CNET_RES res;

    if (CnetSys_w.rcat == 2) {
        if (CnetSys_w.rres == 0) {
            res.val = 0;
        } else {
            res.val = -1;
            __cnet_Recv_ServerMessage(CnetSys_w.rcat);
        }
        __cnetSub_Return_BgProcess(res, 1, 0);
    }
}

int cnLBS_Get_ServerMessage(char *d) {
    strcpy(d, CNWP(0x378C0));
    return 0;
}

void _cnet_RecvFromLbs_RequestLineCheck(void) {
    __cnet_SendSet_LineCheck();
    CNW(s16, 0x10) = 1;
}

void __cnet_SendSet_LineCheck(void) {
    SetSendCommand(&send_work, 1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeShutDown(void) {
    __cnet_Recv_ServerMessage();
    _cnetEvent_JumpCallBack(2, 0);
}

void _cnet_RecvFromLbs_NoticeShutDownOpponent(void) {
    _cnetEvent_JumpCallBack(6, 0);
}

void _cnet_RecvFromLbs_NoticeMatchCancel(void) {
    __cnet_Recv_ServerMessage();
    _cnetEvent_JumpCallBack(7, 0);
}

void _cnet_RecvFromLbs_NoticeLobbyFull(void) {
    __cnet_Recv_ServerMessage();
    _cnetEvent_JumpCallBack(8, 0);
}
