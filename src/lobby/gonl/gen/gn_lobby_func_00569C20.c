/* G online (PC): lobby_func_00569C20, written from Monster Hunter G's code at lobby 0x569C20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ClassInfo;
extern u8 LobbyInfo;
extern u8 pNet;
s32 SetDialogData();         /* extern */
s32 SetDialogYesNo();                    /* extern */
s32 cnWrap_SoundRequest();      /* extern */

u8 lobby_func_00569C20(s32 arg0) {
    u8 temp_a1;
    u8 temp_v1;
    u8 *temp_a2;

    temp_v1 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&LobbyInfo + 0x16)) + (arg0 * 0x15C)));
    switch (temp_v1) {                              /* irregular */
    case 1:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        (*(s8 *)((u8 *)&ClassInfo + 0x4)) = arg0 + 1;
        SetDialogData(0x41, 1U);
        cnWrap_SoundRequest(0);
        return 6U;
    case 3:
        temp_a2 = (*(void **)&pNet);
        temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0x23));
        (*(u8 *)((u8 *)(temp_a2) + 0x26)) = temp_a1;
        (*(s8 *)((u8 *)&ClassInfo + 0x4)) = arg0 + 1;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        SetDialogData(0x3F, temp_a1, temp_a2);
        SetDialogYesNo(1);
        cnWrap_SoundRequest(0);
        return 0xBU;
    default:
        cnWrap_SoundRequest(7, 1);
        return (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23));
    }
}

