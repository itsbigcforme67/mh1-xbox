/* G online (PC): lobby_client_game_ready, written from Monster Hunter G's code at lobby 0x583A40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_InterruptFlag();
s32 Disp_NowLoading2();
extern u8 cw;
extern u8 pNet;
extern u8 lobby_client_game_ready_jmp_2838;

void lobby_client_game_ready(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (Check_InterruptFlag(arg0, arg1, arg2, arg3) == 0) {
        *(&lobby_client_game_ready_jmp_2838 + ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) * 4))();
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 0;
        Disp_NowLoading2(4);
    }
}

