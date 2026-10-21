/* G online (PC): Lb_put_status, written from Monster Hunter G's code at lobby 0x593220 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_icon_free();
void Lb_put_status(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s64 arg4) {
    s64 temp_v1;
    s64 var_a4;

    var_a4 = arg4;
    temp_v1 = ((s16) (var_a4));
    switch (temp_v1) {
        return;
    case 5:
        var_a4 = 0x10;
    default:
block_10:
        Lb_put_icon_free(arg0, arg1, arg2, arg3, var_a4);
        return;
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
}

