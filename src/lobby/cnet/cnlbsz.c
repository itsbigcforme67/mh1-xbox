/* cnlbs, run 26: _cnet_RecvFromLbs_NoticeRoomCommer .. _cnet_RecvFromLbs_NoticeRoomLeaver (lobby.bin 0x005A5880-0x005A5898): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_NoticeRoomCommer(void) {
    _sub_InOutRoomMember(2);
}

void _cnet_RecvFromLbs_NoticeRoomLeaver(void) {
    _sub_InOutRoomMember(3);
}
