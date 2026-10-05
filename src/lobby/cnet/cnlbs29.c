/* cnlbs, run 30: __cnet_Login_Return .. _cnet_RecvFromLbs_RequestBattleResult (lobby.bin 0x005AADE0-0x005AAE7C): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void __cnet_Login_Return(void) {
    CNET_RES res;

    if (CnetSys_w.burst[0].state != 0) {
        res.val = 0;
        CnetSys_w.burst[0].state = 0;
        res.id = 0;
        CnetSys_w.burst[0].x21 = 0;
        CnetSys_w.burst[0].cb(res, &res);
    }
}

void _cnet_RecvFromLbs_RequestTelephoneNumber(void) {
    __cnet_SendSet_TelephoneNumber();
}

void _cnet_RecvFromLbs_RequestPersonalDataRegist(void) {

}

void _cnet_RecvFromLbs_RequestBattleResult(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_SendAns_BattleResult();
    }
}
