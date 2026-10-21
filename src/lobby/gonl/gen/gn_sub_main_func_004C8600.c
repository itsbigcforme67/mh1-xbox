/* G online (PC): sub_main_func_004C8600, written from Monster Hunter G's code at lobby 0x4C8600 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 sub_main_func_004C87C0();                /* extern */

s32 sub_main_func_004C8600(s32 unused_a0, s32 unused_a1, s32 arg2) {
    return (sub_main_func_004C87C0(0) & 0xFFFF) >= (arg2 & 0xFFFF);
}

