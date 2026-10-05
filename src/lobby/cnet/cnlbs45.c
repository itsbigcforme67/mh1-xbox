/* cnlbs, run 46: _cnet_RecvFromLbs_NoticeMiniData .. _cnet_RecvFromLbs_NoticeLoginOk (lobby.bin 0x005AA860-0x005AA8FC): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_NoticeMiniData(void) {
    if (CnetSys_w.rres == 0) {
        memset(CnetSys_w.minidata, 0, 0x5C);
        GetRecvDataOption3(&CnetSys_w.minidata[0x1C], 0x40, GetRecvDataOption3(CnetSys_w.minidata, 8, recv_work));
    }
    _cnet_Return_CallBack(0x2C);
}

void _cnet_RecvFromLbs_NoticeLoginOk(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Login_Return();
    }
}
