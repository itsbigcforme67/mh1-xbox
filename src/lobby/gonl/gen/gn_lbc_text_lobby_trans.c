/* G online (PC): lbc_text_lobby_trans, written from Monster Hunter G's code at lobby 0x57A120 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 add_prim2();
extern u8 pNet;
extern s32 g_main_2EF31C;
extern u8 ot2;
extern u8 ot5;
extern u8 ot6;
extern u8 ot7;

void lbc_text_lobby_trans(void) {
    u8 *temp_a1;
    u8 *temp_a1_2;
    u8 *temp_a1_3;
    u8 *temp_a1_4;
    u8 *temp_a1_5;

    temp_a1 = (*(void **)((u8 *)((*(void **)&pNet)) + 0));
    if ((temp_a1 != NULL) && ((*(s32 *)((u8 *)(temp_a1) + 0x14)) != 0)) {
        add_prim2(&ot5, temp_a1, 0, 1);
    }
    temp_a1_2 = (*(void **)((u8 *)((*(void **)&pNet)) + 4));
    if ((temp_a1_2 != NULL) && ((*(s32 *)((u8 *)(temp_a1_2) + 0x14)) != 0)) {
        add_prim2(&ot6, temp_a1_2, 0, 1);
    }
    temp_a1_3 = (*(void **)((u8 *)((*(void **)&pNet)) + 8));
    if ((temp_a1_3 != NULL) && ((*(s32 *)((u8 *)(temp_a1_3) + 0x14)) != 0)) {
        add_prim2(&ot7, temp_a1_3, 0, 1);
    }
    temp_a1_4 = (*(void **)((u8 *)((*(void **)&pNet)) + 0xC));
    if ((temp_a1_4 != NULL) && ((*(s32 *)((u8 *)(temp_a1_4) + 0x14)) != 0)) {
        add_prim2(&ot2, temp_a1_4, 0, 0x10);
    }
    temp_a1_5 = (*(void **)((u8 *)((*(void **)&pNet)) + 0x10));
    if ((temp_a1_5 != NULL) && ((*(s32 *)((u8 *)(temp_a1_5) + 0x14)) != 0)) {
        add_prim2(&g_main_2EF31C, temp_a1_5, 0, 1);
    }
}

