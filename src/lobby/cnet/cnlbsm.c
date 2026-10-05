/* cnlbs, run 13: cnLBS_Set_RoomProperty .. _cnet_RecvFromLbs_NoticeRoomLeaver (lobby.bin 0x005A5740-0x005A5898): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

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
