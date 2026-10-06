/* lb_bz22 - lobby UI/client 0x005B5D30-0x005B5D64: cnLbc_Init_NgServerId (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 lbs_tryed_ctr;
extern char LbsTryedWork[];
extern char CnetWork[];

void cnLbc_Init_NgServerId(void) {
    memset(&LbsTryedWork, 0, 0xC8);
    lbs_tryed_ctr = 0;
    F(s8, &CnetWork, 6) = 0;
}
