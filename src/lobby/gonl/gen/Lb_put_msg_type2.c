/* G online (PC): Lb_put_msg_type2, written from Monster Hunter G's code at lobby 0x5934C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntLocate();                              /* extern */
s32 func_0012BDD0();                         /* extern */
extern u8 g_lb_636CF0;

void Lb_put_msg_type2(s64 arg2) {
    s64 temp_s0;

    temp_s0 = (s64) (arg2 << 0x30) >> 0x30;
    if (temp_s0 < 0x4E) {
        flfntLocate();
        func_0012BDD0(*(&g_lb_636CF0 + (temp_s0 * 4)));
    }
}

