/* G online (PC): lb_put_comment, written from Monster Hunter G's code at lobby 0x578440 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Draw_square(); /* extern */
s32 KinshiYogo_chk();                        /* extern */
s32 func_0012DE30();      /* extern */

void lb_put_comment(s64 arg0, s64 arg1, s32 arg2, s64 arg3) {
    s64 temp_s0;
    s64 temp_s1;

    if ((((s8) (arg3))) != 0) {
        temp_s1 = ((s16) (arg1));
        temp_s0 = ((s16) (arg0));
        Draw_square(((s16) ((temp_s0 - 6))), ((s16) ((temp_s1 - 2))), 0x134, 0x42, 0xFFAA8820);
        Draw_square(((s16) ((temp_s0 - 7))), ((s16) ((temp_s1 - 3))), 0x136, 0x44, 0xFFAA8820);
    }
    KinshiYogo_chk(arg2);
    func_0012DE30(arg0, ((s16) (((((s16) (arg1))) - 0x16))), 0x16, arg2);
}

