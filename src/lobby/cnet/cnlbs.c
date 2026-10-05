/* cnlbs, run 1: _cnet_RecvFromLbs_NoticeMailMessage .. __cnet_SendReq_SearchUser (lobby.bin 0x005A2A20-0x005A2DBC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_NoticeMailMessage(void) {
    __cnet_Recv_MailMessage();
    _cnetEvent_JumpCallBack(3, 0);
}

int cnLBS_SendMessage(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_SendMail(arg0, arg1);
        return slot;
    }
    return -1;
}

void cnLBS_Get_RecvMessage(char *a, char *b, char *c) {
    if (a != 0) {
        strcpy(a, CNWP(0x308E4));
    }
    if (b != 0) {
        strcpy(b, CNWP(0x308EC));
    }
    if (c != 0) {
        strcpy(c, CNWP(0x30900));
    }
}

void _cnet_RecvFromLbs_AnswerSendMail(void) {
    _cnet_Return_CallBack(0);
}

int __cnet_SendReq_SendMail(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0xF1) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, strlen(arg1) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void __cnet_Recv_MailMessage(void) {
    GetRecvDataString(CNWP(0x30900), GetRecvDataString(CNWP(0x308EC), GetRecvDataString(CNWP(0x308E4), &recv_work)));
}

int cnLBS_SerchUserPlace(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_SearchUser(arg0);
        return slot;
    }
    return -1;
}

void cnLBS_Get_SerchUserPlace(u16 *arg0, u16 *arg1, u16 *arg2, u8 *arg3) {
    *arg0 = CNW(u16, 0x30980);
    *arg1 = CNW(u16, 0x30982);
    *arg2 = CNW(u16, 0x30984);
    *arg3 = CNW(u8, 0x30987);
}

void cnLBS_Get_SerchUserPlaceMessage(char *d) {
    strcpy(d, CNWP(0x30988));
}

void _cnet_RecvFromLbs_AnswerSearchUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_SearchUser();
    }
    _cnet_Return_CallBack(0);
}

int __cnet_SendReq_SearchUser(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xEA) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}
