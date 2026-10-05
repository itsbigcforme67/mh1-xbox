/* cnlbs, run 19: _cnet_RecvFromLbs_ReqestPatchLineCheck .. cnLBS_Get_PatchInformation (lobby.bin 0x005ACDF0-0x005AD024): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void _cnet_RecvFromLbs_ReqestPatchLineCheck(void) {
    u16 v;

    if (CnetSys_w.burst[0].state != 0) {
        __cnet_Recv_Word(&v);
        __cnet_Send_PatchLineCheck(v);
    }
}

int __cnet_Send_PatchLineCheck(int arg0) {
    int cmd = SetSendCommand(&send_work, 0xC2) & 0xFFFF;
    SetSendData16(&send_work, arg0);
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

void _cnet_RecvFromLbs_NoticePatchFooter(void) {

}

void _cnet_RecvFromLbs_RequestPatchFinish(void) {
    CNET_RES res;

    if (CnetSys_w.burst[0].state != 0) {
        if (__cnet_CheckCheckSum(CNW(s32, 0x1054), CnetSys_w.patch_ver, CnetSys_w.patch_size) != 0) {
            res.val = 0;
            res.id = 3;
            CnetSys_w.burst[0].cb(res, &res);
            return;
        }
        res.val = -1;
        res.id = 9;
        CnetSys_w.burst[0].cb(res, &res);
    }
}

int cnLBS_Answer_PatchFinish(void) {
    __cnet_Send_PatchFinish();
    return 0;
}

int __cnet_Send_PatchFinish(void) {
    int cmd = SetSendCommand(&send_work, 0xC4) & 0xFFFF;
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
    return cmd;
}

int cnLBS_Get_PatchInformation(u8 *p) {
    memset(p, 0, 0x1C);
    strncpy(p + 4, CnetSys_w.patch_a, 0xA);
    strncpy(p + 0x14, CnetSys_w.patch_b, 4);
    *(int *)p = CnetSys_w.patch_ver;
    return 0;
}
