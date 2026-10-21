/* G online (PC): lobby_func_005737C0, written from Monster Hunter G's code at lobby 0x5737C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntSetSize();
s32 font_print_double();
s64 lobby_func_005935C0();
void lobby_func_005737C0(void *arg0) {
    s32 temp_s0;

    temp_s0 = ((s16) (lobby_func_005935C0(arg0, 0x1A6, 0x14)));
    flfntSetSize(0x14, 0x14);
    font_print_double((s32) temp_s0, 0xBC, 1, 4, arg0);
}

