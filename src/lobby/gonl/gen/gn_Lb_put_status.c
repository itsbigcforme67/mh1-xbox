/* G online (PC): Lb_put_status, written from Monster Hunter G's code at lobby 0x593220 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 jtbl_0063E9E0;
s32 Lb_put_icon_free();           /* extern */

void Lb_put_status(s64 arg4, s32 *arg5) {
    u8 *var_a5;   /* (s32 *) */
    s64 temp_v1;
    s64 var_a4;

    var_a4 = arg4;
    var_a5 = arg5;
    temp_v1 = ((s16) (var_a4));
    if ((u32) temp_v1 < 7U) {
        var_a5 = &jtbl_0063E9E0;
        switch (temp_v1) {
            return;
        case 5:
            var_a4 = 0x10;
            goto block_10;
        case 2:
            var_a4 = 0x11;
            goto block_10;
        case 1:
            var_a4 = 8;
            goto block_10;
        case 4:
            var_a4 = 0x12;
            goto block_10;
        case 6:
            var_a4 = 0x13;
            goto block_10;
        case 3:
            var_a4 = 0x2F;
            goto block_10;
        }
    } else {
block_10:
        Lb_put_icon_free(var_a4, var_a5);
    }
}

