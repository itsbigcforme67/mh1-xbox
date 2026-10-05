/* cnlbs - lobby.bin network layer 0x005A2A20-0x005AE320: the protocol layer of the online lobby client.
 * cnLBS_* start a request, __cnet_SendReq_* build the packet, _cnet_RecvFromLbs_* handle replies. */
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

void __cnet_Recv_SearchUser(void) {
    char sp10[8];

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

int cnLBS_Get_ConditionSearchUser(void **arg0) {
    *arg0 = CNWP(0x39D8C);
    return 0;
}

int __cnet_Send_ConditionSearchUserCertify(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xEE) & 0xFFFF;
    SetSendData8(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
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

void __cnet_KeepEntryFloorInfo(kind, val)
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

void _cnet_RecvFromLbs_AnswerRoomCreate(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}

int cnLBS_Set_RoomRuleFinish(int arg0) {
    int slot = __cnetSub_Set_BgProcess(1, 0, arg0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RoomSetFinish();
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

int cnLBS_Read_RoomRuleCaption(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleListHeadWord(arg0, arg1);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomRuleChoiceCount(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleNumOfChoice(arg0, arg1);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomRuleNow(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_RuleListNow(arg0, arg1);
        return slot;
    }
    return -1;
}

int cnLBS_Read_RoomRuleChoicePermission(int arg0, int arg1) {
    int slot = __cnetSub_Set_BgProcess(1, 0);

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

void _cnet_CallBack_Result_RoomSetFinish(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[6].res = 1;
        return;
    }
    CnetSys_w.burst[6].res = 2;
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
    int sp5C;
    char name[8];
    int i;
    int p;
    int k;
    u8 *s2;

    if (CnetSys_w.rres == 0) {
        p = GetRecvData8(&cnt, GetRecvData16(&sp5C, recv_work));
        if (cnt != 0) {
            i = 0;
            if ((cnt & 0xFF) > 0) {
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

int cnLBS_Get_CurrentPlace(s16 *d) {
    s16 *t = CnetSys_w.curplace;

    d[0] = t[0];
    d[1] = t[1];
    d[2] = t[2];
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
    CnetSys_w.firstdata.h[14] = (u16)CnetSys_w.echo_sum >> 2;
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
    if (n >= 4) n = 3;
    CnetSys_w.n_login_user = n;
    i = 0;
    if (n > 0) {
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
    u8 v;
    CNET_RES res;

    if (CnetSys_w.burst[7].state != 0) {
        if (CnetSys_w.rcat == 2) {
            if (CnetSys_w.rres == 0) {
                __cnet_Recv_Byte(&v);
                CNW(u8, 0x30310) = v;
            } else {
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
    u8 v;
    CNET_RES res;

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

void _cnet_RecvFromLbs_MatchBattleCode(void) {
    CNET_RES res;
    u8 *p;

    if (CnetSys_w.burst[7].state != 0 && CnetSys_w.rcat != 0x10) {
        if (CnetSys_w.rcat == 2) {
            p = CNWP(0x30312);
            if (CnetSys_w.rres == 0) {
                GetRecvDataString(p, recv_work);
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
            p = CNWP(0x30323);
            if (CnetSys_w.rres == 0) {
                GetRecvDataString(p, recv_work);
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

    *addr = (CnetSys_w.gsaddr[3] << 24 & 0xFF000000) | ((CnetSys_w.gsaddr[2] << 16 & 0xFF0000) | (CnetSys_w.gsaddr[0] | (CnetSys_w.gsaddr[1] << 8 & 0xFF00)));
    p = (CnetSys_w.gsport[1] + (CnetSys_w.gsport[0] << 8)) & 0xFFFF;
    *port = (p << 8 & 0xFF00) | (p >> 8 & 0xFF);
}

void _cnet_RecvFromLbs_NoticePatchData(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Recv_PatchData();
    }
}

void __cnet_Recv_PatchData(void) {
    u16 a = 0;
    u16 b = 0;

    GetRecvDataOption(CNW(s32, 0xFFC), GetRecvData16(&a, GetRecvData16(&b, recv_work)), a);
    CNW(s32, 0xFFC) += a;
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
    } else if (CnetSys_w.rcat == 2) {
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
            got = 1;
            __cnetSub_RecvThreeData();
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
    int n;
    int m;

    n = (((src[0] << 8) & 0xFFFF) | src[1]) & 0xFFFF;
    memcpy(dst, src + 2, n);
    m = n & 0xFFFF;
    dst[m] = 0;
    return (int)(src + (m + 2));
}

int GetRecvDataOption(dst, src, len)
void *dst;
u8 *src;
int len;
{
    memcpy(dst, src, len & 0xFFFF);
    return (int)src + (len & 0xFFFF);
}

int GetRecvDataOption3(dst, maxlen, src)
void *dst;
int maxlen;
u8 *src;
{
    int t;
    int m;
    int v;

    m = maxlen & 0xFFFF;
    v = (((src[0] << 8) & 0xFFFF) | src[1]) & 0xFFFF;
    t = v;
    if (m < v) v = m;
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
int len;
{
    memcpy((u8 *)w + w->len + 0x10, src, len & 0xFFFF);
    w->total += len;
    w->len += len;
}

void SetSendStringData2(w, src, len)
SEND_WORK *w;
char *src;
int len;
{
    char buf[0x100];
    int n;

    n = lbs_encode_ex(buf, src, (((w->seq_h << 8) & 0xFFFF) + w->seq_l) & 0xFFFF, len & 0xFFFF, CnetSys_w.xfee);
    SetSendData16(w, ((len & 0xFFFF) + 2) & 0xFFFF);
    SetSendData16(w, n & 0xFFFF);
    SetSendStringData(w, buf, len);
}

void SetSendEncodeStringData(w, src, len)
SEND_WORK *w;
char *src;
int len;
{
    char buf[0x100];
    int n;

    n = lbs_encode_ex(buf, src, (((w->seq_h << 8) & 0xFFFF) + w->seq_l) & 0xFFFF, len & 0xFFFF, CnetSys_w.xfee);
    SetSendData16(w, ((len & 0xFFFF) + 2) & 0xFFFF);
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

    for (i = 0; i < n; i++) {
        char c = *str;
        if (c >= 0x30 && c < 0x3A) {
            v = (c - 0x30) + v * 10;
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
