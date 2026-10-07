/* cnlbs, run 9: _cnet_RecvFromLbs_NoticePatchStart .. __cnetSub_Set_BgProcess (lobby.bin 0x005ACC20-0x005AD308): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

void _cnet_RecvFromLbs_NoticePatchStart(void) {
    if (CnetSys_w.burst[0].state != 0) {
        if (CnetSys_w.rcat == 16) {
            __cnet_Recv_PatchStart();
            CnetSys_w.patch_cnt = 0;
            CnetSys_w.x1004 = 0;
            CnetSys_w.patch_ptr = CNW(s32, 0x1054);
            return;
        }
        if (CnetSys_w.rcat == 2) {
            return;
        }
    }
}

void __cnet_Recv_PatchStart(void) {
    char b[0x18];

    memset(b, 0, 0x18);
    GetRecvData32(&CnetSys_w.patch_size, GetRecvData32(&CnetSys_w.patch_ver, GetRecvData16(&CnetSys_w.patch_x, GetRecvDataString(b, recv_work))));
    memset(&CnetSys_w.patch_b, 0, 8);
    memcpy(&CnetSys_w.patch_b, b, 4);
    memset(&CnetSys_w.patch_a, 0, 0x10);
    memcpy(&CnetSys_w.patch_a, b + 4, 0xA);
}

void _cnet_RecvFromLbs_NoticePatchData(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Recv_PatchData();
    }
}

void __cnet_Recv_PatchData(void) {
    u16 a;
    u16 b;

    GetRecvDataOption(CnetSys_w.patch_ptr, GetRecvData16(&a, GetRecvData16(&b, recv_work)), a);
    CnetSys_w.patch_ptr += a;
}

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

int __cnet_CheckCheckSum(p, size, sum)
u8 *p;
u32 size;
int sum;
{
    u32 i;
    int acc = 0;

    for (i = 0; i < size; i++) {
        acc += *p++;
    }
    return sum == acc;
}

void _cnet_RecvFromLbs_RequestRegurationVersion(void) {

}

void _cnet_RecvFromLbs_NoticeRegurationAddress(void) {

}

void _cnet_RecvFromLbs_AnswerRegurationData(void) {
    _cnet_RecvFromLbs_AnswerBrowserMethodGet();
}

void cnLBS_Send_RegurationAgree(void) {

}

void _cnet_RecvFromLbs_AnswerRegurationAgree(void) {

}

void cnLBS_Set_CallBackNoticeEvent(int idx, void (*fn)()) {
    pFunc[idx] = fn;
}

void _cnetEvent_JumpCallBack(idx)
int idx;
{
    CNET_RES r;
    void (*fn)();

    r.id = idx;
    r.val = 1;
    fn = pFunc[(u16)idx];
    if (fn != 0) fn(r, 0);
}

void _cnet_Return_CallBack(arg)
int arg;
{
    CNET_RES res;

    if (CnetSys_w.rcat == 0x10) {
        if (arg != 0) {
            _cnetEvent_JumpCallBack(arg, 0);
        }
        return;
    }
    if (CnetSys_w.rcat == 2) {
        if (CnetSys_w.rres == 0) {
            res.val = 0;
        } else {
            res.val = -1;
            __cnet_Recv_ServerMessage();
        }
        __cnetSub_Return_BgProcess(res, 1, 0);
    }
}

void cnLBS_Init_LobbyBgProcess(void) {
    memset((u8 *)&CnetSys_w + 0x18, 0, 0xE00);
}

void cnLBS_Init_LobbyBgBurstProcess(void) {
    memset((u8 *)&CnetSys_w + 0xE18, 0, 0x1B0);
}

int __cnetSub_Set_BgProcess(kind, arg1, arg2)
s8 kind;
int arg1;
int arg2;
{
    int i;

    for (i = 0; i < 0x80; i++) {
        if (CnetSys_w.bg[i].state == 0) {
            CnetSys_w.bg[i].state = kind;
            CnetSys_w.bg[i].x19 = 0;
            CnetSys_w.bg[i].done = (void (*)())arg2;
            CnetSys_w.bg[i].cb = (void (*)())arg1;
            return i;
        }
    }
    return -1;
}
