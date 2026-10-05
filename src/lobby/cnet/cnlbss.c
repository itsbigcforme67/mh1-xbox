/* cnlbs, run 19: _cnet_CallBack_Result_RoomSetFinish .. _cnet_CallBack_Result_RoomSetFinish (lobby.bin 0x005A83A0-0x005A83D8): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void _cnet_CallBack_Result_RoomSetFinish(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[6].res = 1;
        return;
    }
    CnetSys_w.burst[6].res = 2;
}
