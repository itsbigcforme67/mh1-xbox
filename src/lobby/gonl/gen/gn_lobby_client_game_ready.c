/* G online (PC): lobby_client_game_ready, written from Monster Hunter G's code at lobby 0x583A40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_InterruptFlag();
s32 Disp_NowLoading2();
extern u8 cw;
extern u8 pNet;
s32 lbc_game_ready_00();
s32 lbc_game_ready_01();
s32 lbc_game_ready_02();
s32 lbc_game_ready_03();
s32 lbc_game_ready_04();
static s32 (*const lobby_client_game_ready_jmp_2838_g[])() = { lbc_game_ready_00, lbc_game_ready_01, lbc_game_ready_02, lbc_game_ready_03, lbc_game_ready_04 };   /* G's table of functions, by name */
extern u8 lobby_client_game_ready_jmp_2838;

void lobby_client_game_ready(void) {
    if (Check_InterruptFlag() == 0) {
        lobby_client_game_ready_jmp_2838_g[(*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9B))]();
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 0;
        Disp_NowLoading2(4);
    }
}

