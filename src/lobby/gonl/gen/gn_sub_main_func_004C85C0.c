/* G online (PC): sub_main_func_004C85C0, written from Monster Hunter G's code at lobby 0x4C85C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 sub_main_func_004C8870();
s32 sub_main_func_004C85C0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    return (sub_main_func_004C8870(arg0, arg1, arg2, arg3) & 0xFFFF) >= (arg2 & 0xFFFF);
}

