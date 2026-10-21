/* G online (PC): lobby_func_005437F0, written from Monster Hunter G's code at lobby 0x5437F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Quest_clear_bit_ck(); /* extern */
s32 ran_suu();                           /* extern */
extern u16 D_005E7810;
extern s32 g_lb_5E7812;

s32 lobby_func_005437F0(s32 arg0) {
    s32 temp_a3;
    s32 var_a5;
    u8 *temp_a2;   /* (u16 *) */
    u8 *var_a4;   /* (u16 *) */
    u16 temp_a0;

    ran_suu(1);
    if (D_005E7810 != 0xFFFF) {
        temp_a3 = arg0 & 0xFFFF;
        var_a5 = 0;
        var_a4 = &D_005E7810;
loop_2:
        temp_a2 = &(&D_005E7810)[var_a5];
        temp_a0 = (*(u16 *)((u8 *)(temp_a2) + 0));
        if (temp_a3 == temp_a0) {
            return 0;
        }
        if (temp_a3 == (*(u16 *)((u8 *)(temp_a2) + 2))) {
            return Quest_clear_bit_ck(temp_a0, 0xFFFF, temp_a2, temp_a3, var_a4, var_a5) != 1;
        }
        if (temp_a3 == (*(u16 *)((u8 *)(temp_a2) + 4))) {
            if ((Quest_clear_bit_ck(temp_a0, 0xFFFF, temp_a2, temp_a3, var_a4, var_a5) == 1) && (Quest_clear_bit_ck((*(s32 *)((u8 *)&g_lb_5E7812 + (var_a5 * 2)))) == 1)) {
                return 0;
            }
            return 1;
        }
        var_a4 += 6;
        var_a5 += 3;
        if ((*(u16 *)var_a4) == 0xFFFF) {
            goto block_13;
        }
        goto loop_2;
    }
block_13:
    return 0;
}

