/* cnlbs, run 10: _cnet_RecvFromLbs_NoticeLobbyLeaver .. __cnet_SendReq_MatchEntryUser (lobby.bin 0x005A9860-0x005A9AE8): the matching functions of cnlbs_nm.c. */
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
