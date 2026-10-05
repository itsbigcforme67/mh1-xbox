/* cnlbs, run 9: _cnet_RecvFromLbs_AnswerCurrentPlace .. _cnet_RecvFromLbs_NoticeLoginOk (lobby.bin 0x005AA040-0x005AA8FC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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

int cnLBS_Send_LoginUserAccount(id, handle, mini)
char *id;
char *handle;
void *mini;
{
    int n;

    memset(&CnetSys_w.acct, 0, 0x5C);
    strncpy(&CnetSys_w.acct.b[8], handle, strlen(handle));
    memcpy(&CnetSys_w.acct.b[0x1C], mini, 0x40);
    if (id == 0) {
        strncpy(CnetSys_w.login_users[3].id, "******", 6);
        memset(&CnetSys_w.acct, 0, 8);
    } else {
        strncpy(CnetSys_w.login_users[3].id, id, 6);
        strncpy(&CnetSys_w.acct, id, 6);
    }
    n = strlen(handle);
    strncpy(CnetSys_w.login_users[3].handle, handle, n);
    CnetSys_w.login_users[3].handle[n] = 0;
    __cnet_SendReq_UserID();
    return 0;
}

u8 cnetGet_Login_NoOfUserAccount(void) {
    return CNW(u8, 0x145E);
}

int cnetGet_Login_UserID(idx, d)
int idx;
char *d;
{
    int k = idx & 0xFF;

    strcpy(d, (u8 *)&CnetSys_w + k * 0x5C + 0x1462);
    return 0;
}

int cnetGet_Login_UserHandle(idx, d)
int idx;
char *d;
{
    int k = idx & 0xFF;

    strcpy(d, (u8 *)&CnetSys_w + k * 0x5C + 0x146A);
    return 0;
}

int cnetGet_Login_UserMiniData(idx, d)
int idx;
void *d;
{
    int k = idx & 0xFF;

    memcpy(d, (u8 *)&CnetSys_w + k * 0x5C + 0x147E, 0x40);
    return 0;
}

int cnetGet_Login_DecideUserID(char *d) {
    strcpy(d, CNWP(0x1576));
    return 0;
}

int cnetGet_Login_DecideUserHandle(char *d) {
    strcpy(d, CNWP(0x157E));
    return 0;
}

void _cnet_RecvFromLbs_RequestConnectionPair(void) {
    GetRecvData16(CNWP(0xFEE), &recv_work);
    __cnet_SendSet_ConnectionPair();
}

void _cnet_RecvFromLbs_RequestFirstData(void) {
    if (CNW(u8, 0x1034) != 0) {
        __cnet_SendReq_EchoPacket();
        return;
    }
    __cnet_SendSet_FirstData();
}

void _cnet_RecvFromLbs_AnswerEchoPacket(void) {
    int t = (CnetSys_w.rcnt * 0x10) & 0xFFFF;

    CnetSys_w.echo_sum += t;
    CnetSys_w.echo_n++;
    if (CnetSys_w.echo_n < 4) {
        __cnet_SendReq_EchoPacket(t);
        return;
    }
    CnetSys_w.firstdata.h[14] = (u32)CnetSys_w.echo_sum >> 2;
    __cnet_SendSet_FirstData(t);
}

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
