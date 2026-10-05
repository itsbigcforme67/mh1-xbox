/* cnlbs, run 3: __cnet_Send_ConditionSearchUserCertify .. _cnet_CallBack_Result_PersonalDataChange (lobby.bin 0x005A3180-0x005A3888): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

int __cnet_Send_ConditionSearchUserCertify(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xEE) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerConditionSearchUser(void) {
    u8 sp3F;
    u8 sp3E;
    u8 sp3D;
    u8 sp3C;
    int r;
    int i;
    CNET_B5C *p;

    if (CnetSys_w.rres == 0) {
        r = GetRecvData8(&sp3C, GetRecvData8(&sp3D, GetRecvData8(&sp3E, GetRecvData8(&sp3F, recv_work))));
        p = &CnetSys_w.csearch.rec[sp3E];
        for (i = 0; i < sp3D; i++) {
            r = GetRecvDataOption3(p->b + 0x1C, 0x40, GetRecvDataOption3(p->b + 8, 0x10, GetRecvDataOption3(p->b, 8, r)));
            p++;
        }
        if (sp3C == 0) {
            CnetSys_w.bg[CnetSys_w.cs_slot].cmd = __cnet_Send_ConditionSearchUserCertify((sp3D + sp3E) & 0xFF);
            return;
        }
        CnetSys_w.csearch.n = sp3F;
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_RegistPersonalData(pd, cb)
CNET_PDATA *pd;
int cb;
{
    CNET_PDATA tmp;

    tmp = *pd;
    if (CnetSys_w.burst[9].state == 0) {
        CnetSys_w.pdata = tmp;
        CnetSys_w.burst[9].cb = (void *)cb;
        CnetSys_w.burst[9].state = 1;
        CnetSys_w.burst[9].run = __cnet_bgProg_RegistPersonalData;
        CnetSys_w.burst[9].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_RequestPersonalDataChange(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PersonalDataChange();
        return slot;
    }
    return -1;
}

int __cnet_SendReq_PersonalDataChange(void) {
    int cmd = SetSendCommand(&send_work, 0xB2) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerPersonalDataChange(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_Send_PersonalData(cb)
int cb;
{
    int slot = __cnetSub_Set_BgProcess(1, 0, cb);

    if (slot != -1) {
        __cnet_SendSet_PersonalDataName();
        __cnet_SendSet_PersonalDataZip();
        __cnet_SendSet_PersonalDataAddress();
        __cnet_SendSet_PersonalDataTelephone();
        __cnet_SendSet_PersonalDataAge();
        __cnet_SendSet_PersonalDataMailAddress();
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PersonalDataRegisted();
        return slot;
    }
    return -1;
}

void __cnet_SendSet_PersonalDataName(void) {
    char *s0 = CnetSys_w.pdata.name;

    SetSendCommand(&send_work, 0xB4);
    SetSendEncodeStringData(&send_work, s0, strlen(s0) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataZip(void) {
    char *s0 = CnetSys_w.pdata.name;

    SetSendCommand(&send_work, 0xB5);
    SetSendEncodeStringData(&send_work, s0 + 0x41, strlen(s0 + 0x41) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataAddress(void) {
    char *s0 = CnetSys_w.pdata.name;

    SetSendCommand(&send_work, 0xB6);
    SetSendEncodeStringData(&send_work, s0 + 0x4C, strlen(s0 + 0x4C) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataTelephone(void) {
    char *s0 = CnetSys_w.pdata.name;

    SetSendCommand(&send_work, 0xB7);
    SetSendEncodeStringData(&send_work, s0 + 0xCD, strlen(s0 + 0xCD) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataAge(void) {
    SetSendCommand(&send_work, 0xB8);
    SetSendData8(&send_work, CnetSys_w.pdata.age);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_PersonalDataMailAddress(void) {
    char *s0 = CnetSys_w.pdata.name;

    SetSendCommand(&send_work, 0xB9);
    SetSendEncodeStringData(&send_work, s0 + 0x14F, strlen(s0 + 0x14F) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int __cnet_SendReq_PersonalDataRegisted(void) {
    int cmd = SetSendCommand(&send_work, 0xBA) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerPersonalDataRegisted(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_CallBack_Result_PersonalDataChange(CNET_RES res) {
    if (res.val == 0) {
        CNW(s8, 0xF7E) = 1;
        return;
    }
    CNW(s8, 0xF7E) = 2;
}
