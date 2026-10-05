/* cnlbs, run 11: _cnet_RecvFromLbs_NoticeUserId .. _cnet_RecvFromLbs_NoticeLoginOk (lobby.bin 0x005AA690-0x005AA8FC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void _cnet_RecvFromLbs_NoticeUserId(void) {
    CNET_RES res;

    if (CnetSys_w.burst[0].state != 0) {
        __cnet_Recv_UserIDandHandle();
        res.val = 0;
        res.id = 1;
        CnetSys_w.burst[0].cb(res, &res);
    }
}

void _cnet_RecvFromLbs_AnswerUserId(void) {
    CNET_RES res;

    if (CnetSys_w.burst[0].state != 0) {
        if (CnetSys_w.rres == 0) {
            __cnet_Recv_UserID();
            cnetGet_Login_DecideUserID(CnetSys_w.decide_id);
            cnetGet_Login_DecideUserHandle(CnetSys_w.decide_handle);
            res.val = 0;
            res.id = 2;
            CnetSys_w.burst[0].cb(res, &res);
            return;
        }
        __cnet_Recv_ServerMessage();
        res.val = -1;
        res.id = 7;
        CnetSys_w.burst[0].cb(res, &res);
    }
}

int cnLBS_Send_UserMiniData(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_MiniDataRegist(arg0, arg1);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerMiniDataRegist(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_Get_NoticeUserMiniData(CNET_B5C *d) {
    *d = CnetSys_w.minidata;
    return 0;
}

void _cnet_RecvFromLbs_NoticeMiniData(void) {
    if (CnetSys_w.rres == 0) {
        memset(&CnetSys_w.minidata, 0, 0x5C);
        GetRecvDataOption3(&CnetSys_w.minidata.b[0x1C], 0x40, GetRecvDataOption3(&CnetSys_w.minidata, 8, recv_work));
    }
    _cnet_Return_CallBack(0x2C);
}

void _cnet_RecvFromLbs_NoticeLoginOk(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Login_Return();
    }
}
