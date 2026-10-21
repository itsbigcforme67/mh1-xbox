/* G online (PC): lb_select_room_list, written from Monster Hunter G's code at lobby 0x58A580 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw2();
s32 Lb_cursorUD();
s32 cnWrap_SoundRequest();
void * lobby_func_00559C70();
extern u8 g_main_2EFAC0;
extern u8 cw;
extern u8 g_main_2EFAC0;
extern u8 lb_sys;
extern u8 pNet;
s32 Lbs_GetClassAdd();                              /* extern */
void *Lbs_GetRoomInfo();                          /* extern */

s32 lb_select_room_list(void) {
    s32 temp_a0;
    s32 temp_s0;
    s32 temp_v1;
    u16 temp_a0_2;
    u8 *temp_v0;
    u8 *temp_v0_2;

    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_a0 = Lbs_GetClassAdd() & 0xFFFF;
    temp_v1 = temp_s0 & 0xFFFF;
    if (temp_v1 & 0x20) {
        temp_v0 = Lbs_GetRoomInfo((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)));
        if ((*(u8 *)((u8 *)(temp_v0) + 0x16)) == 3) {
            if ((s32) (*(u16 *)((u8 *)(temp_v0) + 6)) >= (s32) (*(u16 *)((u8 *)(temp_v0) + 8))) {
                goto block_4;
            }
            temp_a0_2 = (*(u16 *)((u8 *)(temp_v0) + 0));
            if ((s32) temp_a0_2 >= 0xC350) {
                temp_v0_2 = lobby_func_00559C70((s32) temp_a0_2, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                if ((*(s8 *)((u8 *)((*(void **)&cw)) + 0xB97)) != 0) {
                    if (temp_v0_2 == NULL) {
                        goto block_9;
                    }
                    goto block_10;
                }
block_9:
                cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return 1;
            }
block_10:
            (*(u8 *)((u8 *)&lb_sys + 0x7E)) = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
            (*(s32 *)&g_main_2EFAC0) = (s32) (*(u16 *)((u8 *)(temp_v0) + 0));
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 0;
        }
block_4:
        cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return 2;
    }
    if (temp_v1 & 0x40) {
        cnWrap_SoundRequest(3, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return 3;
    }
    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)) = Lb_cursorUD((s32) (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)), temp_a0 & 0xFFFF, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    return 2;
}

