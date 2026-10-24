/* G online (PC): Lbc_DownloadQuest, written from Monster Hunter G's code at lobby 0x581610 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 cnLBS_Get_ServerMessage();
s32 lobby_func_005817C0();
s32 lobby_func_005BFD60();
extern u8 g_main_2EF390;
extern u8 cw;
extern u8 g_main_2EF390;
extern u8 mission_area;
void *get_quest_info();               /* extern */

s32 Lbc_DownloadQuest(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_a2;
    u8 temp_v1;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_v0;
    u8 *temp_v1_2;

    temp_a0 = (*(void **)&cw);
    temp_v1 = (*(u8 *)((u8 *)(temp_a0) + 0xB9D));
    temp_a1 = temp_a0 + 0xB9D;
    switch (temp_v1) {                              /* irregular */
    case 0x0:
        temp_v0 = get_quest_info(temp_a0, temp_a1);
        if ((temp_v0 != NULL) && ((*(u16 *)((u8 *)(temp_v0) + 0x1E)) == (arg1 & 0xFFFF))) {
            return 0;
        }
        temp_v1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_2) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9D)) + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        temp_a2 = ((s8) (arg0));
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x1C;
        lobby_func_005BFD60((temp_a2 + 1) & 0xFF, (*(s32 *)&mission_area), (*(s32 *)((u8 *)(((*(s32 *)&g_main_2EF390) + 0x23F4 + (temp_a2 * 8))) + 8)), (s32) lobby_func_005817C0);
block_15:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        goto block_15;
    case 0x2:
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = 0U;
        return 0;
    case 0x63:
        cnLBS_Get_ServerMessage(temp_a0 + 0xF75D);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
        return 1;
    }
}

