/* cnlbs, run 7: _cnet_RecvFromLbs_NoticeLobbyLeaver .. cnLBS_Read_CurrentPlace (lobby.bin 0x005A9860-0x005AA008): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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
