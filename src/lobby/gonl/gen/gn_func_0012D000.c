/* G online (PC): func_0012D000, written from Monster Hunter G's code at lobby 0x12D000 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 Psw;

u16 func_0012D000(s32 arg0) {
    return *(&(*(u8 *)((u8 *)&Psw + 0x4)) + (arg0 * 0x22));
}

