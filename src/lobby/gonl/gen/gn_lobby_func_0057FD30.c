/* G online (PC): lobby_func_0057FD30, written from Monster Hunter G's code at lobby 0x57FD30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Plaza_ReadLobbyAllocation();
s32 Check_CallBackWait();
extern u8 cw;
s32 gonl_Read_LobbyAllocation(); /* extern */

s32 lobby_func_0057FD30(s32 arg0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    u8 temp_v1;
    u8 *temp_a1;
    u8 *temp_a2;
    u8 *temp_v1_2;

    temp_a1 = (*(void **)&cw);
    temp_v1 = (*(u8 *)((u8 *)(temp_a1) + 0xB9D));
    temp_a2 = temp_a1 + 0xB9D;
    switch (temp_v1) {                              /* irregular */
    case 0:
        (*(s8 *)((u8 *)(temp_a1) + 0xBA9)) = 0xE;
        temp_v1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_2) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9D)) + 1);
        gonl_Read_LobbyAllocation(((((s16) (arg0))) + 1) & 0xFFFF, CallBack_Result_Plaza_ReadLobbyAllocation, temp_a2);
block_9:
    default:
        return 0;
    case 1:
        Check_CallBackWait((s32) arg0, (s32) temp_a1, (s32) temp_a2, arg3);
        goto block_9;
    case 2:
        (*(u8 *)((u8 *)(temp_a1) + 0xB9D)) = 0U;
        return 1;
    }
}

