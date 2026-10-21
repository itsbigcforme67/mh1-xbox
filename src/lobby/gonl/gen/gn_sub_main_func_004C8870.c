/* G online (PC): sub_main_func_004C8870, written from Monster Hunter G's code at lobby 0x4C8870 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 sub_main_func_004C8580();                /* extern */
extern s32 g_sub_4FEE90;

s32 sub_main_func_004C8870(s32 arg0, s32 arg1) {
    s32 temp_v0;
    s32 var_s0;
    u8 *var_s2;
    u8 var_a1;

    var_s2 = (*(s32 *)((u8 *)&g_sub_4FEE90 + ((arg1 & 0xFF) * 4)));
    var_a1 = *var_s2;
    var_s0 = 0;
    if (var_a1 != 0xFF) {
        do {
            var_s2 += 1;
            temp_v0 = var_s0 + sub_main_func_004C8580(arg0, var_a1);
            var_a1 = *var_s2;
            var_s0 = temp_v0 & 0xFFFF;
        } while (var_a1 != 0xFF);
    }
    if ((var_s0 & 0xFFFF) >= 0x2710) {
        var_s0 = 0x270F;
    }
    return var_s0;
}

