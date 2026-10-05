/* cnlbs, run 33: cnLBS_Get_AllocationProgressCount .. cnLBS_Get_AllocationProgressCount (lobby.bin 0x005A7B00-0x005A7B14): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

int cnLBS_Get_AllocationProgressCount(u16 *arg0) {
    *arg0 = CNW(u16, 0x1032);
    return 0;
}
