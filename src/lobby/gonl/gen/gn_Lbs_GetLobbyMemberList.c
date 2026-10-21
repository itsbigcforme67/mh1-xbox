/* G online (PC): Lbs_GetLobbyMemberList, written from Monster Hunter G's code at lobby 0x57F730 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 lobby_func_0057F610();
s32 lobby_func_0057F6A0();
s32 lobby_func_005ADC70();
s32 lobby_func_005ADDA0();
extern u8 cw;
s32 Lbs_GetLobbyMemberList(s32 arg0) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x10;
        lobby_func_005ADC70(((arg0 & 0xFFFF) + 1) & 0xFFFF, (s32) lobby_func_0057F610);
block_16:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait();
        goto block_16;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x11;
        lobby_func_005ADDA0(((arg0 & 0xFFFF) + 1) & 0xFFFF, (s32) lobby_func_0057F6A0);
        goto block_16;
    case 0x3:
        Check_CallBackWait();
        goto block_16;
    case 0x4:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

