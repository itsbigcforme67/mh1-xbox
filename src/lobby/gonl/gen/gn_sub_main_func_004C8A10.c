/* G online (PC): sub_main_func_004C8A10, written from Monster Hunter G's code at lobby 0x4C8A10 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s64 sub_main_func_004C9250();
extern s32 g_sub_4FF1D0;

s32 sub_main_func_004C8A10(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_a2;
    s32 temp_a3;
    s32 var_a5;
    u8 *var_a4;   /* (u16 *) */
    u16 var_a1;
    u8 *var_a2;

    if ((arg1 & 0xFF) >= 4) {
        return 0;
    }
    var_a4 = (*(s32 *)((u8 *)&g_sub_4FF1D0 + ((arg1 & 0xFF) * 4)));
    var_a1 = (*(u16 *)var_a4);
    temp_a2 = ((s16) (sub_main_func_004C9250(arg0, arg1, arg2, arg3)));
    if (var_a1 != 0xFF) {
        temp_a3 = ((s16) (temp_a2));
loop_5:
        var_a5 = 0;
        if (temp_a3 > 0) {
            var_a2 = arg0;
loop_7:
            if (((*(u8 *)((u8 *)(var_a2) + 0x44)) != 0) && ((*(u8 *)((u8 *)(var_a2) + 0x45)) == var_a1) && ((*(u16 *)((u8 *)(var_a2) + 0x46)) == (*(u16 *)((u8 *)(var_a4) + 2)))) {
                return 1;
            }
            var_a5 = ((s16) ((var_a5 + 1)));
            var_a2 += 6;
            if (var_a5 >= temp_a3) {
                goto block_12;
            }
            goto loop_7;
        }
block_12:
        var_a4 += 4;
        var_a1 = (*(u16 *)var_a4);
        if (var_a1 == 0xFF) {
            goto block_13;
        }
        goto loop_5;
    }
block_13:
    return 0;
}

