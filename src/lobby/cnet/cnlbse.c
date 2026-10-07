/* cnlbs, run 5: _cnet_CallBack_Result_RoomSetFinish .. _cnet_RecvFromLbs_NoticeLoginOk (lobby.bin 0x005A83A0-0x005AA8FC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

typedef struct { s8 val; u8 pad[6]; } R7;

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
