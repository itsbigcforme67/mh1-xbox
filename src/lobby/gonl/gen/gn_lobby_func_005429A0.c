/* G online (PC): lobby_func_005429A0, written from Monster Hunter G's code at lobby 0x5429A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 sub_main_func_004C85C0(); /* extern */
extern u8 D_005E7800;
extern u8 User_data;

s32 lobby_func_005429A0(void) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(u8 *)(g_frame + 0x20))
    u8 *var_s0;

    var_s0 = &sp20;
    (*(s64 *)((u8 *)(var_s0) + 0)) = (s64) (*(s64 *)((u8 *)(&D_005E7800) + 0));
    (*(f32 *)((u8 *)(var_s0) + 8)) = (f32) (*(f32 *)((u8 *)(&D_005E7800) + 8));
    if (sp20 != 0) {
loop_2:
        if (sub_main_func_004C85C0(&User_data, *var_s0, 0x14) == 0) {
            return 0;
        }
        var_s0 += 1;
        if (*var_s0 == 0) {
            goto block_6;
        }
        goto loop_2;
    }
block_6:
    return 1;
}

