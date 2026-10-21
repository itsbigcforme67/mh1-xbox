/* G online (PC): lobby_func_005926C0, written from Monster Hunter G's code at lobby 0x5926C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 System_timer;
s32 flSetRenderState();         /* extern */
f32 flSin(f32);                                     /* extern */
s32 flps0005();                            /* extern */

void lobby_func_005926C0(s64 arg0, s64 arg1, s64 arg2, s64 arg3, s64 arg4, s32 arg5) {
    u8 g_frame[0x90] __attribute__((aligned(16)));   /* G's stack frame */
#define sp70 (*(s16 *)(g_frame + 0x70))
#define sp72 (*(s16 *)(g_frame + 0x72))
#define sp74 (*(s16 *)(g_frame + 0x74))
#define sp76 (*(s16 *)(g_frame + 0x76))
#define sp78 (*(s32 *)(g_frame + 0x78))
#define sp7C (*(s32 *)(g_frame + 0x7C))
#define sp80 (*(s32 *)(g_frame + 0x80))
#define sp84 (*(s32 *)(g_frame + 0x84))
    f32 temp_f0;
    f32 var_f1;
    s32 temp_v0;
    u64 temp_a1;
    u64 temp_a1_2;

    temp_v0 = (((*(u16 *)&System_timer) & 0x3F) << 0xA) & 0xFFFF;
    if (temp_v0 >= 0) {
        var_f1 = (f32) temp_v0;
    } else {
        var_f1 = 2.0f * (f32) (((u32) temp_v0 >> 1) | (temp_v0 & 1));
    }
    temp_f0 = flSin(0.0000958738f * var_f1);
    sp70 = (s16) arg0;
    sp72 = (s16) arg1;
    sp74 = (((s16) (arg0))) + (((s16) (arg2)));
    sp76 = (((s16) (arg1))) + (((s16) (arg3)));
    if (!(arg5 & 0xFF)) {
        temp_a1 = (u64) (arg4 << 0x28) >> 0x28;
        sp78 = temp_a1 | (((((s8) (M2C_BITWISE(s64, (48.0f * temp_f0))))) + 0x90) << 0x18);
        sp7C = (s32) temp_a1;
    } else {
        temp_a1_2 = (u64) (arg4 << 0x28) >> 0x28;
        sp78 = (s32) temp_a1_2;
        sp7C = temp_a1_2 | (((((s8) (M2C_BITWISE(s64, (48.0f * temp_f0))))) + 0x90) << 0x18);
    }
    sp80 = sp78;
    sp84 = sp7C;
    flSetRenderState(0x60, 0);
    flps0005(&sp70);
}

