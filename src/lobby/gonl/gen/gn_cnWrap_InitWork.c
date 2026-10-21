/* G online (PC): cnWrap_InitWork, written from Monster Hunter G's code at lobby 0x5C0BB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Net_all_reset();
s32 init_set_work();
extern u8 cw;
void cnWrap_InitWork(void) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB98)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xEE0)) = 0;
    init_set_work();
    Net_all_reset();
}

