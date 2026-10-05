/* cnlbs, run 31: _cnet_RecvFromLbs_RequestTelephoneNumber .. _cnet_RecvFromLbs_RequestBattleResult (lobby.bin 0x005AAE30-0x005AAE7C): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

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
