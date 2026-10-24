/* G online (PC): sub_main_func_004C9410, written from Monster Hunter G's code at lobby 0x4C9410 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Quest_clear_bit_ck();
extern u16 D_004FEA50;

s32 sub_main_func_004C9410(void) {
    s32 var_s1;
    u8 *var_s0;   /* (u16 *) */

    var_s1 = 0;
    var_s0 = &D_004FEA50;
loop_1:
    if (Quest_clear_bit_ck((s32) (*(u16 *)var_s0)) == 0) {
        return 0;
    }
    var_s1 += 1;
    var_s0 += 2;
    if (var_s1 >= 0x32) {
        return 1;
    }
    goto loop_1;
}

