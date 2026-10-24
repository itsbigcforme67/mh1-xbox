/* G online (PC): sub_main_func_004C9250, written from Monster Hunter G's code at lobby 0x4C9250 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s64 sub_main_func_004C9250(void *arg0) {
    u8 temp_v0;

    temp_v0 = (*(u8 *)((u8 *)(arg0) + 0xC97));
    return ((s16) ((((((temp_v0 & 1) + (((s32) temp_v0 >> 1) & 1)) & 0xFF) + 1) * 0x64)));
}

