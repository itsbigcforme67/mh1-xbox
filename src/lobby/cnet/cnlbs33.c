/* cnlbs, run 34: _cnet_RecvFromLbs_NoticePatchData .. _cnet_RecvFromLbs_NoticePatchData (lobby.bin 0x005ACD60-0x005ACD8C): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void _cnet_RecvFromLbs_NoticePatchData(void) {
    if (CNW(u8, 0xE38) != 0) {
        __cnet_Recv_PatchData();
    }
}
