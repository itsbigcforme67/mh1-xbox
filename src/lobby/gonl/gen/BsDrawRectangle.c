/* G online (PC): BsDrawRectangle, written from Monster Hunter G's code at lobby 0x5B46E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

f32 BsSetRenderState();             /* extern */
s32 net_flps0004();                        /* extern */

void BsDrawRectangle(f32 arg1, f32 arg3, s32 arg0) {
    s32 sp38;
    s16 sp36;
    s16 sp34;
    s16 sp32;
    union { s16 v; u8 b[0x10]; } u_sp30;
#define sp30 (u_sp30.v)
    f32 temp_f0;

    if (((s32) (((s64) (M2C_ERROR(/* Read from unset register $f0 */) << 0x30) >> 0x30) - 4) % 5) == 0) {

    }
    if (((s32) (((s64) (M2C_ERROR(/* Read from unset register $f0 */) << 0x30) >> 0x30) - 4) % 5) == 0) {

    }
    temp_f0 = BsSetRenderState(1, 0);
    sp38 = arg0;
    sp30 = M2C_BITWISE(s16, temp_f0);
    sp32 = M2C_BITWISE(s16, temp_f0);
    sp34 = M2C_BITWISE(s16, temp_f0);
    sp36 = M2C_BITWISE(s16, temp_f0);
    net_flps0004(&sp30);
}

