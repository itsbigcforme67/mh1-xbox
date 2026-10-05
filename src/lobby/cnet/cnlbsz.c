/* cnlbs, run 26: _cnet_RecvFromLbs_AnswerCurrentPlace .. cnLBS_Set_LoginFirstData (lobby.bin 0x005AA040-0x005AA308): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_AnswerCurrentPlace(void) {
    if (CNW(s8, 0xFEC) == 0) {
        GetRecvData16(CNWP(0x3BA5C), GetRecvData16(CNWP(0x3BA5A), GetRecvData16(CNWP(0x3BA58), &recv_work)));
    }
    _cnet_Return_CallBack(0);
}

int __cnet_SendReq_CurrentPlace(void) {
    int cmd = SetSendCommand(&send_work, 0xD6) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void cnLBS_Init_LoginLobbyServer(void) {
    *(int *)((u8 *)CNWP(8)) = 0;
    *(int *)((u8 *)CNWP(0)) = 0;
    memset(CNWP(0x18), 0, 0xE00);
    memset(CNWP(0xE18), 0, 0x1B0);
    memset(CNWP(0x4058), 0, 0xA);
    memset(CNWP(0x1436), 0, 0x28);
}

int cnLBS_LoginLobbyServer(CNET_LOGIN cfg, int cb) {
    if (CnetSys_w.burst[0].state == 0) {
        CnetSys_w.login = cfg;
        CnetSys_w.x14 = 0;
        CnetSys_w.active = 1;
        CnetSys_w.echo_n = 0;
        CnetSys_w.echo_sum = 0;
        memset(&CnetSys_w.acct, 0, 0x5C);
        memset(CnetSys_w.login_users, 0, 0x170);
        CnetSys_w.xff0 = 0;
        CnetSys_w.xff4 = 0x1000;
        CnetSys_w.xff8 = CnetSys_w.loginbuf.b;
        memset(&CnetSys_w.warnmsg, 0, 0x1004);
        memset(&CnetSys_w.loginbuf, 0, 0x2000);
        CnetSys_w.burst[0].cb = (void *)cb;
        CnetSys_w.burst[0].state = 1;
        CnetSys_w.burst[0].x21 = 0;
        CnetSys_w.burst[0].run = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Set_LoginFirstData(CNET_FIRSTDATA *src) {
    CnetSys_w.firstdata = *src;
    return 0;
}
