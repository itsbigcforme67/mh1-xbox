/* G online (PC): Lb_put_msg_type2, written from Monster Hunter G's code at lobby 0x5934C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntLocate();
s32 func_0012BDD0();                         /* extern */
extern s32 g_lb_636CF0;

void Lb_put_msg_type2(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_s0;

    temp_s0 = ((s16) (arg2));
    if (temp_s0 < 0x4E) {
        flfntLocate(arg0, arg1);
        func_0012BDD0((*(s32 *)((u8 *)&g_lb_636CF0 + (temp_s0 * 4))));
    }
}

