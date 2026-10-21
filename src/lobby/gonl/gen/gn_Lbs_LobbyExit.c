/* G online (PC): Lbs_LobbyExit, written from Monster Hunter G's code at lobby 0x583000 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Lobby_LobbyExit();
s32 Check_CallBackWait();
extern u8 cw;
s32 cnLBS_LobbyExit();              /* extern */

s32 Lbs_LobbyExit(void) {
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_3;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        /* fallthrough */
    case 0x1:
        temp_v1_3 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_3) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9D)) + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x26;
        cnLBS_LobbyExit(CallBack_Result_Lobby_LobbyExit);
block_15:
    default:
        return 2;
    case 0x2:
        Check_CallBackWait();
        goto block_15;
    case 0x3:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

