/* G online (PC): lobby_func_005AE500, written from Monster Hunter G's code at lobby 0x5AE500 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005AE5D0();
extern u8 CnetSys_w;
s32 memset();        /* extern */

s32 lobby_func_005AE500(s8 *arg0, s32 arg1) {
    u8 g_frame[0x180] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s8 *)(g_frame + 0x30))
    s32 var_v1;
    s32 var_v1_2;
    s8 *var_a0;
    s8 *var_a0_2;
    s8 *var_a1;
    s8 *var_a1_2;

    var_a0 = arg0;
    var_v1 = 0x14B;
    var_a1 = &sp30;
    do {
        var_v1 -= 1;
        *var_a1 = *var_a0;
        var_a0 += 1;
        var_a1 += 1;
    } while (var_v1 > 0);
    if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0))) + 0x20)) == 0) {
        memset(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0)), 0, 0x28);
        var_a1_2 = &sp30;
        var_a0_2 = &(*(s8 *)((u8 *)&CnetSys_w + 0x12CA));
        var_v1_2 = 0x14B;
        do {
            var_v1_2 -= 1;
            *var_a0_2 = *var_a1_2;
            var_a1_2 += 1;
            var_a0_2 += 1;
        } while (var_v1_2 > 0);
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0))) + 0x20)) = 1U;
        (*(s32 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0))) + 4)) = arg1;
        (*(void (**)())((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0))) + 0)) = lobby_func_005AE5D0;
        return 0;
    }
    return -1;
}

