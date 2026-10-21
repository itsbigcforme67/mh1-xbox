/* G online (PC): lobby_func_005E6D20, written from Monster Hunter G's code at lobby 0x5E6D20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_lb_733CAA;

void lobby_func_005E6D20(void *arg0) {
    s32 *var_a3;
    s16 temp_v1;
    s32 var_a2;
    u8 *var_a0;

    var_a0 = arg0;
    var_a2 = 0x64;
    var_a3 = &g_lb_733CAA;
    do {
        var_a2 -= 1;
        temp_v1 = (*(s16 *)((u8 *)(var_a3) + 2));
        (*(s16 *)((u8 *)(var_a0) + 0)) = (s16) (*(s16 *)((u8 *)(var_a3) + 0));
        var_a3 += 4;
        (*(s16 *)((u8 *)(var_a0) + 2)) = temp_v1;
        var_a0 += 4;
    } while (var_a2 > 0);
}

