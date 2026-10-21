/* G online (PC): func_0012E1C0, written from Monster Hunter G's code at lobby 0x12E1C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 ran_suu();
f32 flSin(u8, f32);                                 /* extern */
extern u8 g_main_25F250;
extern u8 g_main_2EF264;
extern u8 g_main_2EF268;

s32 func_0012E1C0(s32 arg0) {
    f32 temp_f1;
    f32 var_v1;
    s32 var_s0;
    u8 temp_s1;

    if (arg0 != 0) {
        g_main_2EF264 = 0;
        g_main_2EF268 = 8;
    }
    var_s0 = 0xFFFFFF;
    if (g_main_2EF268 == 8) {
        var_s0 = 0xFFFF00;
    }
    temp_s1 = g_main_2EF264;
    g_main_2EF264 += (((s32) (ran_suu(1) & 0xFFFF & 0xE) >> 1) + 0x32) & 0xFF;
    if ((s32) g_main_2EF264 < (temp_s1 & 0xFFFF)) {
        g_main_2EF268 = ran_suu(1) & 0xFFFF & 7;
    }
    temp_f1 = 256.0f * (0.5f * (1.0f + flSin(g_main_2EF264, 2.0f * (3.1415927f * (((360.0f * (f32) (g_main_2EF264 << 8)) / 65536.0f) / 360.0f))))) * *(&g_main_25F250 + (g_main_2EF268 * 4));
    if (!(temp_f1 >= 2.1474836e9f)) {
        var_v1 = temp_f1;
    } else {
        var_v1 = M2C_BITWISE(f32, (M2C_BITWISE(s32, (temp_f1 - 2.1474836e9f)) | 0x80000000));
    }
    return ((((M2C_BITWISE(s32, var_v1) & 0xFF) + 0x50) & 0xFF) << 0x18) + var_s0;
}

