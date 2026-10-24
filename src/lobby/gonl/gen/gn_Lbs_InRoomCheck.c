/* G online (PC): Lbs_InRoomCheck (G lobby 0x583170; HAND-FIXED: m2c could not take the leaf apart): 1 when this player is
 * in a room (cw+0xFA5F, MH1 cw+0x35D3). docs/network.md 3.12. */
#include "../gonl.h"

extern u8 *cw;

s32 Lbs_InRoomCheck(void)
{
    return cw[0xFA5F] != 0;
}
