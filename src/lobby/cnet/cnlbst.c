/* cnlbs, run 20: __cnet_Recv_PatchStart .. _cnet_RecvFromLbs_NoticePatchData (lobby.bin 0x005ACCA0-0x005ACD8C): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

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
