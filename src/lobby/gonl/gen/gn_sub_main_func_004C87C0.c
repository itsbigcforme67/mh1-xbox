/* G online (PC): sub_main_func_004C87C0, written from Monster Hunter G's code at lobby 0x4C87C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 sub_main_func_004C85A0();               /* extern */
extern s32 g_sub_4FEE90;

s32 sub_main_func_004C87C0(s32 arg0, s32 arg1, u8 *arg2) {
    s32 temp_v1;
    s32 var_s1;
    u8 *var_s0;
    u8 var_v1;

    var_s0 = (*(s32 *)((u8 *)&g_sub_4FEE90 + ((arg1 & 0xFF) * 4)));
    var_v1 = *var_s0;
    var_s1 = 0;
    if (var_v1 != 0xFF) {
        do {
            temp_v1 = sub_main_func_004C85A0(arg0, var_v1 & 0xFF) & 0xFFFF;
            if ((var_s1 & 0xFFFF) < temp_v1) {
                var_s1 = temp_v1;
                if (arg2 != NULL) {
                    *arg2 = *var_s0;
                }
            }
            var_s0 += 1;
            var_v1 = *var_s0;
        } while (var_v1 != 0xFF);
    }
    return var_s1;
}

