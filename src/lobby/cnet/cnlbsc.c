/* cnlbs, run 3: cnLBS_Get_ConditionSearchUser .. cnLBS_Get_ConditionSearchUser (lobby.bin 0x005A2F90-0x005A2FA4): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"
#pragma readonly_strings on

int cnLBS_Get_ConditionSearchUser(void **arg0) {
    *arg0 = CNWP(0x39D8C);
    return 0;
}
