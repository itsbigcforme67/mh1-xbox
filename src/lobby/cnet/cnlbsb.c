/* cnlbs, run 2: _cnet_CallBack_Result_LobbyCount .. _cnet_CallBack_Result_LobbyAllocation (lobby.bin 0x005A7240-0x005A72E4): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on


typedef struct { s16 a, b, c; } CPLACE3;

typedef struct { s8 val; u8 pad[6]; } R7;

void _cnet_CallBack_Result_LobbyCount(CNET_RES res) {
    if (res.val == 0) {
        CnetSys_w.burst[3].res = 1;
        return;
    }
    CnetSys_w.burst[3].res = 2;
}

void _cnet_CallBack_Result_LobbyAllocation(CNET_RES res) {
    CNET_RES r;

    if (res.val == 0) {
        r.val = 2;
        r.id = 0xB;
        CnetSys_w.burst[3].cb(r, &r);
        CnetSys_w.burst[3].res = 1;
        return;
    }
    CnetSys_w.burst[3].res = 2;
}
