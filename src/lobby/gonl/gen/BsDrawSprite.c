/* G online (PC): BsDrawSprite, written from Monster Hunter G's code at lobby 0x5B47F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 BsSetRenderState();         /* extern */
s32 SetFilterMode();                     /* extern */
s32 flSetRenderState();         /* extern */
s32 net_flps0008();                        /* extern */

void BsDrawSprite(s16 arg0, s32 arg1, s16 arg2, s16 arg3) {
    s16 spA2;
    s16 spA0;
    s16 sp9E;
    s16 sp9C;
    s32 sp98;
    s16 sp96;
    s16 sp94;
    s16 sp92;
    union { s16 v; u8 b[0x20]; } u_sp90;
#define sp90 (u_sp90.v)

    flSetRenderState(0x6C, 0);
    SetFilterMode(1);
    BsSetRenderState(4, arg1);
    sp98 = (s32) arg0;
    sp90 = arg2;
    sp92 = arg3;
    sp94 = M2C_ERROR(/* Read from unset register $t0 */);
    sp96 = M2C_ERROR(/* Read from unset register $t1 */);
    spA0 = arg0 - 1;
    spA2 = arg2 - 1;
    sp9C = M2C_ERROR(/* Read from unset register $t2 */);
    sp9E = M2C_ERROR(/* Read from unset register $t3 */);
    net_flps0008(&sp90);
    SetFilterMode(0);
}

