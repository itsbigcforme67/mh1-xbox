/* G online (PC): check_room_require, written from Monster Hunter G's code at lobby 0x58A4D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00559C30();
extern u8 cw;
extern u8 lb_sys;
u16 *Lbs_GetRoomInfo();                           /* extern */
s32 lobby_func_0055E300();               /* extern */
extern s32 lb_pit;

s32 check_room_require(void) {
    s32 temp_a0;
    s32 temp_v0;
    u16 temp_s0;

    temp_s0 = *Lbs_GetRoomInfo((*(u8 *)((u8 *)&lb_sys + 0x7E)));
    temp_v0 = lobby_func_00559C30((s32) temp_s0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    temp_a0 = temp_s0 & 0xFFFF;
    if (temp_a0 >= 0xC350) {
        if ((*(s8 *)((u8 *)((*(void **)&cw)) + 0xB97)) != 0) {
            if ((*(u16 *)((u8 *)(temp_v0) + 0x1E)) != temp_a0) {
                goto block_4;
            }
            goto block_6;
        }
block_4:
        lb_pit = 0;
        (*(s8 *)((u8 *)&lb_pit + 0x8)) = 4;
        return 1;
    }
block_6:
    if (lobby_func_0055E300((*(u8 *)((u8 *)(temp_v0) + 0x1C)), 1) == 0) {
        return 1;
    }
    return 0;
}

