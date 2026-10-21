/* G online (PC): lobby_func_00571A90, written from Monster Hunter G's code at lobby 0x571A90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBAC;
extern u8 RecvMailInfo;

void lobby_func_00571A90(void) {
    s32 *var_a2;
    s64 var_a3;
    u8 *temp_a1;

    var_a3 = 0;
    var_a2 = &RecvMailInfo;
    (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x235)) = 0U;
loop_1:
    if ((*(s8 *)((u8 *)(var_a2) + 1)) != 0) {
        temp_a1 = (*(void **)&g_main_2EFBAC);
        var_a2 += 0x9A;
        var_a3 = (s64) ((var_a3 + 1) << 0x38) >> 0x38;
        (*(u8 *)((u8 *)(temp_a1) + 0x235)) = (u8) ((*(u8 *)((u8 *)(temp_a1) + 0x235)) + 1);
        if (var_a3 >= 8) {

        } else {
            goto loop_1;
        }
    }
}

