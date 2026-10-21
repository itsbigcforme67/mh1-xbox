/* G online (PC): lobby_func_005E65F0, written from Monster Hunter G's code at lobby 0x5E65F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 __cnetSub_Set_BgProcess();
s32 lobby_func_005E66A0();
extern u8 pFunc;

s32 lobby_func_005E65F0(void *arg0, s32 arg1) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(s32 *)(g_frame + 0x20))
    u8 *var_a3;   /* (s32 *) */
    s16 temp_v0;
    s32 temp_v0_2;
    s32 var_a1;
    s32 var_v0;
    u8 *var_a0;

    var_a0 = arg0;
    var_a3 = &sp20;
    var_a1 = 9;
    do {
        var_a1 -= 1;
        temp_v0 = (*(s16 *)((u8 *)(var_a0) + 2));
        (*(s16 *)((u8 *)(var_a3) + 0)) = (s16) (*(s16 *)((u8 *)(var_a0) + 0));
        var_a0 += 4;
        (*(s16 *)((u8 *)(var_a3) + 2)) = temp_v0;
        var_a3 += 4;
    } while (var_a1 > 0);
    temp_v0_2 = __cnetSub_Set_BgProcess(1, 0, arg1);
    var_v0 = -1;
    if (temp_v0_2 != -1) {
        (*(u16 *)((u8 *)&(*(u16 *)((u8 *)&pFunc + 0xEE)) + (temp_v0_2 * 0x1C))) = lobby_func_005E66A0(&sp20, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        var_v0 = temp_v0_2;
    }
    return var_v0;
}

