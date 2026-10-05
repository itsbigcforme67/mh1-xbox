/* cnlbs, run 10: _cnet_RecvFromLbs_AnswerRoomCreate .. _cnet_RecvFromLbs_AnswerRoomCreate (lobby.bin 0x005A5320-0x005A5354): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_AnswerRoomCreate(void) {
    if (CNW(s8, 0xFEC) == 0) {
        __cnet_SetEntryFloorInfo(2);
    }
    _cnet_Return_CallBack(0);
}
