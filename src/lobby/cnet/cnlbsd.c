/* cnlbs, run 4: _sub_ReceiveJoinUser .. _cnet_RecvFromLbs_AnswerRuleListName (lobby.bin 0x005A59E0-0x005A69BC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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
