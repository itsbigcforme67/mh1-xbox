/* cnlbs - lobby.bin 0x005A2A20-0x005AE320. The lobby client's network layer:
 * cnLBS_* entry points start a background request (slot in CnetSys_w.bg),
 * __cnet_SendReq_* builds and writes the packet, _cnet_RecvFromLbs_* handle
 * the reply, and the CallBack_Result_* functions deliver it. */
#include "lbnet.h"

int __cnetSub_Set_BgProcess();
void __cnetSub_Return_BgProcess();
int SetSendCommand();
void SetSendCommandLen();
void SetSendData16();
void SetSendStringData2();
void Write_Socket();
void _cnet_Return_CallBack();

int __cnet_SendReq_PersonalDataChange(void);
int __cnet_SendReq_PieceCount();
int __cnet_SendReq_PieceName();
int __cnet_SendReq_RoomJoinInfo();

int __cnet_SendReq_PersonalDataChange(void) {
    int cmd = SetSendCommand(&send_work, 0xB2) & 0xFFFF;

    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_RequestPersonalDataChange(int cb) {
    int slot = __cnetSub_Set_BgProcess(1, 0, cb);

    if (slot != -1) {
        CnetSys_w.bg[slot].cmd = __cnet_SendReq_PersonalDataChange();
        return slot;
    }
    return -1;
}
