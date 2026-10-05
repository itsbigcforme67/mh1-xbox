/* cnlbs, run 8: _cnet_RecvFromLbs_AnswerPlazaEntry .. _cnet_RecvFromLbs_AnswerLobbyExplain (lobby.bin 0x005A41A0-0x005A4894): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

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
