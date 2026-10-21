/* G online (PC): lobby_func_005C15F0, written from Monster Hunter G's code at lobby 0x5C15F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005C15F0(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_a3;
    s32 temp_v1;
    s32 var_a0;
    s32 var_t2;
    s8 temp_v0;
    u8 *var_a1;
    u8 *var_a2;
    u8 *var_t0;
    u8 *var_t1;

    temp_a3 = arg2 & 0xFF;
    temp_v1 = arg1 & 0xFF;
    if (temp_v1 >= temp_a3) {
        return 0;
    }
    var_t2 = (temp_v1 + 1) & 0xFF;
    if (temp_a3 >= var_t2) {
        var_t1 = arg0 + (var_t2 * 0x18);
        var_t0 = var_t1;
        do {
            var_a1 = var_t0 - 0x18;
            if (var_t2 < temp_a3) {
                var_a0 = 0xC;
                var_a2 = var_t1;
                do {
                    var_a0 -= 1;
                    temp_v0 = (*(s8 *)((u8 *)(var_a2) + 1));
                    (*(s8 *)((u8 *)(var_a1) + 0)) = (s8) (*(s8 *)((u8 *)(var_a2) + 0));
                    var_a2 += 2;
                    (*(s8 *)((u8 *)(var_a1) + 1)) = temp_v0;
                    var_a1 += 2;
                } while (var_a0 > 0);
                (*(s8 *)((u8 *)(var_t1) + 0)) = 0;
                (*(s8 *)((u8 *)(var_t1) + 8)) = 0;
            } else {
                (*(s8 *)((u8 *)(var_t0) + -0x18)) = 0;
                (*(s8 *)((u8 *)(var_t0) + -0x10)) = 0;
            }
            var_t2 = (var_t2 + 1) & 0xFF;
            var_t1 += 0x18;
            var_t0 += 0x18;
        } while (temp_a3 >= var_t2);
    }
    return 1;
}

