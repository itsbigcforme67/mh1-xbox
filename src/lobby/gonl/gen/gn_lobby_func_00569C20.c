/* G online (PC): lobby_func_00569C20, written from Monster Hunter G's code at lobby 0x569C20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
s32 SetDialogYesNo();
s32 cnWrap_SoundRequest();
extern u8 ClassInfo;
extern u8 LobbyInfo;
extern u8 pNet;

u8 lobby_func_00569C20(s32 arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    u8 temp_a1;
    u8 temp_v1;
    u8 *temp_a2;

    temp_v1 = *(&(*(u8 *)((u8 *)&LobbyInfo + 0x16)) + (arg0 * 0x15C));
    switch (temp_v1) {                              /* irregular */
    case 1:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        (*(s8 *)((u8 *)&ClassInfo + 0x4)) = arg0 + 1;
        SetDialogData(0x41, 1, arg2, arg3);
        cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return 6U;
    case 3:
        temp_a2 = (*(void **)&pNet);
        temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0x23));
        (*(u8 *)((u8 *)(temp_a2) + 0x26)) = temp_a1;
        (*(s8 *)((u8 *)&ClassInfo + 0x4)) = arg0 + 1;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        SetDialogData(0x3F, (s32) temp_a1, (s32) temp_a2, arg3);
        SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return 0xBU;
    default:
        cnWrap_SoundRequest(7, 1, arg2, arg3);
        return (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23));
    }
}

