/* G online (PC): plaza_log_id_chk_sub, written from Monster Hunter G's code at lobby 0x58F3E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 strcmp();
s32 plaza_log_id_chk_sub(void *arg0, s32 *arg1, s32 arg2) {
    u8 *var_s1;   /* (s32 *) */
    s32 var_s0;

    var_s1 = arg1;
    var_s0 = arg2;
    if (arg2 != 0) {
loop_2:
        if (strcmp((*(s32 *)var_s1) + 0x46, arg0) == 0) {
            return 0;
        }
        var_s0 -= 1;
        var_s1 += 4;
        if (var_s0 == 0) {
            goto block_6;
        }
        goto loop_2;
    }
block_6:
    return 1;
}

