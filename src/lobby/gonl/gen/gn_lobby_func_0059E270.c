/* G online (PC): lobby_func_0059E270, written from Monster Hunter G's code at lobby 0x59E270 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 vs_square();                                /* extern */

s32 lobby_func_0059E270(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7) {   /* a jump to vs_square */
    return (s32) vs_square(a0, a1, a2, a3, a4, a5, a6, a7);
}

