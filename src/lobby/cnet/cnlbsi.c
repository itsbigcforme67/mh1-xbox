/* cnlbs, run 9: _cnet_RecvFromLbs_AnswerLobbyEntry .. __cnet_SendReq_RoomJoinInfo (lobby.bin 0x005A4930-0x005A51D8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

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
