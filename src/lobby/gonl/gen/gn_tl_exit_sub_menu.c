/* G online (PC): tl_exit_sub_menu, written from Monster Hunter G's code at lobby 0x5733D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetHelpLineMsg();
s32 cnWrap_SoundRequest();
extern u8 pNet;
void tl_exit_sub_menu(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 *temp_a2;

    if (!(arg0 & 0xFF)) {
        cnWrap_SoundRequest(3, arg1, arg2, arg3);
    }
    SetHelpLineMsg((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)) + 2, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    temp_a2 = (*(void **)&pNet);
    (*(u8 *)((u8 *)(temp_a2) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_a2) + 0x22)) - 1);
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x24)) = 0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2B)) = 0;
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x32)) = 0;
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = 0;
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1E)) = 0;
    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)) = 0x10U;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 1;
}

