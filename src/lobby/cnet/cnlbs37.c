/* cnlbs, run 38: cnLBS_LogoutLobbyServer .. GetRecvData32 (lobby.bin 0x005AD7E0-0x005ADBB8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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

int GetRecvData8(dst, src)
u8 *dst;
u8 *src;
{
    *dst = *src;
    return (int)(src + 1);
}

int GetRecvData16(dst, src)
u8 *dst;
u8 *src;
{
    dst[1] = src[0];
    dst[0] = src[1];
    return (int)(src + 2);
}

int GetRecvData32(dst, src)
u8 *dst;
u8 *src;
{
    dst[3] = src[0];
    dst[2] = src[1];
    dst[1] = src[2];
    dst[0] = src[3];
    return (int)(src + 4);
}
