/* cnlbs, run 6: _cnet_CallBack_Result_RoomSetFinish .. __cnet_Recv_LobbyMember (lobby.bin 0x005A83A0-0x005A973C): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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
