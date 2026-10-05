/* cnlbs, run 28: cnLBS_Send_UserMiniData .. _cnet_RecvFromLbs_NoticeLoginOk (lobby.bin 0x005AA790-0x005AA8FC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

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
