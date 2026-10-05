/* cnlbs, run 3: __cnet_Send_ConditionSearchUserCertify .. _cnet_RecvFromLbs_NoticeRoomLeaver (lobby.bin 0x005A3180-0x005A5898): the matching functions of cnlbs_nm.c. */
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
