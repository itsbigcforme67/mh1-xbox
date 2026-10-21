/* G online (PC): CallBack_Result_Lobby_JoinUser, written from Monster Hunter G's code at lobby 0x580E30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLbc_CheckInFloorOrder();
extern u8 cw;
s32 gonl_Get_LobbyJoinUser(); /* extern */
extern u8 g_main_3034BA;
extern u8 LobbyInfo;

void CallBack_Result_Lobby_JoinUser(s64 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    s32 temp_a0;
    s32 temp_v0;
    u8 *temp_v0_2;

    sp18 = arg0;
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB99)) != 5) {
        temp_v0 = cnLbc_CheckInFloorOrder(1, arg1, arg2, arg3);
        temp_a0 = temp_v0 - 1;
        if ((s8) sp18 == 0) {
            temp_v0_2 = &LobbyInfo + (temp_a0 * 0x15C);
            gonl_Get_LobbyJoinUser(temp_v0 & 0xFFFF, temp_v0_2 + 6, temp_v0_2 + 0x12);
            return;
        }
        *(&g_main_3034BA + (temp_v0 * 0x15C)) = 0;
    }
}

