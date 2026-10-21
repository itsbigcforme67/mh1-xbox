/* G online (PC): lbc_top_menu_02, written from Monster Hunter G's code at lobby 0x57E080 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBackWaitInit();
s32 Check_CallBackWait();
s32 cnLbc_CheckInFloorOrder();
s32 lobby_client_game_in_plaza();
s32 lobby_func_005E39E0();
extern u8 cw;
s32 lbc_top_menu_02(void) {
    u8 temp_v1_2;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_v1_2) {                            /* irregular */
    case 0x0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit();
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 7;
        lobby_func_005E39E0(cnLbc_CheckInFloorOrder(0) & 0xFFFF, (s32) lobby_client_game_in_plaza);
block_12:
    default:
        return 2;
    case 0x1:
        Check_CallBackWait();
        goto block_12;
    case 0x2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = 0U;
        return 0;
    case 0x63:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = 0U;
        return 1;
    }
}

