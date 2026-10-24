/* G online (PC): lobby_func_005434A0, written from Monster Hunter G's code at lobby 0x5434A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u16 D_005E7810;

s32 lobby_func_005434A0(s32 arg0) {
    s32 temp_a2;
    s32 var_a4;
    u8 *temp_a0;   /* (u16 *) */
    u8 *var_a3;   /* (u16 *) */

    if (D_005E7810 != 0xFFFF) {
        temp_a2 = arg0 & 0xFFFF;
        var_a4 = 0;
        var_a3 = &D_005E7810;
loop_2:
        temp_a0 = &(&D_005E7810)[var_a4];
        if (temp_a2 == (*(u16 *)((u8 *)(temp_a0) + 2))) {
            return 1;
        }
        if (temp_a2 == (*(u16 *)((u8 *)(temp_a0) + 4))) {
            return 2;
        }
        var_a3 += 6;
        var_a4 += 3;
        if ((*(u16 *)var_a3) == 0xFFFF) {
            goto block_9;
        }
        goto loop_2;
    }
block_9:
    return 0;
}

