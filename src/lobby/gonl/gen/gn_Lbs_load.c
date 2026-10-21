/* G online (PC): Lbs_load, written from Monster Hunter G's code at lobby 0x579B60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 func_0010EF50();
s32 load_pit();
extern u8 NET_CON_TEX;
void Lbs_load(void) {
    load_pit();
    func_0010EF50((*(s32 *)&NET_CON_TEX), 0x174, 0);
}

