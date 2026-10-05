/* lb_bz23 - lobby UI/client 0x005B5D70-0x005B5DC4: cnLbc_Set_NgServerId (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 lbs_tryed_ctr;
extern char LbsTryedWork[];
extern char ConnectLbsId[];

void cnLbc_Set_NgServerId(void) {
    u8 temp_a0;

    temp_a0 = lbs_tryed_ctr;
    if (temp_a0 < 0xA) {
        memcpy((u8 *)&LbsTryedWork + (temp_a0 * 0x14), &ConnectLbsId, 0x14);
        lbs_tryed_ctr = (u8) (lbs_tryed_ctr + 1);
    }
}
