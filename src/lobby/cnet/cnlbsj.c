/* cnlbs, run 10: _cnet_RecvFromLbs_MatchBattleCode .. cnLBS_Get_MatchInfomation (lobby.bin 0x005AC690-0x005ACB84): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

void _cnet_RecvFromLbs_MatchBattleCode(void) {
    CNET_RES res;
    u8 *p;

    if (CnetSys_w.burst[7].state != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                GetRecvDataString(CNWP(0x30312), recv_work);
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        __cnet_SendReq_MatchGameRule();
    }
}

void _cnet_RecvFromLbs_MatchGameRule(void) {
    CNET_RES res;
    u8 *p;

    if (CnetSys_w.burst[7].state != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                GetRecvDataString(CNWP(0x30323), recv_work);
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        __cnet_SendReq_MatchMcsIpAddr();
    }
}

void _cnet_RecvFromLbs_MatchGameServerAddr(void) {
    CNET_RES res;

    if (CNW(u8, 0xF34) != 0) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                res.val = 0;
                GetRecvDataString(CNWP(0x30308), GetRecvDataString(CNWP(0x30300), recv_work));
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        res.val = 0;
        __cnet_Return_MatchInformation(res);
    }
}

void __cnet_Return_MatchInformation(CNET_RES res) {
    if (res.val == -1) {
        __cnet_SendReq_MatchRejection(res.val);
    }
    if (CnetSys_w.burst[7].cb != 0) {
        CnetSys_w.burst[7].state = 0;
        CnetSys_w.burst[7].x21 = 0;
        CnetSys_w.burst[7].cb(res, &res);
    }
}

int __cnet_SendReq_MatchJoin(void) {
    int cmd = SetSendCommand(&send_work, 0xA3) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void __cnet_SendReq_MatchPlSide(int arg0) {
    SetSendCommand(&send_work, 0xA5);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchOpponentInfo(int arg0) {
    SetSendCommand(&send_work, 0xA9);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchOpponentStatus(int arg0) {
    SetSendCommand(&send_work, 0xAB);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchGameRule(void) {
    SetSendCommand(&send_work, 0xA7);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchBattleCode(void) {
    SetSendCommand(&send_work, 0xAE);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_MatchMcsIpAddr(void) {
    SetSendCommand(&send_work, 0xB0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int __cnet_SendReq_MatchRejection(void) {
    int cmd = SetSendCommand(&send_work, 0xAD) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Get_MatchInfomation(CNET_W5D4 *d) {
    *d = CnetSys_w.matchinfo;
    return 0;
}
