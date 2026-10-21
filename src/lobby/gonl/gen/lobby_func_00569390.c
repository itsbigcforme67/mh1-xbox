/* G online (PC): lobby_func_00569390, written from Monster Hunter G's code at lobby 0x569390 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnWrap_SoundRequest(); /* extern */

s64 lobby_func_00569390(u8 *arg0, s16 *arg1, s64 arg2, s64 arg3) {
    s16 temp_v0;
    s16 var_s1;
    s32 temp_hi;
    s32 temp_t1;
    s64 temp_a0;
    s64 temp_a1;
    s64 temp_a2;
    s64 temp_a2_2;
    s64 temp_a3;
    s64 temp_t0;
    s64 var_s0;
    s64 var_t2;
    u8 temp_v1;
    u8 var_s2;

    temp_a2 = (s64) (arg2 << 0x30) >> 0x30;
    temp_t1 = M2C_ERROR(/* Read from unset register $t0 */) & 0xFFFF;
    var_s1 = *arg1;
    var_s2 = *arg0;
    temp_v0 = var_s1;
    temp_v1 = var_s2;
    temp_a1 = (s64) ((var_s2 + (var_s1 * temp_a2)) << 0x30) >> 0x30;
    if (temp_t1 & 0x3C00) {
        temp_a0 = (s64) (arg3 << 0x30) >> 0x30;
        if (temp_a0 < 2) {
            goto block_3;
        }
        if (temp_a2 == 0) {
            M2C_BREAK(0);
        }
        temp_hi = temp_a0 % temp_a2;
        var_t2 = (s64) ((s64) (temp_a0 / temp_a2) << 0x30) >> 0x30;
        if (temp_a2 == 0) {
            M2C_BREAK(0);
        }
        if (temp_hi != 0) {
            var_t2 = (s64) ((var_t2 + 1) << 0x30) >> 0x30;
        }
        if (temp_t1 & 0x2000) {
            if (temp_v1 == 0) {
                if ((temp_v0 < (((s64) (var_t2 << 0x30) >> 0x30) - 1)) || (temp_hi == 0)) {
                    var_s2 = (temp_a2 - 1) & 0xFF;
                } else {
                    var_s2 = (temp_hi - 1) & 0xFF;
                }
            } else {
                var_s2 = (var_s2 - 1) & 0xFF;
            }
        } else if (temp_t1 & 0x1000) {
            if ((temp_v0 < (((s64) (var_t2 << 0x30) >> 0x30) - 1)) || (temp_hi == 0)) {
                var_s2 = (var_s2 + 1) & 0xFF;
                if ((s32) var_s2 >= temp_a2) {
                    var_s2 = 0;
                }
            } else {
                var_s2 = (var_s2 + 1) & 0xFF;
                if ((s32) var_s2 >= temp_hi) {
                    var_s2 = 0;
                }
            }
        } else if (temp_t1 & 0x800) {
            var_s1 = (s16) ((s64) ((var_s1 - 1) << 0x30) >> 0x30);
            if (var_s1 < 0) {
                var_s1 = (s16) ((s64) ((((s64) (var_t2 << 0x30) >> 0x30) - 1) << 0x30) >> 0x30);
            }
        } else if (temp_t1 & 0x400) {
            var_s1 = (s16) ((s64) ((var_s1 + 1) << 0x30) >> 0x30);
            if (var_s1 >= ((s64) (var_t2 << 0x30) >> 0x30)) {
                var_s1 = 0;
            }
        }
        temp_t0 = (s64) ((s64) var_s1 << 0x30) >> 0x30;
        temp_a3 = temp_t0 * temp_a2;
        temp_a2_2 = (var_s2 & 0xFF) + temp_a3;
        var_s0 = (s64) (temp_a2_2 << 0x30) >> 0x30;
        if (var_s0 >= temp_a0) {
            var_s2 = 0;
            var_s0 = (s64) (temp_a3 << 0x30) >> 0x30;
        }
        if ((temp_v1 == (var_s2 & 0xFF)) && (temp_v0 == temp_t0)) {
            return (s64) (temp_a1 << 0x30) >> 0x30;
        }
        cnWrap_SoundRequest(1, temp_a1, temp_a2_2, temp_a3);
        *arg1 = var_s1;
        *arg0 = var_s2;
        return (s64) (var_s0 << 0x30) >> 0x30;
    }
block_3:
    return (s64) (temp_a1 << 0x30) >> 0x30;
}

