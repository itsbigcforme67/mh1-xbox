/* cnlbs, run 56: __cnet_Send_PatchLineCheck .. _cnet_RecvFromLbs_NoticePatchFooter (lobby.bin 0x005ACE30-0x005ACEA8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int __cnet_Send_PatchLineCheck(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xC2) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_NoticePatchFooter(void) {

}
