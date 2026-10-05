/* cnlbs, run 8: _cnet_CallBack_Result_Room_NumOfRoom .. _cnet_CallBack_Result_RoomJoinJoinUser (lobby.bin 0x005A7660-0x005A7704): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

void _cnet_CallBack_Result_Room_NumOfRoom(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[4].res = 1;
        return;
    }
    CnetSys_w.burst[4].res = 2;
}

void _cnet_CallBack_Result_RoomJoinJoinUser(CNET_RES res) {
    CNET_RES r;

    if (res.val == 0) {
        r.val = 2;
        r.id = 0xB;
        CnetSys_w.burst[4].cb(r, &r);
        CnetSys_w.burst[4].res = 1;
        return;
    }
    CnetSys_w.burst[4].res = 2;
}
