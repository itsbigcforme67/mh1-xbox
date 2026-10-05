/* cnlbs, run 8: _cnet_RecvFromLbs_NoticeRoomMatchEntryTypeList .. cnLBS_Read_CurrentPlace (lobby.bin 0x005A9C00-0x005AA008): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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
