/* G online (PC): Lbs_GetRoomInfo (G lobby 0x581B20; HAND-FIXED: a leaf, written from the asm): room record idx (0x15C
 * bytes) of G's room list. G's records start at 0x6DE120, 4 bytes before the split's RoomInfo symbol (MH1's function
 * returned &RoomInfo + ..., so every field read by G's room list code was 4 bytes off: the status at +0x16, members
 * +6, maximum +8, quest +0). docs/network.md 3.12. */
#include "../gonl.h"

extern u8 g_lb_6DE120[];

u8 *Lbs_GetRoomInfo(s32 idx)
{
    return g_lb_6DE120 + (s16)idx * 0x15C;
}
