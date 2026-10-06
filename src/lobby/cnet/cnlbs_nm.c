/* cnlbs - lobby.bin network layer 0x005A2A20-0x005AE320: the protocol layer of the online lobby client.
 * cnLBS_* start a request, __cnet_SendReq_* build the packet, _cnet_RecvFromLbs_* handle replies. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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

void __cnet_Recv_SearchUser(void) {
    char sp10[9];

    GetRecvDataString(CNWP(0x30988), GetRecvData8(CNWP(0x30987), GetRecvData8(CNWP(0x30986), GetRecvData16(CNWP(0x30984), GetRecvData16(CNWP(0x30982), GetRecvData16(CNWP(0x30980), GetRecvDataString(&sp10, recv_work)))))));
}

void _cnet_RecvFromLbs_RequestAdminMessage(void) {
    GetRecvDataString(CNWP(0x30900), GetRecvDataString(CNWP(0x308EC), &recv_work));
    _cnetEvent_JumpCallBack(4, 0);
}

void cnLBS_AnswerAdminMessage(void) {
    SetSendCommand(&send_work, 0xF5);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int cnLBS_ConditionSearchUser(cond, cb)
CNET_COND *cond;
int cb;
{
    CNET_COND c;
    int slot;

    c = *cond;
    memset(&CnetSys_w.csearch, 0, 0x1CC4);
    slot = __cnetSub_Set_BgProcess(1, 0, cb);
    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_ConditionSearchUser(c);
        CnetSys_w.cs_slot = slot;
        return slot;
    }
    return -1;
}

int cnLBS_Get_ConditionSearchUser(void **arg0) {
    *arg0 = CNWP(0x39D8C);
    return 0;
}

int __cnet_SendReq_ConditionSearchUser(CNET_COND c) {
    int cmd;
    int n;
    int i;
    u8 *e;

    cmd = SetSendCommand(&send_work, 0xEC) & 0xFFFF;
    SetSendData8(&send_work, c.b[0]);
    n = c.b[1];
    SetSendData8(&send_work, n);
    n &= 0xFF;
    if (0 < n) {
        i = 0;
        e = c.b;
        do {
        int t = e[4];
        SetSendData8(&send_work, t);
        switch (t & 0xFF) {
        case 1:
            SetSendStringData2(&send_work, e + 8, 6);
            break;
        case 2:
            SetSendStringData2(&send_work, e + 8, e[5]);
            break;
        case 3:
            SetSendData8(&send_work, e[8]);
            break;
        case 4:
            SetSendData8(&send_work, e[8]);
            break;
        case 5:
            SetSendData8(&send_work, e[8]);
            break;
        case 6:
            SetSendData8(&send_work, e[8]);
            SetSendData8(&send_work, e[9]);
            break;
        }
            i++;
            e += 0x44;
        } while (i < n);
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

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

void __cnet_bgProg_RegistPersonalData(void) {
    CNET_BURST *b = &CnetSys_w.burst[9];
    CNET_RES res;

    if (b->state != 0) {
        switch (b->x21) {
        case 0:
            b->x21++;
            b->res = 0;
            cnLBS_RequestPersonalDataChange((int)_cnet_CallBack_Result_PersonalDataChange);
            break;
        case 1:
            if (b->res == 1) {
                b->x21++;
            } else if (b->res == 2) {
                b->x21 = 5;
            }
            break;
        case 2:
            b->x21++;
            b->res = 0;
            cnLBS_Send_PersonalData((int)_cnet_CallBack_Result_PersonalDataChange);
            break;
        case 3:
            if (b->res == 1) {
                b->x21++;
            } else if (b->res == 2) {
                b->x21 = 5;
            }
            break;
        case 4:
            res.val = 0;
            b->state = 0;
            b->x21 = 0;
            b->cb(res, &res);
            break;
        case 5:
            res.val = -1;
            b->state = 0;
            b->x21 = 0;
            b->cb(res, &res);
            break;
        }
    }
}

static void __cnet_KeepEntryFloorInfo(kind, val)
int kind;
s8 val;
{
    switch (kind & 0xFF) {
    case 0:
        CNW(s8, 0x4054) = val;
        break;
    case 1:
        CNW(s8, 0x4055) = val;
        break;
    case 2:
        CNW(s8, 0x4056) = val;
        break;
    }
}

void __cnet_ClearEntryFloorInfo(int kind) {
    switch (kind & 0xFF) {
    case 0:
        CNW(s8, 0x405E) = 0;
        break;
    case 1:
        CNW(s8, 0x405F) = 0;
        break;
    case 2:
        CNW(s8, 0x4060) = 0;
        break;
    }
}

void __cnet_SetEntryFloorInfo(int kind) {
    switch (kind & 0xFF) {
    case 0:
        CNW(u8, 0x405E) = CNW(u8, 0x4054);
        break;
    case 1:
        CNW(u8, 0x405F) = CNW(u8, 0x4055);
        break;
    case 2:
        CNW(u8, 0x4060) = CNW(u8, 0x4056);
        break;
    }
}

int cnLBS_Read_PlazaAllocation(int unused, int val, int cb) {
    if (CnetSys_w.burst[2].state == 0) {
        CnetSys_w.burst[2].cb = (void *)cb;
        CnetSys_w.burst[2].val = val & 0xF;
        CnetSys_w.burst[2].state = 1;
        CnetSys_w.burst[2].run = __cnet_bgProg_ReadPlazaAllocation;
        CnetSys_w.burst[2].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Read_PlazaCount(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceCount(0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_PlazaCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x404E);
    return 0;
}

void _cnet_RecvFromLbs_AnswerPlazaNumOfPlaza(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x404E));
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_PlazaName(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceName(0, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_PlazaName(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x4082);
    return 0;
}

void _cnet_RecvFromLbs_AnswerPlazaName(void) {
    char buf[0x4E];
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceName(&id, buf);
        CnetSys_w.plaza[id - 1].id = id;
        strcpy((u8 *)&CnetSys_w + ((id - 1) * 0x164) + 0x4082, buf);
        CnetSys_w.last_id = id;
        strcpy(CnetSys_w.last_name, buf);
        CnetSys_w.plaza[id - 1].flags |= 4;
    }
    _cnet_Return_CallBack(0xE);
}

int cnLBS_Read_PlazaJoinUser(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceJoinUser(0, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_mhPlazaJoinUser(idx, a, b)
int idx;
u16 *a;
u16 *b;
{
    *a = CnetSys_w.plaza[(u16)idx - 1].ja;
    *b = CnetSys_w.plaza[(u16)idx - 1].jb;
    return 0;
}

void _cnet_RecvFromLbs_BothPlazaJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x4064));
    }
    _cnet_Return_CallBack(0xF);
}

int cnLBS_Read_PlazaStatus(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceStatus(0, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_PlazaStatus(int idx, u8 *d) {
    *d = CnetSys_w.plaza[(u16)idx - 1].status;
    return 0;
}

void _cnet_RecvFromLbs_BothPlazaStatus(void) {
    u8 st;
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceStatus(&id, &st);
        CnetSys_w.plaza[id - 1].id = id;
        CnetSys_w.plaza[id - 1].status = st;
        CnetSys_w.last_id = id;
        CnetSys_w.last_status = st;
        CnetSys_w.plaza[id - 1].flags |= 2;
    }
    _cnet_Return_CallBack(0x10);
}

int cnLBS_Read_PlazaExplain(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExplain(0, arg0);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_BothPlazaExplain(void) {
    char buf[0x108];
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceExplain(&id, buf);
        CnetSys_w.plaza[id - 1].id = id;
        strcpy((u8 *)&CnetSys_w + ((id - 1) * 0x164) + 0x40C4, buf);
        CnetSys_w.plaza[id - 1].flags |= 8;
    }
    _cnet_Return_CallBack(0x11);
}

int cnLBS_PlazaEntry(arg0, arg1)
int arg0;
int arg1;
{
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);
    u16 cmd;

    if (slot != -1) {
        cmd = __cnet_SendReq_PieceEntry(0, arg0);
        __cnet_KeepEntryFloorInfo(0, arg0);
        CnetSys_w.bg[slot].cmd = cmd;
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerPlazaEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(0);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_PlazaExit(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExit(0);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerPlazaExit(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(0);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_LobbyAllocation(int unused, int val, int cb) {
    if (CnetSys_w.burst[3].state == 0) {
        CnetSys_w.burst[3].cb = (void *)cb;
        CnetSys_w.burst[3].val = val & 0xF;
        CnetSys_w.burst[3].state = 1;
        CnetSys_w.burst[3].run = __cnet_bgProg_ReadLobbyAllocation;
        CnetSys_w.burst[3].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Read_LobbyCount(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceCount(1);
        return slot;
    }
    return -1;
}

int cnLBS_Get_LobbyCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x4050);
    return 0;
}

void _cnet_RecvFromLbs_AnswerLobbyNumOfLobby(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x4050));
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_LobbyName(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceName(1, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_LobbyName(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x4E6A);
    return 0;
}

void _cnet_RecvFromLbs_AnswerLobbyName(void) {
    char buf[0x4E];
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceName(&id, buf);
        CnetSys_w.lobby[id - 1].id = id;
        strcpy((u8 *)&CnetSys_w + ((id - 1) * 0x164) + 0x4E6A, buf);
        CnetSys_w.last_id = id;
        strcpy(CnetSys_w.last_name, buf);
        CnetSys_w.lobby[id - 1].flags |= 4;
    }
    _cnet_Return_CallBack(0x13);
}

int cnLBS_Read_LobbyJoinUser(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceJoinUser(1, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_mhLobbyJoinUser(idx, a, b)
int idx;
u16 *a;
u16 *b;
{
    *a = CnetSys_w.lobby[(u16)idx - 1].ja;
    *b = CnetSys_w.lobby[(u16)idx - 1].jb;
    return 0;
}

void _cnet_RecvFromLbs_BothLobbyJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x4E4C));
    }
    _cnet_Return_CallBack(0x14);
}

int cnLBS_Read_LobbyStatus(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceStatus(1, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_LobbyStatus(int idx, u8 *d) {
    *d = CnetSys_w.lobby[(u16)idx - 1].status;
    return 0;
}

void _cnet_RecvFromLbs_BothLobbyStatus(void) {
    u8 st;
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceStatus(&id, &st);
        CnetSys_w.lobby[id - 1].id = id;
        CnetSys_w.lobby[id - 1].status = st;
        CnetSys_w.last_id = id;
        CnetSys_w.last_status = st;
        CnetSys_w.lobby[id - 1].flags |= 2;
    }
    _cnet_Return_CallBack(0x15);
}

int cnLBS_Read_LobbyExplain(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExplain(1, arg0);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerLobbyExplain(void) {
    char buf[0x108];
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceExplain(&id, buf);
        CnetSys_w.lobby[id - 1].id = id;
        strcpy((u8 *)&CnetSys_w + ((id - 1) * 0x164) + 0x4EAC, buf);
        CnetSys_w.lobby[id - 1].flags |= 8;
    }
    _cnet_Return_CallBack(0x16);
}

int cnLBS_LobbyEntry(arg0, arg1)
int arg0;
int arg1;
{
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);
    u16 cmd;

    if (slot != -1) {
        cmd = __cnet_SendReq_PieceEntry(1, arg0);
        __cnet_KeepEntryFloorInfo(1, arg0);
        CnetSys_w.bg[slot].cmd = cmd;
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerLobbyEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(1);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_LobbyExit(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExit(1);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerLobbyExit(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(1);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_RoomAllocation(int unused, int val, int cb) {
    if (CnetSys_w.burst[4].state == 0) {
        CnetSys_w.burst[4].cb = (void *)cb;
        CnetSys_w.burst[4].val = val & 0xFF;
        CnetSys_w.burst[4].state = 1;
        CnetSys_w.burst[4].run = __cnet_bgProg_ReadRoomAllocation;
        CnetSys_w.burst[4].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Read_RoomCount(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceCount(2);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x4052);
    return 0;
}

void _cnet_RecvFromLbs_AnswerRoomNumOfRoom(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_NumOfPiece(CNWP(0x4052));
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_RoomJoinUser(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceJoinUser(2, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_mhRoomJoinUser(idx, a, b)
int idx;
u16 *a;
u16 *b;
{
    *a = CnetSys_w.room[(u16)idx - 1].ja;
    *b = CnetSys_w.room[(u16)idx - 1].jb;
    return 0;
}

void _cnet_RecvFromLbs_BothRoomJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        _sub_ReceiveJoinUser(CNWP(0x61C4));
    }
    _cnet_Return_CallBack(0x19);
}

int cnLBS_Read_RoomStatus(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceStatus(2, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomStatus(int idx, u8 *d) {
    *d = CnetSys_w.room[(u16)idx - 1].status;
    return 0;
}

void _cnet_RecvFromLbs_BothRoomStatus(void) {
    u8 st;
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceStatus(&id, &st);
        CnetSys_w.room[id - 1].id = id;
        CnetSys_w.room[id - 1].status = st;
        CnetSys_w.last_id = id;
        CnetSys_w.last_status = st;
        CnetSys_w.room[id - 1].flags |= 2;
    }
    _cnet_Return_CallBack(0x1A);
}

int cnLBS_Read_RoomName(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceName(2, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomName(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x61E2);
    return 0;
}

void _cnet_RecvFromLbs_BothRoomName(void) {
    char buf[0x108];
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceName(&id, buf);
        CnetSys_w.room[id - 1].id = id;
        strcpy((u8 *)&CnetSys_w + ((id - 1) * 0x164) + 0x61E2, buf);
        CnetSys_w.last_id = id;
        strcpy(CnetSys_w.last_name, buf);
        CnetSys_w.room[id - 1].flags |= 4;
    }
    _cnet_Return_CallBack(0x18);
}

int cnLBS_Read_RoomExplain(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExplain(2, arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomExplain(idx, d)
int idx;
char *d;
{
    strcpy(d, (u8 *)&CnetSys_w + (((u16)idx - 1) * 0x164) + 0x6224);
    return 0;
}

void _cnet_RecvFromLbs_BothRoomExplain(void) {
    char buf[0x108];
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_PieceExplain(&id, buf);
        CnetSys_w.room[id - 1].id = id;
        strcpy((u8 *)&CnetSys_w + ((id - 1) * 0x164) + 0x6224, buf);
        CnetSys_w.room[id - 1].flags |= 8;
    }
    _cnet_Return_CallBack(0x1B);
}

int cnLBS_Read_RoomJoinInfo(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomJoinInfo(arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomJoinInfo(idx, a, b, c, d, e)
int idx;
u16 *a;
u16 *b;
u16 *c;
u16 *d;
u16 *e;
{
    *a = CnetSys_w.room[(u16)idx - 1].ri[0];
    *b = CnetSys_w.room[(u16)idx - 1].ri[1];
    *c = CnetSys_w.room[(u16)idx - 1].ri[2];
    *d = CnetSys_w.room[(u16)idx - 1].ri[3];
    *e = CnetSys_w.room[(u16)idx - 1].ri[4];
    return 0;
}

int __cnet_SendReq_RoomJoinInfo(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x8D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_BothRoomJoinInfo(void) {
    u16 id;
    CNET_PIECE *p;

    if (CnetSys_w.rres == 0) {
        int r = GetRecvData16(&id, recv_work);
        p = &CnetSys_w.room[id - 1];
        GetRecvData16(&p->ri[4], GetRecvData16(&p->ri[2], GetRecvData16(&p->ri[3], GetRecvData16(&p->ri[1], GetRecvData16(&p->ri[0], r)))));
        p->flags |= 0x20;
    }
    _cnet_Return_CallBack(0x1D);
}

int cnLBS_RoomCreate(arg0, arg1)
int arg0;
int arg1;
{
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);
    u16 cmd;

    if (slot != -1) {
        cmd = __cnet_SendReq_RoomCreate(arg0);
        __cnet_KeepEntryFloorInfo(2, arg0);
        CnetSys_w.bg[slot].cmd = cmd;
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerRoomCreate(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Set_RoomRule(rule, cb)
CNET_RULE *rule;
int cb;
{
    CnetSys_w.rule = *rule;
    if (CnetSys_w.burst[6].state == 0) {
        CnetSys_w.burst[6].cb = (void *)cb;
        CnetSys_w.burst[6].state = 1;
        CnetSys_w.burst[6].run = __cnet_bgProg_RoomSetRule;
        CnetSys_w.burst[6].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Set_RoomRuleFinish(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomSetFinish();
        return slot;
    }
    return -1;
}

int cnLBS_RoomEntry(arg0, arg1)
int arg0;
int arg1;
{
    int slot = __cnetSub_Set_BgProcess(1, 0);
    u16 cmd;

    if (slot != -1) {
        cmd = __cnet_SendReq_RoomEntry(arg0, arg1);
        __cnet_KeepEntryFloorInfo(2, arg0);
        CnetSys_w.bg[slot].cmd = cmd;
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerRoomEntry(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_RoomExit(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PieceExit(2);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomProperty(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomProperty(arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomProperty(int idx, int *d) {
    *d = CnetSys_w.room[(u16)idx - 1].prop;
    return 0;
}

int __cnet_SendReq_RoomProperty(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x98) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_BothRoomProperty(void) {
    u16 id;
    s32 prop;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_WordLong(&id, &prop);
        CnetSys_w.room[id - 1].id = id;
        CnetSys_w.room[id - 1].prop = prop;
        CnetSys_w.last_id = id;
        CnetSys_w.room[id - 1].flags |= 0x80;
    }
    _cnet_Return_CallBack(0x29);
}

int cnLBS_Set_RoomProperty(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_SetRoomProperty(arg0);
        return slot;
    }
    return -1;
}

int __cnet_SendReq_SetRoomProperty(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x96) & 0xFFFF;
    SetSendData32(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerSetRoomProperty(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_Get_RoomLeaveUser(CNET_B5C *d) {
    *d = CnetSys_w.leave_user;
    return 0;
}

void _cnet_RecvFromLbs_NoticeRoomCommer(void) {
    _sub_InOutRoomMember(2);
}

void _cnet_RecvFromLbs_NoticeRoomLeaver(void) {
    _sub_InOutRoomMember(3);
}

void _sub_InOutRoomMember(kind)
int kind;
{
    int r;

    if (CnetSys_w.rres == 0) {
        memset(&CnetSys_w.leave_user, 0, 0x5C);
        r = GetRecvDataOption3(&CnetSys_w.leave_user, 8, recv_work);
        switch (kind & 0xFF) {
        case 0:
        case 2:
        case 4:
            r = GetRecvDataOption3(&CnetSys_w.leave_user.b[8], 0x10, r);
            GetRecvDataOption3(&CnetSys_w.leave_user.b[0x1C], 0x40, r);
            break;
        }
    }
    if (CnetSys_w.rcat == 0x10) {
        switch (kind & 0xFF) {
        case 0:
            _cnet_Return_CallBack(0x26);
            break;
        case 1:
            _cnet_Return_CallBack(0x27);
            break;
        case 2:
            _cnet_Return_CallBack(0x1F);
            break;
        case 3:
            _cnet_Return_CallBack(0x20);
            break;
        case 4:
            _cnet_Return_CallBack(0x24);
            break;
        case 5:
            _cnet_Return_CallBack(0x25);
            break;
        }
    }
}

void _sub_ReceiveJoinUser(tbl)
CNET_PIECE *tbl;
{
    u16 id;
    u16 ja;
    u16 jb;
    CNET_PIECE *p;

    if (CnetSys_w.rres == 0) {
        p = tbl;
        if (CNW(u8, 0x10D2) != 4) {
            __cnet_Recv_PieceJoinUser(&id, &ja);
            p += id - 1;
            p->id = id;
            p->ja = ja;
            p->flags |= 1;
        } else if (CNW(u8, 0x10D2) == 4) {
            __cnet_Recv_PieceJoinUserMH(&id, &ja, &jb);
            p += id - 1;
            p->id = id;
            p->ja = ja;
            p->jb = jb;
            p->flags |= 1;
        }
        CnetSys_w.last_id = id;
        CnetSys_w.last_ja = ja;
    }
}

int cnLBS_Read_RoomRuleAllocation(int val, int cb) {
    if (CnetSys_w.burst[5].state == 0) {
        CnetSys_w.burst[5].cb = (void *)cb;
        CNW(s32, 0xEE0) = val & 0xFFFF;
        CnetSys_w.burst[5].state = 1;
        CnetSys_w.burst[5].run = __cnet_bgProg_ReadRoomRule;
        CnetSys_w.burst[5].x21 = 0;
        return 0;
    }
    return -1;
}

int cnLBS_Read_RoomRuleCount(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_NumOfRule(arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomNamePermission(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomNamePermission(arg0);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerRoomNamePermission(void) {
    u8 v;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_Byte(&v);
        CnetSys_w.ruletbl.name_perm = v;
    }
    _cnet_Return_CallBack(0);
}

int __cnet_SendReq_RoomNamePermission(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x55) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Read_RoomPasswordPermission(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomPasswordPermission(arg0 & 0xFFFF);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerRoomPasswordPermission(void) {
    u8 v;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_Byte(&v);
        CnetSys_w.ruletbl.pw_perm = v;
    }
    _cnet_Return_CallBack(0);
}

int __cnet_SendReq_RoomPasswordPermission(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x57) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Read_RoomPasswordInfo(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomPasswordInfo(arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomPasswordInfo(int idx, u8 *d) {
    *d = CnetSys_w.room[(u16)idx - 1].pwinfo;
    return 0;
}

int __cnet_SendReq_RoomPasswordInfo(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x86) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_BothRoomPasswordInfo(void) {
    u8 pw;
    u16 id;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_WordByte(&id, &pw);
        CnetSys_w.room[id - 1].pwinfo = pw;
        CnetSys_w.room[id - 1].flags |= 0x10;
        CnetSys_w.last_id = id;
        CnetSys_w.last_pwinfo = pw;
    }
    _cnet_Return_CallBack(0x1E);
}

int cnLBS_Read_RoomMemberList(arg, cb)
int arg;
int cb;
{
    int slot;

    memset(CnetSys_w.room_member, 0, 0x300);
    slot = __cnetSub_Set_BgProcess(1, 0, cb);
    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomMember(arg);
        return slot;
    }
    return -1;
}

int cnLBS_Get_RoomMemberList(idx, d1, d2, d3)
int idx;
char *d1;
char *d2;
void *d3;
{
    u8 *t = (u8 *)&CnetSys_w + (u16)idx * 0x60;

    strcpy(d1, t + 0x39EE);
    strcpy(d2, t + 0x39F6);
    memcpy(d3, t + 0x3A0A, 0x40);
    return 0;
}

int __cnet_SendReq_RoomMember(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x8B) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerRoomMember(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_RoomMember();
    }
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_RoomMember(void) {
    memset(CnetSys_w.room_member, 0, 0x300);
    __cnet_Recv_MemberSub(&CnetSys_w.n_room_member, CnetSys_w.room_member);
}

void _cnet_RecvFromLbs_AnswerAppointJump(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(0);
        __cnet_SetEntryFloorInfo(1);
        __cnet_SetEntryFloorInfo(2);
        __cnet_Recv_ServerMessage();
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_TopPageJump(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_TopPageJump();
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerTopPageJump(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_ClearEntryFloorInfo(0);
        __cnet_ClearEntryFloorInfo(1);
        __cnet_ClearEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

int __cnet_SendReq_TopPageJump(void) {
    int cmd = SetSendCommand(&send_work, 0x2C) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Read_RoomRuleCaption(int arg0, int arg1, int arg2) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg2);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleListHeadWord(arg0, arg1);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomRuleChoiceCount(int arg0, int arg1, int arg2) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg2);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleNumOfChoice(arg0, arg1);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomRuleNow(int arg0, int arg1, int arg2) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg2);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleListNow(arg0, arg1);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomRuleChoicePermission(int arg0, int arg1, int arg2) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg2);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleListPermission(arg0, arg1);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomRuleChoiceName(int arg0, int arg1, int arg2, int arg3) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg3);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleListName(arg0, arg1, arg2);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomRuleChoiceControl(int arg0, int arg1, int arg2, int arg3) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg3);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleControl(arg0, arg1, arg2);
        return slot;
    }
    return -1;
}

int cnetGet_Room_LastRecvNumber(u16 *arg0) {
    *arg0 = CNW(u16, 0x6CEC);
    return 0;
}

int cnLBS_Get_RoomRuleAllocation(unused, d)
int unused;
CNET_RULETBL *d;
{
    *d = CnetSys_w.ruletbl;
    return 0;
}

void _cnet_RecvFromLbs_BothGameJoin(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_GameJoin();
    }
    _cnet_Return_CallBack(0xD);
}

void _cnet_RecvFromLbs_AnswerRoomNumOfRule(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_Byte(CNWP(0x6E4B));
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRuleListHeadWord(void) {
    u8 n;
    char buf[0x4E];

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_ByteString(&n, buf);
        strcpy(CnetSys_w.ruletbl.e[n].head, buf);
        CnetSys_w.ruletbl.e[n].flags |= 1;
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRuleListPermission(void) {
    u8 n;
    u8 v;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_ByteByte(&n, &v);
        CnetSys_w.ruletbl.e[n].perm = v;
        CnetSys_w.ruletbl.e[n].flags |= 8;
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRuleListNow(void) {
    u8 n;
    u8 v;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_ByteByte(&n, &v);
        CnetSys_w.ruletbl.e[n].now = v;
        CnetSys_w.ruletbl.e[n].flags |= 2;
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRuleListNumOf(void) {
    u8 n;
    u8 v;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_ByteByte(&n, &v);
        CnetSys_w.ruletbl.e[n].numof = v;
        CnetSys_w.ruletbl.e[n].flags |= 4;
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRuleListName(void) {
    u8 n;
    u8 c;
    char buf[0x4E];

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_ByteByteString(&n, &c, buf);
        strcpy(CnetSys_w.ruletbl.e[n].names + c * 0x41, buf);
        CnetSys_w.ruletbl.e[n].cflag[c] |= 1;
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerRuleControl(void) {
    u8 rule;
    u8 ch;
    u8 *src;
    int i;
    int n;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_RuleControl(&rule, &ch, &src);
        n = *src;
        src++;
        for (i = 0; i < n; i++) {
            CnetSys_w.ruletbl.e[rule].tri[ch][i].a = src[0];
            CnetSys_w.ruletbl.e[rule].tri[ch][i].b = src[1];
            CnetSys_w.ruletbl.e[rule].tri[ch][i].c = src[2];
            src += 3;
        }
        CnetSys_w.ruletbl.e[rule].tcnt[ch] = n;
        CnetSys_w.ruletbl.e[rule].cflag[ch] |= 2;
    }
    _cnet_Return_CallBack(0);
}

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

void __cnet_bgProg_ReadPlazaAllocation(void) {
    CNET_BURST *b = &CnetSys_w.burst[2];
    CNET_RES res;

    if (b->state != 0) {
        switch (b->x21) {
        case 0: {
            int i;
            int m;
            b->x21++;
            b->a08 = 1;
            b->a0C = 1;
            b->res = 0;
            m = b->val;
            b->cnt = 0;
            for (i = 0; i < 0x20; i++) {
                if (m & 1) {
                    b->cnt++;
                }
                m >>= 1;
            }
            CNW(u16, 0x1032) = 0;
            CNW(u16, 0x404E) = 0;
            memset(CnetSys_w.plaza, 0, 0xDE8);
            cnLBS_Read_PlazaCount((int)__cnet_CallBack_Result_Plaza_NumOfPlaza_005A6E20);
            break;
        }
        case 1:
            if (b->res == 1) {
                b->x21++;
                b->res = 0;
                CNW(u16, 0x1032) = CNW(u16, 0x404E) * b->cnt;
                res.val = 2;
                res.id = 0xA;
                b->cb(res, &res);
            } else if (b->res == 2) {
                b->x21 = 6;
            }
            break;
        case 2:
            b->x21++;
            b->res = 0;
            if (CNW(u16, 0x404E) == 0) {
                b->x21 = 5;
            }
            break;
        case 3: {
            int k;
            int n;
            if (b->res == 2) {
                b->x21 = 6;
                cnLBS_Init_LobbyBgProcess();
                break;
            }
            k = b->a0C;
            if (__cnetSub_Get_RestBgWork() < b->cnt) {
                break;
            }
            if (b->val & 1) {
                cnLBS_Read_PlazaJoinUser((u16)k, (int)_cnet_CallBack_Result_Plaza_PlazaStatus_005A6E60);
            }
            if (b->val & 2) {
                cnLBS_Read_PlazaStatus((u16)k, (int)_cnet_CallBack_Result_Plaza_PlazaStatus_005A6E60);
            }
            if (b->val & 4) {
                cnLBS_Read_PlazaName((u16)k, (int)_cnet_CallBack_Result_Plaza_PlazaStatus_005A6E60);
            }
            if (b->val & 8) {
                cnLBS_Read_PlazaExplain((u16)k, (int)_cnet_CallBack_Result_Plaza_PlazaStatus_005A6E60);
            }
            n = k + 1;
            b->a0C++;
            if (CNW(u16, 0x404E) < n || n > 0xA) {
                b->x21++;
            }
            break;
        }
        case 4: {
            int n;
            int ok;
            int k;
            int i;
            n = CNW(u16, 0x404E);
            ok = 1;
            for (k = 0, i = 0; ; ) {
                k++;
                if (n < k) {
                    break;
                }
                if (b->val != (b->val & CnetSys_w.plaza[k - 1].flags)) {
                    ok = 0;
                    break;
                }
                i++;
                if (i >= 10) {
                    break;
                }
            }
            if (b->res == 2) {
                b->x21 = 6;
                cnLBS_Init_LobbyBgProcess();
            } else if (ok != 0) {
                b->x21++;
            }
            break;
        }
        case 5:
            res.val = 0;
            b->state = 0;
            b->x21 = 0;
            b->cb(res, &res);
            break;
        case 6:
            res.val = -1;
            b->state = 0;
            b->x21 = 0;
            b->cb(res, &res);
            break;
        }
    }
}

void _cnet_CallBack_Result_LobbyCount(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[3].res = 1;
        return;
    }
    CnetSys_w.burst[3].res = 2;
}

void _cnet_CallBack_Result_LobbyAllocation(CNET_RES res) {
    CNET_RES r;

    if (res.val == 0) {
        r.val = 2;
        r.id = 0xB;
        CnetSys_w.burst[3].cb(r, &r);
        CnetSys_w.burst[3].res = 1;
        return;
    }
    CnetSys_w.burst[3].res = 2;
}

void __cnet_bgProg_ReadLobbyAllocation(void) {
    CNET_BURST *b = &CnetSys_w.burst[3];
    CNET_RES res;

    if (b->state != 0) {
        switch (b->x21) {
        case 0: {
            int i;
            int m;
            b->x21++;
            b->a08 = 1;
            b->a0C = 1;
            b->res = 0;
            m = b->val;
            b->cnt = 0;
            for (i = 0; i < 0x20; i++) {
                if (m & 1) {
                    b->cnt++;
                }
                m >>= 1;
            }
            CNW(u16, 0x1032) = 0;
            CNW(u16, 0x4050) = 0;
            memset(CnetSys_w.lobby, 0, 0x1378);
            cnLBS_Read_LobbyCount((int)_cnet_CallBack_Result_LobbyCount);
            break;
        }
        case 1:
            if (b->res == 1) {
                b->x21++;
                b->res = 0;
                CNW(u16, 0x1032) = CNW(u16, 0x4050) * b->cnt;
                res.val = 2;
                res.id = 0xA;
                b->cb(res, &res);
            } else if (b->res == 2) {
                b->x21 = 6;
            }
            break;
        case 2:
            b->x21++;
            b->res = 0;
            if (CNW(u16, 0x4050) == 0) {
                b->x21 = 5;
            }
            break;
        case 3: {
            int k;
            int n;
            if (b->res == 2) {
                b->x21 = 6;
                cnLBS_Init_LobbyBgProcess();
                break;
            }
            k = b->a0C;
            if (__cnetSub_Get_RestBgWork() < b->cnt) {
                break;
            }
            if (b->val & 1) {
                cnLBS_Read_LobbyJoinUser((u16)k, (int)_cnet_CallBack_Result_LobbyAllocation);
            }
            if (b->val & 2) {
                cnLBS_Read_LobbyStatus((u16)k, (int)_cnet_CallBack_Result_LobbyAllocation);
            }
            if (b->val & 4) {
                cnLBS_Read_LobbyName((u16)k, (int)_cnet_CallBack_Result_LobbyAllocation);
            }
            if (b->val & 8) {
                cnLBS_Read_LobbyExplain((u16)k, (int)_cnet_CallBack_Result_LobbyAllocation);
            }
            n = k + 1;
            b->a0C++;
            if (CNW(u16, 0x4050) < n || n > 0xE) {
                b->x21++;
            }
            break;
        }
        case 4: {
            int n;
            int ok;
            int k;
            int i;
            n = CNW(u16, 0x4050);
            ok = 1;
            for (k = 0, i = 0; ; ) {
                k++;
                if (n < k) {
                    break;
                }
                if (b->val != (b->val & CnetSys_w.lobby[k - 1].flags)) {
                    ok = 0;
                    break;
                }
                i++;
                if (i >= 0xE) {
                    break;
                }
            }
            if (b->res == 2) {
                b->x21 = 6;
                cnLBS_Init_LobbyBgProcess();
            } else if (ok != 0) {
                b->x21++;
            }
            break;
        }
        case 5:
            res.val = 0;
            b->state = 0;
            b->x21 = 0;
            b->cb(res, &res);
            break;
        case 6:
            res.val = -1;
            b->state = 0;
            b->x21 = 0;
            b->cb(res, &res);
            break;
        }
    }
}

void _cnet_CallBack_Result_Room_NumOfRoom(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[4].res = 1;
        return;
    }
    CnetSys_w.burst[4].res = 2;
}

void _cnet_CallBack_Result_RoomJoinJoinUser(CNET_RES res) {
    CNET_RES r;

    if (res.val == 0) {
        r.val = 2;
        r.id = 0xB;
        CnetSys_w.burst[4].cb(r, &r);
        CnetSys_w.burst[4].res = 1;
        return;
    }
    CnetSys_w.burst[4].res = 2;
}

void __cnet_bgProg_ReadRoomAllocation(void) {
    CNET_BURST *b = &CnetSys_w.burst[4];
    CNET_RES res;

    if (b->state != 0) {
        switch (b->x21) {
        case 0: {
            int i;
            int m;
            b->x21++;
            b->a08 = 1;
            b->a0C = 1;
            b->res = 0;
            m = b->val;
            b->cnt = 0;
            for (i = 0; i < 0x20; i++) {
                if (m & 1) {
                    b->cnt++;
                }
                m >>= 1;
            }
            CNW(u16, 0x1032) = 0;
            CNW(u16, 0x4052) = 0;
            memset(CnetSys_w.room, 0, 0xB20);
            cnLBS_Read_RoomCount((int)_cnet_CallBack_Result_Room_NumOfRoom);
            break;
        }
        case 1:
            if (b->res == 1) {
                b->x21++;
                b->res = 0;
                CNW(u16, 0x1032) = CNW(u16, 0x4052) * b->cnt;
                res.val = 2;
                res.id = 0xA;
                b->cb(res, &res);
            } else if (b->res == 2) {
                b->x21 = 6;
            }
            break;
        case 2:
            b->x21++;
            b->res = 0;
            if (CNW(u16, 0x4052) == 0) {
                b->x21 = 5;
            }
            break;
        case 3: {
            int k;
            int n;
            if (b->res == 2) {
                b->x21 = 6;
                cnLBS_Init_LobbyBgProcess();
                break;
            }
            k = b->a0C;
            if (__cnetSub_Get_RestBgWork() < b->cnt) {
                break;
            }
            if (b->val & 1) {
                cnLBS_Read_RoomJoinUser((u16)k, (int)_cnet_CallBack_Result_RoomJoinJoinUser);
            }
            if (b->val & 2) {
                cnLBS_Read_RoomStatus((u16)k, (int)_cnet_CallBack_Result_RoomJoinJoinUser);
            }
            if (b->val & 4) {
                cnLBS_Read_RoomName((u16)k, (int)_cnet_CallBack_Result_RoomJoinJoinUser);
            }
            if (b->val & 0x20) {
                cnLBS_Read_RoomJoinInfo((u16)k, (int)_cnet_CallBack_Result_RoomJoinJoinUser);
            }
            if (b->val & 8) {
                cnLBS_Read_RoomExplain((u16)k, (int)_cnet_CallBack_Result_RoomJoinJoinUser);
            }
            if (b->val & 0x10) {
                cnLBS_Read_RoomPasswordInfo((u16)k, (int)_cnet_CallBack_Result_RoomJoinJoinUser);
            }
            if (b->val & 0x40) {
                cnLBS_Read_MatchEntryJoinUser((u16)k, (int)_cnet_CallBack_Result_RoomJoinJoinUser);
            }
            if (b->val & 0x80) {
                cnLBS_Read_RoomProperty((u16)k, (int)_cnet_CallBack_Result_RoomJoinJoinUser);
            }
            n = k + 1;
            b->a0C++;
            if (CNW(u16, 0x4052) < n || n > 8) {
                b->x21++;
            }
            break;
        }
        case 4: {
            int n;
            int ok;
            int k;
            int i;
            n = CNW(u16, 0x4052);
            ok = 1;
            for (k = 0, i = 0; ; ) {
                k++;
                if (n < k) {
                    break;
                }
                if (b->val != (b->val & CnetSys_w.room[k - 1].flags)) {
                    ok = 0;
                    break;
                }
                i++;
                if (i >= 10) {
                    break;
                }
            }
            if (b->res == 2) {
                b->x21 = 6;
                cnLBS_Init_LobbyBgProcess();
            } else if (ok != 0) {
                b->x21++;
            }
            break;
        }
        case 5:
            res.val = 0;
            b->state = 0;
            b->x21 = 0;
            b->cb(res, &res);
            break;
        case 6:
            res.val = -1;
            b->state = 0;
            b->x21 = 0;
            b->cb(res, &res);
            break;
        }
    }
}

int cnLBS_Get_AllocationProgressCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x1032);
    return 0;
}

void _cnet_CallBack_Result_Rule_NumOfRule(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[5].res = 1;
        return;
    }
    CnetSys_w.burst[5].res = 2;
}

void _cnet_CallBack_Result_RoomRuleCaption(void) {
    CNET_RES r;

    r.val = 2;
    r.id = 0xB;
    CnetSys_w.burst[5].cb(r, &r);
}

void __cnet_bgProg_ReadRoomRule(void) {
    CNET_BURST *b = &CnetSys_w.burst[5];
    CNET_RES res;

    if (b->state == 0) {
        return;
    }
    switch (b->x21) {
    case 0:
        b->x21++;
        CNW(u16, 0x1032) = 0;
        CnetSys_w.ruletbl.n = 0;
        memset(&CnetSys_w.ruletbl, 0, 0x294A4);
        b->a08 = 1;
        b->a0C = 1;
        b->res = 0;
        cnLBS_Read_RoomRuleCount((u16)b->x14, (int)_cnet_CallBack_Result_Rule_NumOfRule);
        return;
    case 1:
        if (b->res == 1) {
            b->x21++;
            b->res = 0;
            if (CnetSys_w.ruletbl.n == 0) {
                b->x21 = 5;
                return;
            }
        } else if (b->res == 2) {
            b->x21 = 0x12;
            return;
        }
        break;
    case 2:
        b->x21++;
        b->a08 = 0;
        b->a0C = 0;
    case 3: {
        int k;
        k = b->a0C;
        if (__cnetSub_Get_RestBgWork() > 0) {
            cnLBS_Read_RoomRuleChoiceCount((u16)b->x14, k & 0xFF, (int)_cnet_CallBack_Result_RoomRuleCaption);
            b->a0C++;
            if (k + 1 >= CnetSys_w.ruletbl.n) {
                b->x21++;
                return;
            }
        }
        break;
    }
    case 4: {
        int i;
        int all;
        int n;
        n = CnetSys_w.ruletbl.n;
        all = 1;
        for (i = 0; i < n; i++) {
            if ((CnetSys_w.ruletbl.e[i].flags & 0xF) != 4) {
                all = 0;
                break;
            }
        }
        if (all != 0) {
            b->x21++;
            b->a08 = 0;
            for (i = 0; i < 0x20; i++) {
                CnetSys_w.ruletbl.e[i].flags = 0;
            }
            return;
        }
        break;
    }
    case 5: {
        int i;
        int sum;
        b->x21++;
        sum = 0;
        for (i = 0; i < CnetSys_w.ruletbl.n; i++) {
            sum += CnetSys_w.ruletbl.e[i].numof;
        }
        res.val = 2;
        res.id = 0xA;
        CNW(s16, 0x1032) = CnetSys_w.ruletbl.n * 3 + 2 + sum * 2;
        b->cb(res, &res);
        cnLBS_Read_RoomNamePermission((u16)b->x14, (int)_cnet_CallBack_Result_Rule_NumOfRule);
        return;
    }
    case 6:
        if (b->res != 0) {
            b->x21++;
            b->res = 0;
            res.val = 2;
            res.id = 0xB;
            b->cb(res, &res);
            return;
        }
        break;
    case 7:
        b->x21++;
        cnLBS_Read_RoomPasswordPermission((u16)b->x14, (int)_cnet_CallBack_Result_Rule_NumOfRule);
        return;
    case 8:
        if (b->res != 0) {
            b->x21++;
            b->res = 0;
            res.val = 2;
            res.id = 0xB;
            b->cb(res, &res);
            return;
        }
        break;
    case 9:
        b->x21++;
        cnLBS_Read_RoomExplainPermission((u16)b->x14, (int)_cnet_CallBack_Result_Rule_NumOfRule);
        return;
    case 10:
        if (b->res != 0) {
            b->res = 0;
            if (CnetSys_w.ruletbl.n == 0) {
                b->x21 = 0x11;
                return;
            }
            b->x21++;
            res.val = 2;
            res.id = 0xB;
            b->cb(res, &res);
            return;
        }
        break;
    case 11:
        b->x21++;
        b->a08 = 0;
        b->a0C = 0;
    case 12: {
        int k;
        k = b->a0C;
        if (__cnetSub_Get_RestBgWork() >= 3) {
            cnLBS_Read_RoomRuleCaption((u16)b->x14, k & 0xFF, (int)_cnet_CallBack_Result_RoomRuleCaption);
            cnLBS_Read_RoomRuleNow((u16)b->x14, k & 0xFF, (int)_cnet_CallBack_Result_RoomRuleCaption);
            cnLBS_Read_RoomRuleChoicePermission((u16)b->x14, k & 0xFF, (int)_cnet_CallBack_Result_RoomRuleCaption);
            b->a0C++;
            if (k + 1 >= CnetSys_w.ruletbl.n) {
                b->x21++;
                return;
            }
        }
        break;
    }
    case 13: {
        int i;
        int all;
        int n;
        n = CnetSys_w.ruletbl.n;
        all = 1;
        for (i = 0; i < n; i++) {
            if ((CnetSys_w.ruletbl.e[i].flags & 0xF) != 0xB) {
                all = 0;
                break;
            }
        }
        if (all != 0) {
            b->x21++;
            b->a08 = 0;
            for (i = 0; i < 0x20; i++) {
                memset(CnetSys_w.ruletbl.e[i].cflag, 0, 0x20);
            }
            return;
        }
        break;
    }
    case 14:
        b->x21++;
        b->a0C = 0;
        return;
    case 15: {
        int k;
        k = b->a0C;
        if (CnetSys_w.ruletbl.e[(u32)b->a08].numof == 0) {
            b->x21++;
            return;
        }
        if (__cnetSub_Get_RestBgWork() >= 2) {
            cnLBS_Read_RoomRuleChoiceName((u16)b->x14, (u8)b->a08, k & 0xFF, (int)_cnet_CallBack_Result_RoomRuleCaption);
            cnLBS_Read_RoomRuleChoiceControl((u16)b->x14, (u8)b->a08, k & 0xFF, (int)_cnet_CallBack_Result_RoomRuleCaption);
            b->a0C++;
            if (k + 1 >= CnetSys_w.ruletbl.e[(u32)b->a08].numof) {
                b->x21++;
                return;
            }
        }
        break;
    }
    case 16: {
        int i;
        int all;
        all = 1;
        for (i = 0; i < CnetSys_w.ruletbl.e[(u32)b->a08].numof; i++) {
            if ((CnetSys_w.ruletbl.e[(u32)b->a08].cflag[i] & 3) != 3) {
                all = 0;
            }
        }
        if (all != 0) {
            b->a08++;
            if ((u32)b->a08 >= CnetSys_w.ruletbl.n) {
                b->x21++;
                return;
            }
            b->x21 = 0xE;
            return;
        }
        break;
    }
    case 17:
        res.val = 0;
        b->state = 0;
        b->x21 = 0;
        b->cb(res, &res);
        return;
    case 18:
        res.val = -1;
        b->state = 0;
        b->x21 = 0;
        b->cb(res, &res);
        break;
    }
}

void _cnet_CallBack_Result_RoomSetFinish(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[6].res = 1;
        return;
    }
    CnetSys_w.burst[6].res = 2;
}

void __cnet_bgProg_RoomSetRule(void) {
    CNET_BURST *b = &CnetSys_w.burst[6];
    CNET_RES res;
    int i;

    if (b->state != 0) {
        switch (b->x21) {
        case 0:
            b->x21 = 2;
            b->res = 0;
            __cnet_SendReq_RoomSetName(CnetSys_w.rule.name);
            if (CnetSys_w.rule.pw[0] != 0 && CnetSys_w.ruletbl.pw_perm == 1) {
                __cnet_SendReq_RoomSetPassword(CnetSys_w.rule.pw);
            }
            break;
        case 2:
            b->x21 = 4;
            for (i = 0; i < CnetSys_w.ruletbl.n; i++) {
                if (CnetSys_w.ruletbl.e[i].perm == 1) {
                    __cnet_SendReq_RoomSetRule(i & 0xFF, CnetSys_w.rule.sel[i]);
                }
            }
            break;
        case 4:
            b->x21 = 6;
            if (CnetSys_w.rule.explain[0] != 0 && CnetSys_w.ruletbl.explain_perm == 1) {
                __cnet_SendReq_RoomSetExplain(CnetSys_w.rule.explain, strlen(CnetSys_w.rule.explain) & 0xFFFF);
            }
            break;
        case 6:
            b->x21++;
            cnLBS_Set_RoomRuleFinish((int)_cnet_CallBack_Result_RoomSetFinish);
            break;
        case 7:
            if (b->res == 1) {
                res.val = 0;
                b->state = 0;
                b->x21 = 0;
                b->cb(res, &res);
            } else if (b->res == 2) {
                res.val = -1;
                b->state = 0;
                b->x21 = 0;
                b->cb(res, &res);
            }
            break;
        case 1:
        case 3:
        case 5:
            break;
        }
    }
}

int __cnet_SendReq_PieceCount(kind)
int kind;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x32) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x46) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x7B) & 0xFFFF;
        break;
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PieceName(kind, arg1)
int kind;
int arg1;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x34) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x48) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x7D) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PieceJoinUser(kind, arg1)
int kind;
int arg1;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x36) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x4A) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x80) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PieceStatus(kind, arg1)
int kind;
int arg1;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x39) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x4D) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x83) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PieceExplain(kind, arg1)
int kind;
int arg1;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x3C) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x50) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x90) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PieceEntry(kind, arg1)
int kind;
int arg1;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x30) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x42) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x73) & 0xFFFF;
        break;
    }
    SetSendData16(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomEntry(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x73) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendStringData2(&send_work, arg1, strlen(arg1) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_PieceExit(kind)
int kind;
{
    int cmd;

    switch (kind & 0xFFFF) {
    case 0:
        cmd = SetSendCommand(&send_work, 0x3F) & 0xFFFF;
        break;
    case 1:
        cmd = SetSendCommand(&send_work, 0x44) & 0xFFFF;
        break;
    case 2:
        cmd = SetSendCommand(&send_work, 0x75) & 0xFFFF;
        break;
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_NumOfRule(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x59) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleListHeadWord(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x5B) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleNumOfChoice(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x61) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleListNow(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x5F) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleListPermission(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x5D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleListName(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0x63) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendData8(&send_work, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RuleControl(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0x65) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendData8(&send_work, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void __cnet_Recv_GameJoin(void) {

}

void __cnet_Recv_NumOfPiece(u16 *d) {
    GetRecvData16(d, recv_work);
}

void __cnet_Recv_PieceName(void *a, void *b) {
    GetRecvDataString(b, GetRecvData16(a, recv_work));
}

void __cnet_Recv_PieceJoinUser(void *a, void *b) {
    GetRecvData16(b, GetRecvData16(a, recv_work));
}

void __cnet_Recv_PieceJoinUserMH(void *a, void *b, void *c) {
    GetRecvData16(c, GetRecvData16(b, GetRecvData16(a, recv_work)));
}

void __cnet_Recv_PieceStatus(void *a, void *b) {
    GetRecvData8(b, GetRecvData16(a, recv_work));
}

void __cnet_Recv_PieceExplain(void *a, void *b) {
    GetRecvDataString(b, GetRecvData16(a, recv_work));
}

void __cnet_Recv_RuleControl(void *a, void *b, void *c) {
    *(int *)c = GetRecvData8(b, GetRecvData8(a, recv_work));
}

int __cnet_SendReq_RoomCreate(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x53) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetName(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x67) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, strlen(arg0) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetPassword(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x69) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, strlen(arg0) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetRule(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x6B) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendData8(&send_work, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int __cnet_SendReq_RoomSetFinish(void) {
    int cmd = SetSendCommand(&send_work, 0x6D) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerLobbyMatchEntry(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerLobbyMatchEntryUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        GetRecvData16(CNWP(0x302F4), GetRecvData16(CNWP(0x302F2), GetRecvData16(CNWP(0x302F0), &recv_work)));
    }
    _cnet_Return_CallBack(0x22);
}

void _cnet_RecvFromLbs_AnswerAnnexEntry(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_AnswerAnnexExit(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_BothAnnexJoinUser(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_Word(CNWP(0x302FC));
    }
    _cnet_Return_CallBack(0x23);
}

void _cnet_RecvFromLbs_AnswerAnnexMember(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_AnnexMember();
    }
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_AnnexMember(void) {
    memset(CnetSys_w.annex_member, 0, 0x300);
    __cnet_Recv_MemberSub(&CnetSys_w.n_annex_member, CnetSys_w.annex_member);
}

void _cnet_RecvFromLbs_NoticeAnnexLeaver(void) {
    _sub_InOutRoomMember(5);
}

void _cnet_RecvFromLbs_NoticeAnnexCommer(void) {
    _sub_InOutRoomMember(4);
}

int cnLBS_Read_LobbyMemberList(arg, cb)
int arg;
int cb;
{
    int slot = __cnetSub_Set_BgProcess(1, 0, cb);

    if (slot != -1) {
        memset(CnetSys_w.lobby_member, 0, 0x300);
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_LobbyMemberList(arg);
        return slot;
    }
    return -1;
}

int cnLBS_Get_LobbyMemberList(idx, d1, d2, d3)
int idx;
char *d1;
char *d2;
void *d3;
{
    u8 *t = (u8 *)&CnetSys_w + (u16)idx * 0x60;

    strcpy(d1, t + 0x36EE);
    strcpy(d2, t + 0x36F6);
    memcpy(d3, t + 0x370A, 0x40);
    return 0;
}

int __cnet_SendReq_LobbyMemberList(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xFE) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerLobbyMember(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_Recv_LobbyMember();
    }
    _cnet_Return_CallBack(0);
}

void __cnet_Recv_LobbyMember(void) {
    memset(CnetSys_w.lobby_member, 0, 0x300);
    __cnet_Recv_MemberSub(&CnetSys_w.n_lobby_member, CnetSys_w.lobby_member);
}

void __cnet_Recv_MemberSub(arg0, arg1)
u8 *arg0;
u8 *arg1;
{
    u8 sp4F;
    u8 sp4E;
    u16 sp4C;
    int x;
    int r;
    int i;
    int j;

    r = GetRecvData8(&sp4E, GetRecvData8(&sp4F, GetRecvData16(&sp4C, recv_work)));
    if (sp4E > 8) {
        sp4E = 8;
    }
    *arg0 = sp4E;
    for (i = 0; i < sp4E; i++) {
        r = GetRecvDataOption3(arg1 + 0x1C, 0x40, GetRecvDataOption3(arg1 + 8, 0x10, GetRecvDataOption3(arg1, 8, r)));
        arg1 += 0x60;
        if (sp4F > 3) {
            for (j = 0; j < sp4F - 3; j++) {
                x = GetRecvData16(&sp4C, r); r = x + sp4C;
            }
        }
    }
}

void _cnet_RecvFromLbs_NoticeLobbyLeaver(void) {
    _sub_InOutRoomMember(1);
}

void _cnet_RecvFromLbs_NoticeLobbyCommer(void) {
    _sub_InOutRoomMember(0);
}

int cnLBS_Read_MatchEntryJoinUser(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_MatchEntryUser(arg0);
        return slot;
    }
    return -1;
}

int cnLBS_Get_MatchEntryJoinUser(idx, a, b)
int idx;
u16 *a;
u16 *b;
{
    if (idx != 0) {
        *a = CnetSys_w.room[(u16)idx - 1].ma;
        *b = CnetSys_w.room[(u16)idx - 1].mb;
    }
    return 0;
}

void _cnet_RecvFromLbs_AnswerMatchEntryUser(void) {
    u16 id;

    if (CnetSys_w.rres == 0) {
        GetRecvData16(&CnetSys_w.room[id - 1].mb, GetRecvData16(&CnetSys_w.room[id - 1].ma, GetRecvData16(&id, recv_work), id));
        CnetSys_w.room[id - 1].flags |= 0x40;
        CnetSys_w.last_id = id;
    }
    _cnet_Return_CallBack(0x21);
}

void _cnet_RecvFromLbs_NoticeMatchEntryUser(void) {
    int id;
    u8 *s0;

    if (CnetSys_w.rres == 0) {
        id = CNW(u8, 0x4060);
        s0 = (u8 *)&CnetSys_w + ((id - 1) * 0x164);
        GetRecvData16(s0 + 0x61DE, GetRecvData16(s0 + 0x61DC, recv_work));
        CnetSys_w.last_id = id;
    }
    _cnet_Return_CallBack(0x21);
}

int __cnet_SendReq_MatchEntryUser(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x9D) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerRoomMatchEntryTypeList(void) {
    u8 cnt;
    u8 type;
    u16 sp5C;
    char name[8];
    int i;
    int p;
    int k;
    u8 *s2;

    if (CnetSys_w.rres == 0) {
        p = GetRecvData8(&cnt, GetRecvData16(&sp5C, recv_work));
        if (cnt != 0) {
            i = 0;
            if (i < (cnt & 0xFF)) {
                do {
                    memset(name, 0, 8);
                    p = GetRecvData8(&type, GetRecvDataOption3(name, 8, p));
                    k = 0;
                    s2 = (u8 *)&CnetSys_w;
                    do {
                        if (memcmp(name, s2 + 0x39EE, 8) == 0) {
                            s2[0x3A4A] = type;
                        }
                        k++;
                        s2 += 0x60;
                    } while (k < 8);
                    i++;
                } while (i < cnt);
            }
        }
    }
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeRoomMatchEntryTypeList(void) {
    u16 id;
    int p;
    u8 *s0;

    if (CnetSys_w.rres == 0) {
        memset(CnetSys_w.extra_member, 0, 0x60);
        p = GetRecvData16(&id, recv_work);
        if (id == CNW(u8, 0x4060)) {
            s0 = CnetSys_w.room_member;
            CnetSys_w.n_room_member = 1;
            GetRecvData8(s0 + 0x5C, GetRecvDataOption3(s0, 8, p));
            goto cb;
        }
    } else {
cb:
        _cnet_Return_CallBack(0x28);
    }
}

int __cnet_SendReq_RoomSetExplain(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x71) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerRoomSetExplain(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_RoomExplainPermission(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomExplainPermission(arg0);
        return slot;
    }
    return -1;
}

void _cnet_RecvFromLbs_AnswerRoomExplainPermission(void) {
    u8 v;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_Byte(&v);
        CnetSys_w.ruletbl.explain_perm = v;
    }
    _cnet_Return_CallBack(0);
}

int __cnet_SendReq_RoomExplainPermission(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x6F) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Read_TimingValue(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_TimingValue();
        return slot;
    }
    return -1;
}

int cnLBS_Get_TimingValue(int *arg0) {
    *arg0 = CNW(int, 0x3BA54);
    return 0;
}

void _cnet_RecvFromLbs_BothTimingValue(void) {
    s32 v;

    if (CnetSys_w.rres == 0) {
        __cnet_Recv_Long(&v);
        CNW(s32, 0x3BA54) = v;
    }
    _cnet_Return_CallBack(0x2B);
}

int __cnet_SendReq_TimingValue(void) {
    int cmd = SetSendCommand(&send_work, 0xD3) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Read_CurrentPlace(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_CurrentPlace();
        return slot;
    }
    return -1;
}

typedef struct { s16 a, b, c; } CPLACE3;
int cnLBS_Get_CurrentPlace(s16 *d) {
    *(CPLACE3 *)d = *(CPLACE3 *)CnetSys_w.curplace;
    return 0;
}

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

void _cnet_RecvFromLbs_RequestWarningMessage(void) {
    CNET_RES res;
    u16 n;
    int s;
    u32 t;
    int r;

    if (CnetSys_w.burst[0].state != 0) {
        if ((u32)CnetSys_w.rlen >= 2) {
            n = 0x600;
        } else {
            n = (((((recv_header[4] << 8) & 0xFFFF) + recv_header[5]) & 0xFFFF) - CNW(u16, 0xFF0)) & 0xFFFF;
        }
        t = CnetSys_w.xff4 - CnetSys_w.xff0;
        if ((n & 0xFFFF) >= t) {
            n = (t - 1) & 0xFFFF;
        }
        s = n & 0xFFFF;
        memcpy(CnetSys_w.xff8 + CnetSys_w.xff0, recv_work, s);
        CnetSys_w.xff0 += s;
        if (CnetSys_w.rlen == 1) {
            r = GetRecvData8(&CnetSys_w.warnmsg, &CnetSys_w.loginbuf);
            r = GetRecvData16((u8 *)&CnetSys_w.warnmsg + 2, r);
            GetRecvDataString((u8 *)&CnetSys_w.warnmsg + 4, GetRecvDataString((u8 *)&CnetSys_w.warnmsg + 4, r));
            res.val = 0;
            res.id = 4;
            CnetSys_w.burst[0].cb(res, &res);
        }
    }
}

void cnLBS_Get_LoginWarningMessage(CNET_H1004 *d) {
    *d = CnetSys_w.warnmsg;
}

int cnLBS_Answer_LoginWarningMessage(void) {
    __cnet_SendAns_WarningMessage();
    return 0;
}

void __cnet_SendAns_WarningMessage(int arg0) {
    SetSendCommand(&send_work, 0x14);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int cnLBS_Send_LoginFinish(void) {
    __cnet_SendSet_LoginFinish();
    return 0;
}

void _cnet_RecvFromLbs_AnswerBillEstimate(void) {

}

void _cnet_RecvFromLbs_AnswerUserBinary(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_Read_TopInformation(cb)
int cb;
{
    int slot = __cnetSub_Set_BgProcess(1, 0, cb);

    CnetSys_w.xff0 = 0;
    CnetSys_w.xff4 = 0x1000;
    CnetSys_w.xff8 = CnetSys_w.loginbuf.b;
    memset(&CnetSys_w.topinfo, 0, 0x1004);
    memset(&CnetSys_w.loginbuf, 0, 0x2000);
    CnetSys_w.xff0 = 0;
    CnetSys_w.xff4 = 0x1000;
    CnetSys_w.xff8 = CnetSys_w.loginbuf.b;
    memset(&CnetSys_w.topinfo, 0, 0x1004);
    memset(&CnetSys_w.loginbuf, 0, 0x2000);
    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_TopInformation();
        return slot;
    }
    return -1;
}

int cnLBS_Get_TopInformation(CNET_B1004 *d) {
    *d = CnetSys_w.topinfo;
    return 0;
}

int __cnet_SendReq_TopInformation(void) {
    int cmd = SetSendCommand(&send_work, 0x1F) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerTopInformation(void) {
    u16 n;
    int s;
    u32 t;
    int r;
    u8 *tp;

    if (CnetSys_w.rres == 0) {
        if ((u32)CnetSys_w.rlen >= 2) {
            n = 0x600;
        } else {
            n = (((((recv_header[4] << 8) & 0xFFFF) + recv_header[5]) & 0xFFFF) - CNW(u16, 0xFF0)) & 0xFFFF;
        }
        t = CnetSys_w.xff4 - CnetSys_w.xff0;
        if ((n & 0xFFFF) >= t) {
            n = (t - 1) & 0xFFFF;
        }
        s = n & 0xFFFF;
        memcpy(CnetSys_w.xff8 + CnetSys_w.xff0, recv_work, s);
        CnetSys_w.xff0 += s;
        if (CnetSys_w.rlen == 1) {
            tp = (u8 *)&CnetSys_w.topinfo;
            r = GetRecvData8(tp, &CnetSys_w.loginbuf);
            GetRecvDataString(tp + 4, r);
            _cnet_Return_CallBack(0);
        }
    } else {
        _cnet_Return_CallBack(0);
    }
}

void __cnet_Login_Return(void) {
    CNET_RES res;

    if (CnetSys_w.burst[0].state != 0) {
        res.val = 0;
        CnetSys_w.burst[0].state = 0;
        res.id = 0;
        CnetSys_w.burst[0].x21 = 0;
        CnetSys_w.burst[0].cb(res, &res);
    }
}

void _cnet_RecvFromLbs_RequestTelephoneNumber(void) {
    __cnet_SendSet_TelephoneNumber();
}

void _cnet_RecvFromLbs_RequestPersonalDataRegist(void) {

}

void _cnet_RecvFromLbs_RequestBattleResult(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_SendAns_BattleResult();
    }
}

void __cnet_SendAns_BattleResult(void) {
    CNET_BATRES *b = &CnetSys_w.batres;

    SetSendCommand(&send_work, 0x19);
    if (b->flag != 0) {
        SetSendData16(&send_work, 0x5678);
        SetSendStringData2(&send_work, b, 0xF);
        SetSendData8(&send_work, 0);
        SetSendData8(&send_work, 0);
        SetSendData8(&send_work, b->flag);
        SetSendData16(&send_work, b->v[0]);
        SetSendData16(&send_work, b->v[1]);
        SetSendData16(&send_work, b->v[2]);
        SetSendData16(&send_work, b->v[3]);
        SetSendData16(&send_work, b->v[4]);
        SetSendData16(&send_work, b->v[5]);
        SetSendData16(&send_work, b->v[6]);
        SetSendData16(&send_work, b->v[7]);
    } else {
        SetSendResult(&send_work, 0xFF);
    }
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_AnswerPersonalRecordHeader(void) {
    u8 n;
    int r;
    int i;

    if (CNW(u8, 0xF58) != 0) {
        if (CnetSys_w.rcat == 2 && CnetSys_w.rres == 0) {
            r = GetRecvData8(&n, recv_work);
            if (n > 2) {
                CNW(u8, 0x30CAC) = n;
                for (i = 0; i < n; i++) {
                    r = GetRecvData8(CNWP(0x30CAD) + i, r);
                }
            } else {
                CNW(u8, 0x30CAC) = 2;
            }
            CNW(s8, 0x30CAD) = 1;
            CNW(s8, 0x30CAE) = 0x11;
        }
        CNW(s8, 0xF5A) = 1;
    }
}

void _cnet_RecvFromLbs_AnswerPersonalRecordData(void) {
    u8 a;
    u8 b;
    int r;
    int i;
    u8 *p;

    if (CNW(u8, 0xF58) != 0) {
        if (CnetSys_w.rcat == 2 && CnetSys_w.rres == 0) {
            r = GetRecvData8(&b, GetRecvData8(&a, recv_work));
            p = CNWP(0x30CB8) + a * 0x3480 + b * 0x118;
            p[0] = a;
            p[1] = b;
            r = GetRecvData32(p + 0x28, GetRecvData32(p + 0x24, GetRecvData32(p + 0x20, GetRecvData32(p + 0x1C, GetRecvData32(p + 0x18, GetRecvData32(p + 0x14, GetRecvData8(p + 0xC, GetRecvData32(p + 8, GetRecvData32(p + 4, GetRecvData32(p + 0x10, r))))))))));
            if (a == 1) {
                if (CnetSys_w.rseq2 > 0x27) {
                    for (i = 0; i < 3; i++) {
                        r = GetRecvDataOption3(p + 0x91 + i * 0x11, 0x10, GetRecvDataOption3(p + 0x79 + i * 8, 8, GetRecvData8(p + 0x76 + i, GetRecvData8(p + 0x73 + i, r))));
                    }
                    for (i = 0; i < 3; i++) {
                        r = GetRecvDataOption3(p + 0xE2 + i * 0x11, 0x10, GetRecvDataOption3(p + 0xCA + i * 8, 8, GetRecvData8(p + 0xC7 + i, GetRecvData8(p + 0xC4 + i, r))));
                    }
                }
            }
        }
        CNW(u8, 0xF5A)++;
    }
}

void _cnet_RecvFromLbs_AnswerPersonalRecordVide(void) {
    u8 a;
    u8 b;
    int r;
    u8 *p;

    if (CNW(u8, 0xF58) != 0) {
        if (CnetSys_w.rcat == 2 && CnetSys_w.rres == 0) {
            r = GetRecvData8(&b, GetRecvData8(&a, recv_work));
            p = CNWP(0x30CB8) + a * 0x3480 + b * 0x118;
            GetRecvData8(p + 0x72, GetRecvData8(p + 0x71, GetRecvData8(p + 0x70, GetRecvData8(p + 0x6F, GetRecvData8(p + 0x6E, GetRecvData8(p + 0x6D, GetRecvDataString(p + 0x2C, r)))))));
        }
        CNW(u8, 0xF5A)++;
    }
}

void __cnet_SendSet_ConnectionPair(void) {
    char sp10[0x10];

    SetSendCommand(&send_work, 0xD);
    mmbbc_encode(sp10, CnetSys_w.login.key, (((send_work.seq_h << 8) & 0xFFFF) + send_work.seq_l) & 0xFFFF);
    SetSendData16(&send_work, 0xA);
    SetSendStringData(&send_work, sp10, 0xA);
    SetSendEncodeStringData(&send_work, CnetSys_w.login.pass, strlen(CnetSys_w.login.pass) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_FirstData(void) {
    u8 *s0 = (u8 *)&CnetSys_w.firstdata;

    SetSendCommand(&send_work, 0x11);
    SetSendData8(&send_work, s0[0]);
    SetSendData8(&send_work, s0[1]);
    SetSendData8(&send_work, s0[2]);
    SetSendStringData2(&send_work, s0 + 4, 0xA);
    SetSendData16(&send_work, *(u16 *)(s0 + 0x14));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x16));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x18));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x1A));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x1C));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x1E));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x20));
    SetSendData16(&send_work, *(u16 *)(s0 + 0x22));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendReq_UserID(void) {
    SetSendCommand(&send_work, 0x16);
    SetSendStringData2(&send_work, CnetSys_w.login_users[3].id, 6);
    SetSendStringData2(&send_work, CnetSys_w.login_users[3].handle, strlen(CnetSys_w.login_users[3].handle) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_LoginFinish(void) {
    SetSendCommand(&send_work, 0x1A);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void __cnet_SendSet_TelephoneNumber(void) {
    SetSendCommand(&send_work, 0xF);
    SetSendStringData2(&send_work, CnetSys_w.login.tel, strlen(CnetSys_w.login.tel) & 0xFFFF);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int __cnet_SendSet_MiniDataRegist(int arg0, int arg1) {
    int cmd = SetSendCommand(&send_work, 0x21) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void __cnet_SendReq_EchoPacket(void) {
    CnetSys_w.rcnt = 0;
    SetSendCommand(&send_work, 0xA);
    SetSendStringData2(&send_work, "0", 1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

int __cnet_Recv_UserIDandHandle(void) {
    u8 n;
    int i;
    u8 *s0;
    int p;

    memset(CnetSys_w.login_users, 0, 0x170);
    p = GetRecvData8(&n, recv_work);
    if (n > 3) n = 3;
    CnetSys_w.n_login_user = n;
    i = 0;
    if (0 < n) {
        s0 = (u8 *)&CnetSys_w;
        do {
            p = GetRecvDataOption3(s0 + 0x147E, 0x40, GetRecvDataOption3(s0 + 0x146A, 0x10, GetRecvDataOption3(s0 + 0x1462, 8, p)));
            i++;
            s0 += 0x5C;
        } while (i < n);
    }
    return 0;
}

int __cnet_Recv_UserID(void) {
    GetRecvDataOption3(CNWP(0x1576), 8, &recv_work);
    return 0;
}

void cnLBS_Send_ChatMessage(int a, int b) {
    __cnet_SendSet_ChatMessage(0, a, b);
}

void cnLBS_Get_ChatMessage(CNET_CHAT *d) {
    *d = CnetSys_w.chat;
}

void __cnet_SendSet_ChatMessage(int arg0, int arg1, int arg2) {
    SetSendCommand(&send_work, 0xE8);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeChatMessage(void) {
    memset(CnetSys_w.chat.from, 0, 0x120);
    GetRecvData8(&CnetSys_w.chat.d, GetRecvData8(&CnetSys_w.chat.c, GetRecvData8(&CnetSys_w.chat.b, GetRecvData8(&CnetSys_w.chat.a, GetRecvDataOption3(CnetSys_w.chat.msg, 0x100, GetRecvDataOption3(CnetSys_w.chat.x, 0x10, GetRecvDataOption3(CnetSys_w.chat.from, 8, recv_work)))))));
    _cnetEvent_JumpCallBack(5, 0);
}

void cnLBS_Send_ChatBinary(void) {
    __cnet_SendSet_ChatBinary();
}

void cnLBS_Get_ChatBinary(CNET_B308 *d) {
    *d = CnetSys_w.chatbin;
}

void __cnet_SendSet_ChatBinary(int arg0, int arg1) {
    SetSendCommand(&send_work, 0xF6);
    SetSendStringData2(&send_work, arg0, arg1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}

void _cnet_RecvFromLbs_NoticeChatBinary(void) {
    GetRecvDataOption3(CNWP(0x375C0), 0x300, GetRecvDataOption3(CNWP(0x375B8), 8, &recv_work));
    _cnetEvent_JumpCallBack(0xC, 0);
}

int cnLBS_Send_ChatMessageTU(int arg0, int arg1, int arg2, int arg3) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg3);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_ChatMessageTU(arg0, arg1, arg2);
        return slot;
    }
    return -1;
}

int __cnet_SendSet_ChatMessageTU(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0xF8) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendData8(&send_work, 0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerChatMessageTU(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeChatMessageTU(void) {
    memset(CnetSys_w.chat.from, 0, 0x120);
    GetRecvData8(&CnetSys_w.chat.d, GetRecvData8(&CnetSys_w.chat.c, GetRecvData8(&CnetSys_w.chat.b, GetRecvData8(&CnetSys_w.chat.a, GetRecvDataOption3(CnetSys_w.chat.msg, 0x100, GetRecvDataOption3(CnetSys_w.chat.x, 0x10, GetRecvDataOption3(CnetSys_w.chat.from, 8, recv_work)))))));
    _cnetEvent_JumpCallBack(0x2A, 0);
}

int cnLBS_Send_ChatBinaryTU(int arg0, int arg1, int arg2, int arg3) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg3);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendSet_ChatBinaryTU(arg0, arg1, arg2);
        return slot;
    }
    return -1;
}

int __cnet_SendSet_ChatBinaryTU(int arg0, int arg1, int arg2) {
    int cmd = SetSendCommand(&send_work, 0xFB) & 0xFFFF;
    SetSendStringData2(&send_work, arg0, 6);
    SetSendStringData2(&send_work, arg1, arg2);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_AnswerChatBinaryTU(void) {
    _cnet_Return_CallBack(0);
}

void _cnet_RecvFromLbs_NoticeChatBinaryTU(void) {
    GetRecvDataOption3(CNWP(0x375C0), 0x300, GetRecvDataOption3(CNWP(0x375B8), 8, &recv_work));
    _cnetEvent_JumpCallBack(0xC, 0);
}

void _cnet_RecvFromLbs_MatchStart(void) {
    _cnetEvent_JumpCallBack(1, 0);
}

int cnLBS_MatchEntry(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg1);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_MatchEntry(arg0);
        return slot;
    }
    return -1;
}

int __cnet_SendReq_MatchEntry(int arg0) {
    int cmd = SetSendCommand(&send_work, 0x9B) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_MatchEntry(void) {
    _cnet_Return_CallBack(0);
}

int cnLBS_MatchStart(void) {
    SetSendCommand(&send_work, 0xA1);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return 0;
}

int cnLBS_Read_MatchInfomation(int cb) {
    if (CnetSys_w.burst[7].state == 0) {
        memset(&CnetSys_w.matchinfo, 0, 0x5D4);
        __cnet_SendReq_MatchJoin();
        CnetSys_w.burst[7].cb = (void *)cb;
        CnetSys_w.burst[7].state = 1;
        CnetSys_w.burst[7].x21 = 0;
        CnetSys_w.burst[7].run = 0;
        return 0;
    }
    return -1;
}

void _cnet_RecvFromLbs_MatchJoin(void) {
    if (CnetSys_w.burst[7].state != 0) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                u8 v;
                __cnet_Recv_Byte(&v);
                CNW(u8, 0x30310) = v;
            } else {
                CNET_RES res;
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        __cnet_SendReq_MatchPlSide(0);
    }
}

void _cnet_RecvFromLbs_MatchPlSide(void) {
    CNET_RES res;
    u8 v;

    if (CnetSys_w.burst[7].state != 0) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                __cnet_Recv_Byte(&v);
                if (v != 0) v -= 1;
                CNW(u8, 0x30311) = v;
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        pl_infoget_ctr = 1;
        __cnet_SendReq_MatchOpponentInfo(1);
    }
}

void _cnet_RecvFromLbs_MatchOpponentInfo(void) {
    u8 idx;
    CNET_RES r;
    u8 *p;

    if (CNW(u8, 0xF34) != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                p = CNWP(0x30310);
                GetRecvData8(p + (idx - 1) * 0x98 + 0x1A9, GetRecvDataString(p + (idx - 1) * 0x98 + 0x170, GetRecvDataString(p + (idx - 1) * 0x98 + 0x130, GetRecvDataString(p + (idx - 1) * 0x98 + 0x11C, GetRecvDataString(p + (idx - 1) * 0x98 + 0x114, GetRecvData8(p + (idx - 1) * 0x98 + 0x1AA, GetRecvData8(&idx, recv_work)))))));
                (p + idx * 0x98)[0x110] = idx;
            } else {
                r.val = -1;
                __cnet_Recv_ServerMessage();
                __cnet_Return_MatchInformation(r);
                return;
            }
        }
        pl_infoget_ctr++;
        if (pl_infoget_ctr <= CNW(u8, 0x30310)) {
            __cnet_SendReq_MatchOpponentInfo(pl_infoget_ctr);
            return;
        }
        pl_infoget_ctr = 1;
        __cnet_SendReq_MatchOpponentStatus(1);
    }
}

void _cnet_RecvFromLbs_MatchOpponentStatus(void) {
    u8 *p;
    CNET_RES res;
    u8 idx;

    if (CNW(u8, 0xF34) != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                p = CNWP(0x30310);
                GetRecvData32(p + (idx - 1) * 0x98 + 0x1A4, GetRecvData32(p + (idx - 1) * 0x98 + 0x1A0, GetRecvData32(p + (idx - 1) * 0x98 + 0x19C, GetRecvData32(p + (idx - 1) * 0x98 + 0x198, GetRecvData32(p + (idx - 1) * 0x98 + 0x194, GetRecvData16(p + (idx - 1) * 0x98 + 0x190, GetRecvData8(&idx, recv_work)))))));
                (p + idx * 0x98)[0x110] = idx;
            } else {
                res.val = -1;
                __cnet_Recv_ServerMessage(CnetSys_w.rcat, recv_work);
                __cnet_Return_MatchInformation(res);
                return;
            }
        }
        pl_infoget_ctr++;
        if (pl_infoget_ctr <= CNW(u8, 0x30310)) {
            __cnet_SendReq_MatchOpponentStatus(pl_infoget_ctr);
            return;
        }
        __cnet_SendReq_MatchBattleCode(pl_infoget_ctr);
    }
}

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

void cnLBS_Get_GameServerAddress(u32 *addr, u16 *port) {
    int p;
    u32 v;
    v = CnetSys_w.gsaddr[0];
    v |= CnetSys_w.gsaddr[1] << 8 & 0xFF00;
    v = (CnetSys_w.gsaddr[2] << 16 & 0xFF0000) | v;
    v = (CnetSys_w.gsaddr[3] << 24 & 0xFF000000) | v;
    *addr = v;
    p = (CnetSys_w.gsport[1] + (CnetSys_w.gsport[0] << 8)) & 0xFFFF;
    *port = (p << 8 & 0xFF00) | (p >> 8 & 0xFF);
}

void _cnet_RecvFromLbs_NoticePatchStart(void) {
    if (CnetSys_w.burst[0].state != 0) {
        if (CnetSys_w.rcat == 16) {
            __cnet_Recv_PatchStart();
            CnetSys_w.patch_cnt = 0;
            CnetSys_w.x1004 = 0;
            CnetSys_w.patch_ptr = CNW(s32, 0x1054);
            return;
        }
        if (CnetSys_w.rcat == 2) {
            return;
        }
    }
}

void __cnet_Recv_PatchStart(void) {
    char b[0x18];

    memset(b, 0, 0x18);
    GetRecvData32(&CnetSys_w.patch_size, GetRecvData32(&CnetSys_w.patch_ver, GetRecvData16(&CnetSys_w.patch_x, GetRecvDataString(b, recv_work))));
    memset(&CnetSys_w.patch_b, 0, 8);
    memcpy(&CnetSys_w.patch_b, b, 4);
    memset(&CnetSys_w.patch_a, 0, 0x10);
    memcpy(&CnetSys_w.patch_a, b + 4, 0xA);
}

void _cnet_RecvFromLbs_NoticePatchData(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Recv_PatchData();
    }
}

void __cnet_Recv_PatchData(void) {
    u16 a;
    u16 b;

    GetRecvDataOption(CnetSys_w.patch_ptr, GetRecvData16(&a, GetRecvData16(&b, recv_work)), a);
    CnetSys_w.patch_ptr += a;
}

void _cnet_RecvFromLbs_ReqestPatchLineCheck(void) {
    u16 v;

    if (CnetSys_w.burst[0].state != 0) {
        __cnet_Recv_Word(&v);
        __cnet_Send_PatchLineCheck(v);
    }
}

int __cnet_Send_PatchLineCheck(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xC2) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_NoticePatchFooter(void) {

}

void _cnet_RecvFromLbs_RequestPatchFinish(void) {
    CNET_RES res;

    if (CnetSys_w.burst[0].state != 0) {
        if (__cnet_CheckCheckSum(CNW(s32, 0x1054), CnetSys_w.patch_ver, CnetSys_w.patch_size) != 0) {
            res.val = 0;
            res.id = 3;
            CnetSys_w.burst[0].cb(res, &res);
            return;
        }
        res.val = -1;
        res.id = 9;
        CnetSys_w.burst[0].cb(res, &res);
    }
}

int cnLBS_Answer_PatchFinish(void) {
    __cnet_Send_PatchFinish();
    return 0;
}

int __cnet_Send_PatchFinish(void) {
    int cmd = SetSendCommand(&send_work, 0xC4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Get_PatchInformation(u8 *p) {
    memset(p, 0, 0x1C);
    strncpy(p + 4, CnetSys_w.patch_a, 0xA);
    strncpy(p + 0x14, CnetSys_w.patch_b, 4);
    *(int *)p = CnetSys_w.patch_ver;
    return 0;
}

int __cnet_CheckCheckSum(p, size, sum)
u8 *p;
u32 size;
int sum;
{
    u32 i;
    int acc = 0;

    for (i = 0; i < size; i++) {
        acc += *p++;
    }
    return sum == acc;
}

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

void _cnet_Return_CallBack(arg)
int arg;
{
    CNET_RES res;

    if (CnetSys_w.rcat == 0x10) {
        if (arg != 0) {
            _cnetEvent_JumpCallBack(arg, 0);
        }
        return;
    }
    if (CnetSys_w.rcat == 2) {
        if (CnetSys_w.rres == 0) {
            res.val = 0;
        } else {
            res.val = -1;
            __cnet_Recv_ServerMessage();
        }
        __cnetSub_Return_BgProcess(res, 1, 0);
    }
}

void cnLBS_Init_LobbyBgProcess(void) {
    memset((u8 *)&CnetSys_w + 0x18, 0, 0xE00);
}

void cnLBS_Init_LobbyBgBurstProcess(void) {
    memset((u8 *)&CnetSys_w + 0xE18, 0, 0x1B0);
}

int __cnetSub_Set_BgProcess(kind, arg1, arg2)
s8 kind;
int arg1;
int arg2;
{
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 0) {
            CnetSys_w.bg[i].state = kind;
            CnetSys_w.bg[i].x19 = 0;
            CnetSys_w.bg[i].done = (void (*)())arg2;
            CnetSys_w.bg[i].cb = (void (*)())arg1;
            return i;
        }
    }
    return -1;
}

int __cnetSub_Return_BgProcess(CNET_RES res, int mode, int slot) {
    CNET_RES sp28 = res;
    int i;

    switch (mode) {
    case 1:
        for (i = 0; i < 0x80; i++) {
            if (mode == 1 && CnetSys_w.rseq == CnetSys_w.bg[i].cmd) {
                CnetSys_w.bg[i].state = 0;
                CnetSys_w.bg[i].x19 = 0;
                if (CnetSys_w.bg[i].done != 0) CnetSys_w.bg[i].done(sp28, &sp28, &CnetSys_w.bg[i]);
                return i;
            }
        }
        return -1;
    case 2:
        CnetSys_w.bg[slot].state = 0;
        CnetSys_w.bg[slot].x19 = 0;
        if (CnetSys_w.bg[slot].done != 0) CnetSys_w.bg[slot].done(sp28, &sp28);
        return slot;
    default:
        return -1;
    }
}

void __cnetSub_Run_BgProcess(void) {
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 2) {
            if (CnetSys_w.bg[i].cb != 0) CnetSys_w.bg[i].cb(i);
        }
    }
    for (i = 0; i < 12; i++) {
        if (CnetSys_w.burst[i].state == 1) {
            if (CnetSys_w.burst[i].run != 0) CnetSys_w.burst[i].run(i);
        }
    }
}

int __cnetSub_Get_RestBgWork(void) {
    int n = 0;
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 0) n++;
    }
    return n;
}

int __cnet_RecvFromLbs(int cmd, int from, int cat, int x) {
    int i;
    int c16;
    int c8;
    u8 *h;
    u8 *l;
    u8 *ft;
    u8 *ct;
    void (**jmp)();
    int hi;
    int full;

    c16 = cmd & 0xFFFF;
    c8 = cat & 0xFF;
    i = 0;
    h = lbs_command_tbl_h;
    l = lbs_command_tbl_l;
    ft = lbs_fromto_tbl;
    ct = lbs_category_tbl;
    jmp = lbs_command_jmp;
    for (; i < 0x102; i++, h++, l++, ft++, ct++, jmp++) {
        hi = (*h << 8) & 0xFFFF;
        full = (hi | *l) & 0xFFFF;
        if (*ft != 8 && c16 == (full & 0xFFFF) && *ct == c8 && *jmp != 0) {
            lbs_command_jmp[i](full, hi, c8, c16);
            return 1;
        }
    }
    return 0;
}

int cnLBS_RecvData(int sock) {
    int i;
    int got;

    if (CnetSys_w.active == 0) return 0;
    CnetSys_w.sock = sock;
    got = 0;
    i = 0;
    CnetSys_w.rcnt++;
    do {
        CnetSys_w.rlen = select_ps2(CnetSys_w.sock, recv_header, recv_work, 0x600);
        if (CnetSys_w.rlen != -1 && CnetSys_w.rlen != 0) {
            __cnetSub_RecvThreeData();
            got = 1;
        }
        i++;
    } while (i < 4);
    __cnetSub_Run_BgProcess();
    return got;
}

void __cnetSub_RecvThreeData(void) {
    int cmd = ((recv_header[2] << 8) & 0xFFFF) | recv_header[3];

    CnetSys_w.rcat = recv_header[1];
    CnetSys_w.rres = recv_header[8];
    CnetSys_w.rcmd = cmd;
    CnetSys_w.rseq = recv_header[6] << 8;
    CnetSys_w.rseq2 = recv_header[4] << 8;
    CnetSys_w.rseq = CnetSys_w.rseq | recv_header[7];
    CnetSys_w.rseq2 = CnetSys_w.rseq2 | recv_header[5];
    __cnet_RecvFromLbs(cmd & 0xFFFF, (recv_header[0] >> 4) & 0xF, recv_header[1], recv_header[5]);
}

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

int GetRecvDataString(dst, src)
char *dst;
u8 *src;
{
    u16 n;
    int m;

    n = (u16)(src[0] << 8) | src[1];
    memcpy(dst, src + 2, n);
    m = n & 0xFFFF;
    dst[m] = 0;
    return (int)(src + (m + 2));
}

int GetRecvDataOption(dst, src, len)
void *dst;
u8 *src;
u16 len;
{
    memcpy(dst, src, len);
    return (int)(src + len);
}

int GetRecvDataOption3(dst, maxlen, src)
void *dst;
u16 maxlen;
u8 *src;
{
    int t;
    int v;
    u16 hi;

    hi = src[0] << 8;
    v = (hi | src[1]) & 0xFFFF;
    t = v;
    if (maxlen < v) v = maxlen;
    if (v != 0) {
        memcpy(dst, src + 2, v & 0xFFFF);
    }
    return (int)(src + (t + 2));
}

void __cnet_Recv_ServerMessage(void) {
    memset(CnetSys_w.srvmsg, 0, 0x300);
    GetRecvDataOption3(CnetSys_w.srvmsg, 0x300, recv_work);
}

void __cnet_Recv_Byte(a0)
void *a0;
{
    GetRecvData8(a0, recv_work);
}

void __cnet_Recv_Word(a0)
void *a0;
{
    GetRecvData16(a0, recv_work);
}

void __cnet_Recv_Long(a0)
void *a0;
{
    GetRecvData32(a0, recv_work);
}

void __cnet_Recv_ByteString(a0, a1)
void *a0;
void *a1;
{
    GetRecvDataString(a1, GetRecvData8(a0, recv_work));
}

void __cnet_Recv_ByteByte(a0, a1)
void *a0;
void *a1;
{
    GetRecvData8(a1, GetRecvData8(a0, recv_work));
}

void __cnet_Recv_WordByte(a0, a1)
void *a0;
void *a1;
{
    GetRecvData8(a1, GetRecvData16(a0, recv_work));
}

void __cnet_Recv_WordWord(a0, a1)
void *a0;
void *a1;
{
    GetRecvData16(a1, GetRecvData16(a0, recv_work));
}

void __cnet_Recv_WordLong(a0, a1)
void *a0;
void *a1;
{
    GetRecvData32(a1, GetRecvData16(a0, recv_work));
}

void __cnet_Recv_ByteByteString(a0, a1, a2)
void *a0;
void *a1;
void *a2;
{
    GetRecvDataString(a2, GetRecvData8(a1, GetRecvData8(a0, recv_work)));
}

u16 SetSendCommand(w, cmd)
SEND_WORK *w;
int cmd;
{
    int c;

    memset(w->data, 0, 0x300);
    c = cmd & 0xFFFF;
    w->cmd_h = lbs_command_tbl_h[c];
    w->cmd_l = lbs_command_tbl_l[c];
    w->cat = lbs_category_tbl[c];
    w->total = 0;
    w->len = 0;
    w->magic = 0x81;
    if (w->cat == 2) {
        send_work.seq_h = recv_header[6];
        send_work.seq_l = recv_header[7];
    } else {
        seq_no++;
        w->seq_h = (int)seq_no >> 8;
        w->seq_l = seq_no;
    }
    w->x0D = 0xFF;
    w->x0E = 0xFF;
    w->x0F = 0xFF;
    w->x0C = 0;
    return seq_no;
}

void Mcs_SetSendCommand(w, cmd)
SEND_WORK *w;
int cmd;
{
    int c;

    memset(w->data, 0, 0x300);
    c = cmd & 0xFFFF;
    w->cmd_h = c >> 8;
    w->cmd_l = c;
    w->total = 0;
    w->len = 0;
    w->magic = 0x82;
    w->x0D = 0xFF;
    w->x0E = 0xFF;
    w->x0F = 0xFF;
    w->cat = 0;
    w->x0C = 0;
    memcpy(&w->seq_h, recv_header + 6, 2);
}

void SetSendCategory(w, v)
SEND_WORK *w;
s8 v;
{
    w->cat = v;
}

void SetSendResult(w, v)
SEND_WORK *w;
s8 v;
{
    w->x0C = v;
}

void SetSendCommandLen(w)
SEND_WORK *w;
{
    w->len_h = (int)w->len >> 8;
    w->len_l = w->len;
}

void SetSendData8(w, v)
SEND_WORK *w;
s8 v;
{
    *((u8 *)w + w->len + 0x10) = v;
    w->total += 1;
    w->len += 1;
}

void SetSendData16(w, v)
SEND_WORK *w;
int v;
{
    int b;
    SEND_WORK *t;

    b = v & 0xFFFF;
    t = (SEND_WORK *)((u8 *)w + w->len);
    t->data[0] = b >> 8;
    t->data[1] = b;
    w->total += 2;
    w->len += 2;
}

void SetSendData32(w, v)
SEND_WORK *w;
u32 v;
{
    u8 *t = (u8 *)w + w->len;

    t[0x10] = v >> 24;
    t[0x11] = v >> 16;
    t[0x12] = v >> 8;
    t[0x13] = v;
    w->total += 4;
    w->len += 4;
}

void SetSendStringData(w, src, len)
SEND_WORK *w;
void *src;
u16 len;
{
    memcpy((u8 *)w + w->len + 0x10, src, len);
    w->total += len;
    w->len += len;
}

void SetSendStringData2(w, src, len)
SEND_WORK *w;
char *src;
int len;
{
    char buf[0x108];
    int n;

    n = lbs_encode_ex(buf, src, (((w->seq_h << 8) & 0xFFFF) + w->seq_l) & 0xFFFF, len & 0xFFFF, CnetSys_w.xfee);
    SetSendData16(w, ((u16)len + 2) & 0xFFFF);
    SetSendData16(w, n & 0xFFFF);
    SetSendStringData(w, buf, len);
}

void SetSendEncodeStringData(w, src, len)
SEND_WORK *w;
char *src;
int len;
{
    char buf[0x108];
    int n;

    n = lbs_encode_ex(buf, src, (((w->seq_h << 8) & 0xFFFF) + w->seq_l) & 0xFFFF, len & 0xFFFF, CnetSys_w.xfee);
    SetSendData16(w, ((u16)len + 2) & 0xFFFF);
    SetSendData16(w, n & 0xFFFF);
    SetSendStringData(w, buf, len);
}

void Write_Socket(w)
u16 *w;
{
    CpInetTcpSend(CnetSys_w.sock, (u8 *)w + 4, (*w + 0xC) << 16 >> 16);
}

int lbs_encode_ex(out, in, key, len, extra)
u8 *out;
u8 *in;
int key;
int len;
int extra;
{
    int sum = 0;
    int i;

    if (out == 0 || in == 0) return -1;
    for (i = 0; i < len; i++) {
        out[i] = in[i] ^ encrypt_str[i & 7] ^ (extra + (key & 0xFF) + i);
        sum += in[i];
    }
    return sum & 0x7FFF;
}

static int write_col_numeric(buf, val, n)
char *buf;
int val;
int n;
{
    int i;

    buf += n - 1;
    for (i = 0; i < n; i++) {
        *buf = val % 10 + 0x30;
        buf--;
        val /= 10;
    }
    return 0;
}

static int read_col_numeric(str, n)
char *str;
int n;
{
    int v = 0;
    int i;
    int d;

    for (i = 0; i < n; i++) {
        char c = *str;
        if (c >= 0x30 && c < 0x3A) {
            d = c - 0x30;
            v = v * 10;
            v += d;
        }
        str++;
    }
    return v;
}

int mmbbc_encode(out, str, seq)
char *out;
char *str;
int seq;
{
    int a;
    int v1;
    int v2;

    if (out == 0 || str == 0) return -1;
    a = seq & 0xFFFF;
    v1 = a + read_col_numeric(str, 4);
    v2 = a + read_col_numeric(str + 4);
    write_col_numeric(out, v1, 5);
    write_col_numeric(out + 5, v2);
    out[10] = 0;
    return 0;
}
